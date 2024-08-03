#pragma once

#include "Engine/IntTypes.h"
#include "Memory/Memory.h"
#include "Templates/Exchange.h"
#include "Templates/Forward.h"
#include "Templates/Move.h"
#include "Templates/Swap.h"
#include "Templates/TypeTraits.h"

/**
 * @brief Default constructs an array of elements.
 *
 * @tparam T The element type.
 * @tparam SizeType The size type.
 * @param elements The pointer to the elements to default construct.
 * @param numElements The number of elements to default construct.
 */
template<typename T, typename SizeType>
void DefaultConstructElements(T* elements, SizeType numElements)
{
	if constexpr (TIsZeroConstructible<T>::Value)
	{
		FMemory::ZeroOutArray(elements, numElements);
	}
	else
	{
		static_assert(TIsDefaultConstructible<T>::Value, "Type is not default constructible");

		while (numElements > 0)
		{
			FMemory::ConstructObjectAt<T>(elements);
			++elements;
			--numElements;
		}
	}
}

/**
 * @brief Copy-assigns an array of elements from another.
 *
 * @tparam T The element type.
 * @tparam SizeType The size type.
 * @param destination The pointer to the elements to assign to.
 * @param source The source elements to copy from.
 * @param numElements The number of elements to copy-assign.
 */
template<typename T, typename SizeType>
void CopyAssignElements(T* destination, const T* source, SizeType numElements)
{
	static_assert(TIsCopyAssignable<T>::Value, "Type is not copy-assignable");

	while (numElements > 0)
	{
		*destination = static_cast<const T&>(*source);
		++destination;
		++source;
		--numElements;
	}
}

/**
 * @brief Copy-constructs an array of elements from another.
 *
 * @tparam T The element type.
 * @tparam SizeType The size type.
 * @param destination The pointer to the elements to construct.
 * @param source The source elements to copy from.
 * @param numElements The number of elements to copy-construct.
 */
template<typename T, typename SizeType>
void CopyConstructElements(T* destination, const T* source, SizeType numElements)
{
	static_assert(TIsCopyConstructible<T>::Value, "Type is not copy-constructible");

	while (numElements > 0)
	{
		FMemory::ConstructObjectAt<T>(destination, static_cast<const T&>(*source));
		++destination;
		++source;
		--numElements;
	}
}

/**
 * @brief Move-assigns an array of elements from another.
 *
 * @tparam T The element type.
 * @tparam SizeType The size type.
 * @param destination The pointer to the elements to assign to.
 * @param source The source elements to mvoe from.
 * @param numElements The number of elements to move-assign.
 */
template<typename T, typename SizeType>
void MoveAssignElements(T* destination, T* source, SizeType numElements)
{
	static_assert(TIsMoveAssignable<T>::Value, "Type is not move-assignable");
	static_assert(TIsConst<T>::Value == false, "Cannot move-assign from const values");

	while (numElements > 0)
	{
		*destination = MoveTemp(*source);
		++destination;
		++source;
		--numElements;
	}
}

/**
 * @brief Move-constructs an array of elements from another.
 *
 * @tparam T The element type.
 * @tparam SizeType The size type.
 * @param destination The pointer to the elements to construct.
 * @param source The source elements to move from.
 * @param numElements The number of elements to move-construct.
 */
template<typename T, typename SizeType>
void MoveConstructElements(T* destination, T* source, SizeType numElements)
{
	static_assert(TIsMoveConstructible<T>::Value, "Type is not move-constructible");
	static_assert(TIsConst<T>::Value == false, "Cannot move-construct from const values");

	while (numElements > 0)
	{
		FMemory::ConstructObjectAt<T>(destination, MoveTemp(*source));
		++destination;
		++source;
		--numElements;
	}
}

/**
 * @brief Destructs an array of elements.
 *
 * @tparam T The element type.
 * @tparam SizeType The size type.
 * @param elements The pointer to the elements to destruct.
 * @param numElements The number of elements to destruct.
 */
template<typename T, typename SizeType>
void DestructElements(T* elements, SizeType numElements)
{
	while (numElements > 0)
	{
		FMemory::DestructObject(elements);
		++elements;
		--numElements;
	}
}