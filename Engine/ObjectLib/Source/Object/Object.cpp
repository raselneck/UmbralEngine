#include "Object/Object.h"
#include "Object/ObjectHeap.h"
#include "Misc/StringBuilder.h"

// Object heap only uses placement new
void* UObject::operator new(size_t /*numBytes*/, void* location)
{
	return location;
}

// Object heap only uses placement new, and we can only be created using placement new
void UObject::operator delete(void* /*memory*/) noexcept
{
	UM_ASSERT_NOT_REACHED_MSG("Non-placement operator delete for objects should never be called");
}

// Object heap only uses combined resource blocks, so we don't need to manually free anything here
void UObject::operator delete(void* /*memory*/, void* /*location*/) noexcept
{
}

UObject::UObject() = default;

UObject::~UObject() = default;

FObjectPtr UObject::FindAncestorOfType(const FClassInfo* type) const
{
	constexpr TBadge<UObject> badge;

	FObjectPtr ancestorToCheck { const_cast<UObject*>(this) };
	while (ancestorToCheck.IsValid())
	{
		if (ancestorToCheck.IsA(type))
		{
			return ancestorToCheck;
		}

		ancestorToCheck = ancestorToCheck->GetParent();
	}

	return nullptr;
}

uint64 UObject::GetHash() const
{
	return m_ObjectHash;
}

FStringView UObject::GetName() const
{
	return m_Name;
}

FString UObject::GetPath() const
{
	FStringBuilder pathBuilder;
	AppendPathToStringBuilder(pathBuilder);
	return pathBuilder.ReleaseString();
}

FObjectPtr UObject::GetParent() const
{
	return m_Parent;
}

bool UObject::IsA(const FClassInfo* otherType) const
{
	return GetType()->IsA(otherType);
}

void UObject::NotifyCreated(TBadge<FObjectHeap>, const FObjectCreationContext& context)
{
	Created(context);
}

void UObject::NotifyDestroyed(TBadge<FObjectHeap>)
{
	Destroyed();
}

void UObject::SetMarkedForGarbageCollection(TBadge<FObjectHeap>, const bool marked)
{
	m_MarkedForGarbageCollection = marked;
}

void UObject::SetName(TBadge<FObjectHeap>, FString newName)
{
	UM_ASSERT(m_Name.IsEmpty(), "Attempting to set object name when it is already set!");

	m_Name = MoveTemp(newName);
	m_ObjectHash = GetHashCode(m_Name);
}

void UObject::SetParent(TBadge<FObjectHeap>, FObjectPtr newParent)
{
	UM_ASSERT(m_Parent.IsNull(), "Attempting to set object parent when it is already set!");
	m_Parent = MoveTemp(newParent);
}

void UObject::SetVisitedByGarbageCollector(TBadge<FObjectHeap>, const bool visited)
{
	m_VisitedByGarbageCollector = visited;
}

void UObject::SetShouldKeepAlive(const bool keepAlive)
{
	m_KeepAlive = keepAlive;
}

bool UObject::ShouldKeepAlive() const
{
	return m_KeepAlive;
}

void UObject::Created(const FObjectCreationContext& context)
{
	(void)context;
}

void UObject::Destroyed()
{
}

void UObject::ManuallyVisitReferencedObjects(FObjectHeapVisitor& visitor)
{
	(void)visitor;
}

void UObject::AppendPathToStringBuilder(FStringBuilder& builder) const
{
	if (m_Parent.IsValid())
	{
		m_Parent->AppendPathToStringBuilder(builder);
	}

	builder.Append("/"_sv);
	builder.Append(m_Name);
}