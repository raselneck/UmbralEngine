#pragma once

#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Memory/MemoryMacros.h"
#include "Meta/ClassInfo.h"
#include "Meta/MetaMacros.h"
#include "Meta/SubclassOf.h"
#include "Misc/Badge.h"
#include "Object/ObjectCreationContext.h"
#include "Object/ObjectHeap.h"
#include "Object/ObjectPtr.h"
#include "Object/WeakObjectPtr.h"
#include "Templates/IsBaseOf.h"
#include "Templates/IsConstructible.h"
#include "Object.Generated.h"

class FStringBuilder;

/**
 * @brief Defines the base class for all objects.
 */
UM_CLASS(Abstract)
class UObject
{
	UM_GENERATED_BODY();

protected:

	// Objects should only be allocated using NewObject, which will go through the object heap
	DECLARE_NEW_OPERATORS();
	DECLARE_DELETE_OPERATORS();

public:

	/**
	 * @brief Destroys this object.
	 */
	virtual ~UObject();

	/**
	 * @brief Walks this object's ancestor chain to find a parent of the given type.
	 *
	 * @param type The type.
	 * @return The ancestor with the given \p type. The ancestor could be this object.
	 */
	[[nodiscard]] FObjectPtr FindAncestorOfType(const FClassInfo* type) const;

	/**
	 * @brief Walks this object's ancestor chain to find a parent of the given type.
	 *
	 * @tparam ObjectType The ancestor's type.
	 * @return The ancestor with the given \p type. The ancestor could be this object.
	 */
	template<typename ObjectType>
	[[nodiscard]] TObjectPtr<ObjectType> FindAncestorOfType() const
	{
		return Cast<ObjectType>(FindAncestorOfType(ObjectType::StaticType()));
	}

	/**
	 * @brief Gets this object's unique hash.
	 *
	 * @return This object's unique hash.
	 */
	[[nodiscard]] uint64 GetHash() const;

	/**
	 * @brief Gets this object's name.
	 *
	 * @return This object's name.
	 */
	[[nodiscard]] FStringView GetName() const;

	/**
	 * @brief Gets this object's fully qualified path.
	 *
	 * @return This object's fully qualified path.
	 */
	[[nodiscard]] FString GetPath() const;

	/**
	 * @brief Gets this object's parent object.
	 *
	 * @return This object's parent object.
	 */
	[[nodiscard]] FObjectPtr GetParent() const;

	/**
	 * @brief Gets this object's parent object as a specific type.
	 *
	 * @tparam T The desired object type.
	 * @return This object's parent object as a specific type.
	 */
	template<typename T>
	[[nodiscard]] TObjectPtr<T> GetTypedParent() const
	{
		static_assert(IsBaseOf<UObject, T>, "Given type is not an object type");
		return Cast<T>(FindAncestorOfType(T::StaticType()));
	}

	/**
	 * @brief Checks to see if this object has been visited by the garbage collector.
	 *
	 * @return True if this object has been visited by the garbage collector, otherwise false.
	 */
	[[nodiscard]] bool HasBeenVisitedByGarbageCollector() const
	{
		return m_VisitedByGarbageCollector;
	}

	/**
	 * @brief Checks to see if this object is an instance of the given type.
	 *
	 * @param otherType The other type info.
	 * @return True if this object is an instance of the type pointed to by \p otherType, otherwise false.
	 */
	[[nodiscard]] bool IsA(const FClassInfo* otherType) const;

	/**
	 * @brief Checks to see if this object is an instance of the given type.
	 *
	 * @tparam ObjectType The other type.
	 * @return True if this object is an instance of \p ObjectType, otherwise false.
	 */
	template<typename ObjectType>
	[[nodiscard]] bool IsA() const
	{
		return IsA(ObjectType::StaticType());
	}

	/**
	 * @brief Checks to see if this object is marked for garbage collection.
	 *
	 * @return True if this object is marked for garbage collection, otherwise false.
	 */
	[[nodiscard]] bool IsMarkedForGarbageCollection() const
	{
		return m_MarkedForGarbageCollection;
	}

	/**
	 * @brief Calls this object's Created function.
	 *
	 * @param context The object creation context.
	 */
	void NotifyCreated(TBadge<FObjectHeap>, const FObjectCreationContext& context);

	/**
	 * @brief Calls this object's Destroyed function.
	 */
	void NotifyDestroyed(TBadge<FObjectHeap>);

	/**
	 * @brief Sets whether or not this object has been marked for garbage collection.
	 *
	 * @param marked True to mark this object for garbage collection, false to unmark it.
	 */
	void SetMarkedForGarbageCollection(TBadge<FObjectHeap>, bool marked);

	/**
	 * @brief Sets this object's name.
	 *
	 * @param name The new name.
	 */
	void SetName(TBadge<FObjectHeap>, FString name);

	/**
	 * @brief Sets this object's parent.
	 *
	 * @param parent The new parent object.
	 */
	void SetParent(TBadge<FObjectHeap>, FObjectPtr parent);

	/**
	 * @brief Sets whether or not this object has been marked by the garbage collector.
	 *
	 * @param marked True to mark this object as visited by the garbage collector, false to unmark it.
	 */
	void SetVisitedByGarbageCollector(TBadge<FObjectHeap>, bool visited);

	/**
	 * @brief Sets whether or not this object should be kept alive during garbage collection.
	 *
	 * @param keepAlive The new value for the "keep alive" flag.
	 */
	void SetShouldKeepAlive(bool keepAlive);

	/**
	 * @brief Gets a value indicating whether or not this object should be kept alive during garbage collection.
	 *
	 * @return The value of the "keep alive" flag.
	 */
	[[nodiscard]] bool ShouldKeepAlive() const;

#if WITH_TESTING
	/**
	 * @brief Gets the offset to the name property for the given object type.
	 *
	 * @tparam ObjectType The object type.
	 * @return The offset to the name property.
	 */
	template<typename ObjectType>
	[[nodiscard]] static int32 GetOffsetOfNameProperty()
	{
		return UM_OFFSET_OF(ObjectType, m_Name);
	}

	/**
	 * @brief Gets the offset to the parent object property for the given object type.
	 *
	 * @tparam ObjectType The object type.
	 * @return The offset to the parent object property.
	 */
	template<typename ObjectType>
	[[nodiscard]] static int32 GetOffsetOfParentProperty()
	{
		return UM_OFFSET_OF(ObjectType, m_Parent);
	}

	/**
	 * @brief Gets a pointer to the name property.
	 *
	 * @return A pointer to the name property.
	 */
	[[nodiscard]] const void* GetPointerToNameProperty() const
	{
		return static_cast<const void*>(&m_Name);
	}

	/**
	 * @brief Gets a pointer to the parent property.
	 *
	 * @return A pointer to the parent property.
	 */
	[[nodiscard]] const void* GetPointerToParentProperty() const
	{
		return static_cast<const void*>(&m_Parent);
	}
#endif

protected:

	/**
	 * @brief Called when this object was just created.
	 *
	 * @param context The object creation context.
	 */
	virtual void Created(const FObjectCreationContext& context);

	/**
	 * @brief Called when this object is about to be destroyed.
	 */
	virtual void Destroyed();

	/**
	 * @brief "Manually" visits referenced objects. This is a workaround until the header tool supports
	 *        struct types that themselves have object references.
	 *
	 * @param visitor The visitor.
	 */
	virtual void ManuallyVisitReferencedObjects(FObjectHeapVisitor& visitor);

private:

	/**
	 * @brief Appends this object's path to the given string builder.
	 *
	 * @param builder The string builder.
	 */
	void AppendPathToStringBuilder(FStringBuilder& builder) const;

	UM_PROPERTY()
	FObjectPtr m_Parent;

	UM_PROPERTY()
	FString m_Name;

	uint64 m_ObjectHash = INVALID_HASH;
	uint64 m_KeepAlive : 1 = false;
	uint64 m_MarkedForGarbageCollection : 1 = false;
	uint64 m_VisitedByGarbageCollector : 1 = false;

	// TODO Eventually may need a way to keep an object alive even though it has no references
};

/**
 * @brief Creates a new object.
 *
 * @tparam ObjectType The expected type of the object to create.
 * @param objectClass The object's class.
 * @param parent The parent to set for the object.
 * @param name The object's name.
 * @param context The object's creation context.
 * @return The object reference.
 */
template<typename ObjectType>
TObjectPtr<ObjectType> MakeObject(const FClassInfo* objectClass, FObjectPtr parent = nullptr, const FStringView name = nullptr, const FObjectCreationContext& context = {})
{
	FObjectPtr object = FObjectHeap::AllocateObject(objectClass, MoveTemp(parent), name, context);
	return Cast<ObjectType>(object);
}

/**
 * @brief Creates a new object.
 *
 * @tparam ObjectType The type of the object to create.
 * @param parent The parent to set for the object.
 * @param name The object's name.
 * @param context The object's creation context.
 * @return The object reference.
 */
template<typename ObjectType>
TObjectPtr<ObjectType> MakeObject(FObjectPtr parent = nullptr, const FStringView name = nullptr, const FObjectCreationContext& context = {})
{
	// TODO This can be a requires clause if we also use IsTypeIncomplete
	static_assert(IsBaseOf<UObject, ObjectType>, "Given type is not an object type");

	return FObjectHeap::AllocateObject<ObjectType>(MoveTemp(parent), name, context);
}