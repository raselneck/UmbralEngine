#pragma once

#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Engine/Error.h"
#include "Meta/ClassInfo.h"
#include "Misc/Badge.h"
#include "Object/ObjectHeapVisitor.h"
#include "Object/ObjectPtr.h"
#include "Object/WeakObjectPtr.h"
#include <atomic>

class UObject;

/**
 * @brief Defines a way to access the object heap.
 */
class FObjectHeap
{
public:

	/**
	 * @brief Allocates an object.
	 *
	 * @tparam ObjectType The object type.
	 * @param parent The object's parent.
	 * @param name The object's name.
	 * @param context The object's creation context.
	 * @return The object reference.
	 */
	template<typename ObjectType>
	static TObjectPtr<ObjectType> AllocateObject(FObjectPtr parent, FStringView name, const FObjectCreationContext& context)
	{
		FObjectPtr objectPtr = AllocateObject(ObjectType::StaticType(), MoveTemp(parent), MoveTemp(name), context);
		return Cast<ObjectType>(objectPtr);
	}

	/**
	 * @brief Allocates an object.
	 *
	 * @param objectClass The object's class.
	 * @param parent The object's parent.
	 * @param name The object's name.
	 * @param context The object's creation context.
	 * @return The created object, or nullptr if it could not be created.
	 */
	static FObjectPtr AllocateObject(const FClassInfo* objectClass, FObjectPtr parent, FStringView name, const FObjectCreationContext& context);

	/**
	 * @brief Attempts to collect all garbage from the object heap.
	 */
	static void CollectGarbage();

	/**
	 * @brief Destructs an object.
	 *
	 * @tparam ObjectType The object's type.
	 * @param object The object to destruct. Assumed to be non-null.
	 */
	template<typename ObjectType>
	static void DestructObject(TBadge<class FObjectHeader>, ObjectType* object)
	{
		NotifyObjectDestroyed(object);

		using ObjectTypeToDestruct = ObjectType;
		object->~ObjectTypeToDestruct();
	}

	/**
	 * @brief Initializes the object heap.
	 */
	[[nodiscard]] static TErrorOr<void> Initialize(TBadge<class FEngineInitializer>);

	/**
	 * @brief Shuts down the object heap, destroying all objects that have been created.
	 */
	static void Shutdown(TBadge<class FEngineInitializer>);

private:

	/**
	 * @brief Allocates memory for an object from the heap
	 *
	 * @param objectClass The object's class.
	 * @param parent The desired parent object for the allocated object.
	 * @return The memory location for the object.
	 */
	static void* AllocateObjectMemoryFromHeap(const FClassInfo* objectClass);

	/**
	 * @brief Called just after an object has been created.
	 *
	 * @param object The object.
	 * @param context The object's creation context.
	 */
	static void NotifyObjectCreated(UObject* object, const FObjectCreationContext& context);

	/**
	 * @brief Called just before an object is destroyed.
	 *
	 * @param object The object.
	 */
	static void NotifyObjectDestroyed(UObject* object);

	/**
	 * @brief Sets an object's parent.
	 *
	 * @param object The object.
	 * @param parent The new parent.
	 */
	static void SetObjectParent(UObject* object, FObjectPtr parent);

	/**
	 * @brief Sets an object's name.
	 *
	 * @param object The object.
	 * @param name The desired name.
	 */
	static void SetObjectName(UObject* object, FStringView name, uint64 nameCounter);
};