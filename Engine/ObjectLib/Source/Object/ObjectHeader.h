#pragma once

#include "Engine/IntTypes.h"
#include "HAL/TimePoint.h"
#include "Meta/ClassInfo.h"
#include "Misc/Badge.h"
#include <atomic>

class UObject;

/**
 * @brief Defines a header for an object allocated on the heap.
 */
class FObjectHeader final
{
public:

	/**
	 * @brief Gets the header for the given object.
	 *
	 * @return The header for the given object.
	 */
	[[nodiscard]] static FObjectHeader* FromObject(UObject* object);

	/**
	 * @brief Gets the next header in the free list.
	 *
	 * @return The next header in the free list.
	 */
	[[nodiscard]] FObjectHeader* GetNextFreeHeader() const;

	/**
	 * @brief Gets the object that this header points to.
	 *
	 * @return The object that this header points to, or nullptr if one does not exist.
	 */
	[[nodiscard]] UObject* GetObject() const;

	/**
	 * @brief Gets the time, in ticks, that the owned object was allocated at.
	 *
	 * If the time is zero, then no object was allocated.
	 *
	 * @return The time, in ticks, that the owned object was allocated at.
	 */
	[[nodiscard]] FTimePoint GetObjectAllocationTime() const
	{
		return m_AllocationTime;
	}

	/**
	 * @brief Gets the type of the owned object.
	 *
	 * @return The type of the owned object.
	 */
	[[nodiscard]] const FClassInfo* GetObjectType() const;

	/**
	 * @brief Checks to see if this header points to a valid object.
	 *
	 * @return True if this header points to a valid object, otherwise false.
	 */
	[[nodiscard]] bool HasObject() const;

	/**
	 * @brief Notifies this object header that it has been allocated with an object.
	 *
	 * @param objectClass The object's class.
	 */
	void NotifyAllocated(TBadge<class FObjectHeapBlock>);

	/**
	 * @brief Notifies this object header that its object has been destroyed.
	 *
	 * @param nextHeaderInFreeList The next object header in the free list.
	 */
	void NotifyDestroyed(TBadge<class FObjectHeapBlock>, FObjectHeader* nextHeaderInFreeList);

	/**
	 * @brief Notifies this object header that its object has been destroyed.
	 */
	void NotifyDestroyed(TBadge<class FObjectHeap>);

private:

	FObjectHeader* m_NextFreeHeader = nullptr;
	FTimePoint m_AllocationTime;
};