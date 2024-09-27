#pragma once

#include "Containers/Array.h"
#include "Containers/Span.h"
#include "Object/ObjectPtr.h"
#include "Object/WeakObjectPtr.h"
#include "Templates/AndNotOr.h"
#include "Templates/IsConvertible.h"
#include "Templates/IsSame.h"

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
	 * @param span The span of objects.
	 */
	template<typename T>
	void Visit(const TSpan<T> span)
	{
		static_assert(Or<
		    TAnd<TIsBaseOf<UObject, T>, TIsPointer<T>>, // Can be a span of raw object pointers
		    TIsConvertible<T, const FObjectPtr&>,       // Can be a span of FObjectPtr or TObjectPtr<T>
		    TIsConvertible<T, const FWeakObjectPtr&>    // Can be a span of FWeakObjectPtr or TWeakObjectPtr<T>
		>);

		for (T& object : span)
		{
			Visit(object);
		}
	}

	/**
	 * @brief Visits all objects in an object array.
	 *
	 * @param objects The object array to iterate.
	 */
	template<typename T>
	void Visit(const TArray<T>& objects)
	{
		Visit(objects.AsSpan());
	}

protected:

	/**
	 * @brief Destroys this object heap visitor.
	 */
	virtual ~FObjectHeapVisitor() = default;
};

template<typename T> struct TCanObjectHeapVisit : FFalseType {};
template<> struct TCanObjectHeapVisit<UObject*> : FTrueType {};
template<> struct TCanObjectHeapVisit<FObjectPtr> : FTrueType {};
template<> struct TCanObjectHeapVisit<FWeakObjectPtr> : FTrueType {};
template<typename T> struct TCanObjectHeapVisit<TObjectPtr<T>> : FTrueType {};
template<typename T> struct TCanObjectHeapVisit<TWeakObjectPtr<T>> : FTrueType {};
template<typename T> struct TCanObjectHeapVisit<TSpan<T>> : TCanObjectHeapVisit<T> {};
template<typename T> struct TCanObjectHeapVisit<TArray<T>> : TCanObjectHeapVisit<T> {};