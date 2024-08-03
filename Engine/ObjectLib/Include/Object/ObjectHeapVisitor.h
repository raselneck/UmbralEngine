#pragma once

#include "Containers/Span.h"
#include "Templates/IsObject.h"

class FObjectPtr;
class FWeakObjectPtr;
class UObject;

/**
 * @brief Defines the base for visiting objects on the heap.
 */
class FObjectHeapVisitor
{
public:

	/**
	 * @brief Visits an object.
	 *
	 * @param object The object.
	 */
	virtual void Visit(UObject* object) = 0;

	/**
	 * @brief Visits an object.
	 *
	 * @param objectPtr The object pointer.
	 */
	virtual void Visit(const FObjectPtr& objectPtr);

	/**
	 * @brief Visits an object.
	 *
	 * @param objectPtr The soft object pointer.
	 */
	virtual void Visit(const FWeakObjectPtr& objectPtr);

	/**
	 * @brief Visits all objects in a span.
	 *
	 * @tparam T The object type.
	 * @param objectSpan The object span.
	 */
	template<typename T>
	void Visit(TSpan<T> objectSpan)
	{
		static_assert(TIsObject<T>::Value);

		for (T& object : objectSpan)
		{
			Visit(object);
		}
	}

protected:

	/**
	 * @brief Destroys this object heap visitor.
	 */
	virtual ~FObjectHeapVisitor() = default;
};