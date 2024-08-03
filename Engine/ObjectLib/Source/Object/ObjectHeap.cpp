#include "Containers/HashTable.h"
#include "Engine/Logging.h"
#include "HAL/Timer.h"
#include "Object/Object.h"
#include "Object/ObjectHeap.h"
#include "Object/ObjectHeapBlock.h"
#include "Memory/Memory.h"
#include "Misc/StringBuilder.h"

static TArray<TUniquePtr<FObjectHeapBlock>> GObjectHeapBlocks;
static int64 GNumObjectsMarkedForDeletion = 0;

/**
 * @brief Gets the heap block that an object header belongs to.
 *
 * @param objectHeader The object header.
 * @return The owning heap block.
 */
static FObjectHeapBlock* GetHeapBlockForObjectHeader(FObjectHeader* objectHeader)
{
	if (objectHeader == nullptr)
	{
		return nullptr;
	}

	for (TUniquePtr<FObjectHeapBlock>& heapBlock : GObjectHeapBlocks)
	{
		if (heapBlock->OwnsCell(objectHeader))
		{
			return heapBlock.Get();
		}
	}

	UM_ASSERT_NOT_REACHED();
}

/**
 * @brief Checks to see if the given parent object respects the desired parent class of the given class.
 *
 * @param objectClass The object class.
 * @param parent The potential parent object.
 * @return True if \p parent is a suitable parent object for an object of type \p objectClass, otherwise false.
 */
static bool ParentObjectRespectsDesiredClassParent(const FClassInfo* objectClass, const FObjectPtr& parent)
{
	const FStructInfo* parentClassWithChildOfAttribute = objectClass;
	while (parentClassWithChildOfAttribute->HasAttribute("ChildOf"_sv) == false)
	{
		parentClassWithChildOfAttribute = parentClassWithChildOfAttribute->GetBaseType();

		// If we've exhausted the parent class chain and still haven't found the attribute, we can exit
		if (parentClassWithChildOfAttribute == nullptr)
		{
			return true;
		}
	}

	const FStringView parentClassName = parentClassWithChildOfAttribute->GetAttributeByName("ChildOf"_sv)->GetValue();
	if (UM_ENSURE(parentClassName.IsEmpty() == false) == false)
	{
		return false;
	}

	if (parent.IsNull())
	{
		UM_LOG(Error, "Type {} requires a parent of type {}, but given null parent", objectClass->GetName(), parentClassName);
		return false;
	}

	const FStructInfo* parentType = parent->GetType();
	UM_ENSURE(parentType != nullptr);

	// TODO This would be better if we could get a type by name, but walking like this should be okay for now
	while (parentType != nullptr)
	{
		if (parentType->GetName() == parentClassName)
		{
			return true;
		}

		parentType = parentType->GetBaseType();
	}

	UM_LOG(Error, "Type {} requires a parent of type {}, but given parent is of type {}", objectClass->GetName(), parentClassName, parent->GetType()->GetName());
	return false;
}

/**
 * @brief Visits all objects in the object heap blocks.
 *
 * @tparam VisitorType The visitor's type.
 * @param visitor The visitor.
 */
template<typename VisitorType>
static void VisitObjectsInHeapBlocks(VisitorType& visitor)
{
	for (TUniquePtr<FObjectHeapBlock>& heapBlock : GObjectHeapBlocks)
	{
		const int32 numCells = heapBlock->GetNumCells();
		for (int32 idx = 0; idx < numCells; ++idx)
		{
			FObjectHeader* cell = heapBlock->GetCell(idx);
			if (UObject* object = cell->GetObject())
			{
				visitor.Visit(object);
			}
		}
	}
}

FObjectPtr FObjectHeap::AllocateObject(const FClassInfo* objectClass, FObjectPtr parent, FStringView name, const FObjectCreationContext& context)
{
	if (objectClass == nullptr)
	{
		UM_LOG(Error, "Attempting to allocate object with null class");
		return nullptr;
	}

	if (objectClass->IsA(UObject::StaticType()) == false)
	{
		UM_LOG(Error, "Cannot allocate non-object class {} on object heap", objectClass->GetName());
		return nullptr;
	}

	if (objectClass->HasAttribute("Abstract"_sv))
	{
		UM_LOG(Error, "Cannot allocate abstract class {}", objectClass->GetName());
		return nullptr;
	}

	if (objectClass->CanConstructAtLocation() == false)
	{
		UM_LOG(Error, "Cannot allocate class {} on object heap without a construct function", objectClass->GetName());
		return nullptr;
	}

	if (ParentObjectRespectsDesiredClassParent(objectClass, parent) == false)
	{
		return nullptr;
	}

	// We don't need to ENSURE or ASSERT on the object memory because we already do that in the allocate function
	void* objectMemory = AllocateObjectMemoryFromHeap(objectClass);

	objectClass->ConstructAtLocation(objectMemory);

	UObject* object = reinterpret_cast<UObject*>(objectMemory);
	SetObjectParent(object, MoveTemp(parent));
	SetObjectName(object, MoveTemp(name), (reinterpret_cast<uint64>(objectMemory) % 0xFFFE) + 1);
	NotifyObjectCreated(object, context);

	return FObjectPtr { object };
}

void* FObjectHeap::AllocateObjectMemoryFromHeap(const FClassInfo* objectClass)
{
	UM_ASSERT(objectClass != nullptr, "Given null class when allocating object memory");

	void* objectMemory = nullptr;
	for (TUniquePtr<FObjectHeapBlock>& heapBlock : GObjectHeapBlocks)
	{
		objectMemory = heapBlock->Allocate(objectClass);

		if (objectMemory != nullptr)
		{
			break;
		}
	}

	// If we couldn't allocate the memory, we need to create a new heap block suitable for the object
	if (objectMemory == nullptr)
	{
		const int32 suitableCellSize = FObjectHeapBlock::GetAlignedCellSize(objectClass->GetSize());

		TUniquePtr<FObjectHeapBlock>& heapBlock = GObjectHeapBlocks.Emplace(FObjectHeapBlock::Create(suitableCellSize));
		objectMemory = heapBlock->Allocate(objectClass);
	}

	UM_ASSERT(objectMemory != nullptr, "Failed to allocate memory for object from heap");

	return objectMemory;
}

/**
 * @brief Defines an object heap visitor that marks every non-"rooted" object it visits for garbage collection.
 */
class FMarkObjectForGarbageCollectionHeapVisitor : public FObjectHeapVisitor
{
public:

	/**
	 * @brief Sets default values for this heap visitor's properties.
	 *
	 * @param badge The object heap badge to use.
	 */
	explicit FMarkObjectForGarbageCollectionHeapVisitor(TBadge<FObjectHeap> badge)
		: m_Badge { badge }
	{
	}

	/** @copydoc FObjectHeapVisitor::Visit */
	virtual void Visit(UObject* object) override
	{
		object->SetVisitedByGarbageCollector(m_Badge, false);

		if (object->ShouldKeepAlive())
		{
			return;
		}

		++GNumObjectsMarkedForDeletion;
		object->SetMarkedForGarbageCollection(m_Badge, true);
	}

private:

	TBadge<FObjectHeap> m_Badge;
};

/**
 * @brief Defines an object heap visitor that un-marks from garbage collection every object referenced by a "rooted" object.
 */
class FUnmarkObjectForGarbageCollectionHeapVisitor : public FObjectHeapVisitor
{
public:

	/**
	 * @brief Sets default values for this heap visitor's properties.
	 *
	 * @param badge The object heap badge to use.
	 */
	explicit FUnmarkObjectForGarbageCollectionHeapVisitor(TBadge<FObjectHeap> badge)
		: m_Badge { badge }
	{
	}

	/**
	 * @brief Checks to see if this object heap visitor is currently tracing a root object.
	 *
	 * @return True if this object heap visitor is currently tracing a root object, otherwise false.
	 */
	[[nodiscard]] bool IsTracingRootObject() const
	{
		return m_RootObjectCount > 0;
	}

	/** @copydoc FObjectHeapVisitor::Visit */
	virtual void Visit(UObject* object) override
	{
		if (object->HasBeenVisitedByGarbageCollector())
		{
			return;
		}

		if (object->ShouldKeepAlive())
		{
			++m_RootObjectCount;
			object->SetVisitedByGarbageCollector(m_Badge, true);
			object->VisitReferencedObjects(*this);
			--m_RootObjectCount;
		}
		else if (IsTracingRootObject())
		{
			--GNumObjectsMarkedForDeletion;

			object->SetVisitedByGarbageCollector(m_Badge, true);
			object->SetMarkedForGarbageCollection(m_Badge, false);
			object->VisitReferencedObjects(*this);
		}
	}

private:

	int64 m_RootObjectCount = 0;
	TBadge<FObjectHeap> m_Badge;
};

/**
 * @brief Defines a combination of an object header as well as its owning heap block.
 */
class FObjectHeaderAndBlock
{
public:

	/** @brief The heap block. */
	FObjectHeapBlock* Block = nullptr;

	/** @brief The object header. */
	FObjectHeader* Header = nullptr;

	/**
	 * @brief Created a new object header and heap block pair.
	 *
	 * @param block The heap block.
	 * @param header The object header.
	 */
	explicit FObjectHeaderAndBlock(FObjectHeapBlock* block, FObjectHeader* header)
	    : Block { block }
	    , Header { header }
	{
	}
};

// Used by SortObjectHeadersForDestruction
inline  const FObjectHeader* GetObjectHeader(const FObjectHeader* header)
{
	return header;
}

// Used by SortObjectHeadersForDestruction
inline const FObjectHeader* GetObjectHeader(const FObjectHeaderAndBlock& headerAndBlock)
{
	return headerAndBlock.Header;
}

/**
 * @brief Sorts a list of object headers so that "newer" objects are first.
 *
 * @param headers The list of headers to sort.
 */
template<typename ElementType>
static void SortObjectHeadersForDestruction(TArray<ElementType>& headers)
{
	headers.Sort([](const ElementType& firstElement, const ElementType& secondElement)
	{
		const FObjectHeader* firstHeader = GetObjectHeader(firstElement);
		const FObjectHeader* secondHeader = GetObjectHeader(secondElement);

		const FTimePoint firstTime = firstHeader->GetObjectAllocationTime();
		const FTimePoint secondTime = secondHeader->GetObjectAllocationTime();

		if (firstTime < secondTime)
		{
			return ECompareResult::GreaterThan;
		}
		if (firstTime > secondTime)
		{
			return ECompareResult::LessThan;
		}
		return ECompareResult::Equals;
	});
}

/**
 * @brief Defines a heap visitor to gather objects marked for deletion.
 */
class FGatherObjectsForDeletionHeapVisitor : public FObjectHeapVisitor
{
public:

	/**
	 * @brief Sets default values for this heap visitor's properties.
	 *
	 * @param badge The object heap badge to use.
	 */
	explicit FGatherObjectsForDeletionHeapVisitor(TBadge<FObjectHeap> badge)
		: m_Badge { badge }
	{
		m_ObjectsMarkedForDeletion.Reserve(8);
	}

	/**
	 * @brief Sorts object marked for deletion to be deleted in reverse allocation order and then deletes them.
	 */
	void SortAndDeleteMarkedObjects()
	{
		SortObjectHeadersForDestruction(m_ObjectsMarkedForDeletion);

		for (FObjectHeaderAndBlock& headerAndBlock : m_ObjectsMarkedForDeletion)
		{
			headerAndBlock.Block->DeleteMarkedCell(m_Badge, headerAndBlock.Header);
		}
	}

	/** @copydoc FObjectHeapVisitor::Visit */
	virtual void Visit(UObject* object) override
	{
		if (object->IsMarkedForGarbageCollection())
		{
			FObjectHeader* objectHeader = FObjectHeader::FromObject(object);
			FObjectHeapBlock* heapBlock = GetHeapBlockForObjectHeader(objectHeader);
			(void)m_ObjectsMarkedForDeletion.Emplace(heapBlock, objectHeader);
		}
	}

private:

	TArray<FObjectHeaderAndBlock> m_ObjectsMarkedForDeletion;
	TBadge<FObjectHeap> m_Badge;
};

void FObjectHeap::CollectGarbage()
{
	constexpr TBadge<FObjectHeap> badge;

	FTimer collectionStartTime = FTimer::Start();

	// 1. Mark all objects
	GNumObjectsMarkedForDeletion = 0;
	FMarkObjectForGarbageCollectionHeapVisitor markVisitor { badge };
	VisitObjectsInHeapBlocks(markVisitor);

	// 2. Un-mark all live objects
	FUnmarkObjectForGarbageCollectionHeapVisitor unmarkVisitor { badge };
	VisitObjectsInHeapBlocks(unmarkVisitor);

	// 3. Delete all marked objects
	if (GNumObjectsMarkedForDeletion > 0)
	{
		FGatherObjectsForDeletionHeapVisitor gatherAndDeleteVisitor { badge };
		VisitObjectsInHeapBlocks(gatherAndDeleteVisitor);
		gatherAndDeleteVisitor.SortAndDeleteMarkedObjects();
	}

	const FTimeSpan collectionDuration = collectionStartTime.Stop();
	if (GNumObjectsMarkedForDeletion > 0)
	{
		UM_LOG(Info, "Garbage collected {} objects in {}ms ({} ticks)", GNumObjectsMarkedForDeletion, collectionDuration.GetTotalMilliseconds(), collectionDuration.GetTicks());
		GNumObjectsMarkedForDeletion = 0;
	}
}

TErrorOr<void> FObjectHeap::Initialize(TBadge<class FEngineInitializer>)
{
	if (GObjectHeapBlocks.Num() > 0)
	{
		return MAKE_ERROR("Object heap is already initialized");
	}

	for (const int32 cellSize : FObjectHeapBlock::GetAlignedCellSizes())
	{
		(void)GObjectHeapBlocks.Emplace(FObjectHeapBlock::Create(cellSize));
	}

	if (GObjectHeapBlocks.IsEmpty())
	{
		return MAKE_ERROR("No aligned cell sizes set for the object heap");
	}

	return {};
}

void FObjectHeap::Shutdown(TBadge<class FEngineInitializer>)
{
	if (GObjectHeapBlocks.IsEmpty())
	{
		return;
	}

	// When shutting down the object heap, let's try to destroy objects in the reverse
	// order that they were created in. This almost definitely will take longer than a
	// normal garbage collection, but will hopefully allow objects with dependencies,
	// such as a graphics device depending on an engine window, to be cleaned up in
	// a way such that they can feel free to properly dispose of their resources
	FTimer destroyObjectsTimer = FTimer::Start();
	{
		TArray<FObjectHeader*> allocatedObjects;
		allocatedObjects.Reserve(32);

		for (TUniquePtr<FObjectHeapBlock>& heapBlock : GObjectHeapBlocks)
		{
			heapBlock->AccumulateValidHeaders(allocatedObjects);
		}

		SortObjectHeadersForDestruction(allocatedObjects);

		constexpr TBadge<FObjectHeap> badge;
		for (FObjectHeader* objectHeader : allocatedObjects)
		{
			objectHeader->NotifyDestroyed(badge);
		}
	}
	const FTimeSpan destroyObjectsDuration = destroyObjectsTimer.Stop();
	UM_LOG(Info, "Object heap shutdown in {}ms ({} ticks)", destroyObjectsDuration.GetTotalMilliseconds(), destroyObjectsDuration.GetTicks());

	for (TUniquePtr<FObjectHeapBlock>& heapBlock : GObjectHeapBlocks)
	{
		heapBlock.Reset();
	}
	GObjectHeapBlocks.Reset();
}

void FObjectHeap::NotifyObjectCreated(UObject* object, const FObjectCreationContext& context)
{
	constexpr TBadge<FObjectHeap> badge;
	object->NotifyCreated(badge, context);
}

void FObjectHeap::NotifyObjectDestroyed(UObject* object)
{
	constexpr TBadge<FObjectHeap> badge;

	UM_LOG(Verbose, "[{}] Destroyed object {}", object, object->GetPath());
	object->NotifyDestroyed(badge);
}

void FObjectHeap::SetObjectParent(UObject* object, FObjectPtr parent)
{
	constexpr TBadge<FObjectHeap> badge;
	object->SetParent(badge, MoveTemp(parent));
}

void FObjectHeap::SetObjectName(UObject* object, FStringView name, uint64 nameCounter)
{
	constexpr TBadge<FObjectHeap> badge;

	FStringBuilder nameBuilder;
	if (name.IsEmpty())
	{
		nameBuilder.Append(object->GetType()->GetName());
	}
	else
	{
		nameBuilder.Append(name);
	}

	nameBuilder.Append("_"_sv);
	nameBuilder.Append(nameCounter);

	object->SetName(badge, nameBuilder.ReleaseString());
}