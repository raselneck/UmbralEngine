#pragma once

#include "Containers/Span.h"
#include "Engine/Assert.h"
#include "Engine/CoreTypes.h"
#include "Engine/Hashing.h"
#include "Engine/MiscMacros.h"
#include "Memory/Memory.h"
#include "Templates/BulkOperations.h"
#include "Templates/ComparisonTraits.h"
#include "Templates/IsCallable.h"
#include "Templates/IsSame.h"
#include "Templates/NumericLimits.h"
#include "Templates/Swap.h"
#include "Templates/TypeTraits.h"
#include <initializer_list>

// TODO Write a custom array iterator that verifies nothing is added or removed while iterating

namespace Private
{
	// Quick sort implementation adapted from https://www.geeksforgeeks.org/quick-sort/#

	template<typename ComparerType, typename ElementType>
	struct TIsComparer : TIsCallable<ECompareResult, ComparerType, AddLValueReference<AddConst<ElementType>>, AddLValueReference<AddConst<ElementType>>>
	{
	};

	template<typename ComparerType, typename ElementType>
	inline constexpr bool IsComparer = TIsComparer<ComparerType, ElementType>::Value;

	/**
	 * @brief Performs the partition portion of a quick sort.
	 *
	 * @tparam ComparerType The comparer type.
	 * @tparam ElementType The element type.
	 * @param compare The compare function.
	 * @param elements The element span to sort.
	 * @param lowIndex The lower bound of the span being sorted.
	 * @param highIndex The upper bound of the span being sorted.
	 * @return The new pivot index for sorting.
	 */
	template<typename ComparerType, typename ElementType>
	int32 QuickSortPartition(ComparerType& compare, ElementType* elements, int32 lowIndex, int32 highIndex)
		requires IsComparer<ComparerType, ElementType>
	{
		const ElementType& pivot = elements[highIndex];

		// Index of smaller element and indicate
		// the right position of pivot found so far
		int32 i = (lowIndex - 1);

		for(int32 j = lowIndex; j <= highIndex; ++j)
		{
			// If current element is smaller than the pivot
			if (compare(elements[j], pivot) == ECompareResult::LessThan)
			{
				// Increment index of smaller element
				++i;
				Swap(elements[i], elements[j]);
			}
		}

		Swap(elements[i + 1], elements[highIndex]);

		return i + 1;
	}

	/**
	 * @brief Performs a quick sort on the given array of elements.
	 *
	 * @tparam ComparerType The comparer type.
	 * @tparam ElementType The element type.
	 * @param compare The compare functor.
	 * @param elements The element span to sort.
	 * @param lowIndex The lower bound of the span to sort.
	 * @param highIndex The upper bound of the span to sort.
	 */
	template<typename ComparerType, typename ElementType>
	void QuickSort(ComparerType& compare, ElementType* elements, int32 lowIndex, int32 highIndex)
		requires IsComparer<ComparerType, ElementType>
	{
		if (highIndex <= lowIndex)
		{
			return;
		}

		const int32 partitionIndex = ::Private::QuickSortPartition(compare, elements, lowIndex, highIndex);

		::Private::QuickSort(compare, elements, lowIndex,partitionIndex - 1);
		::Private::QuickSort(compare, elements, partitionIndex + 1,highIndex);
	}
}

/**
 * @brief Defines a dynamically sized array.
 *
 * @tparam T The type contained within the array.
 */
template<typename T>
class TArray final
{
public:

	using ElementType = T;
	using PointerType = AddPointer<ElementType>;
	using ConstPointerType = AddPointer<AddConst<ElementType>>;
	using ReferenceType = AddLValueReference<ElementType>;
	using ConstReferenceType = AddLValueReference<AddConst<ElementType>>;
	using SizeType = int32;
	using IteratorType = PointerType;
	using ConstIteratorType = ConstPointerType;
	using SpanType = TSpan<ElementType>;
	using ConstSpanType = TSpan<const ElementType>;
	using ComparisonTraits = TComparisonTraits<ElementType>;

	template<typename IteratorType>
	using TIsIterator = TIsCallable<EIterationDecision, IteratorType, ReferenceType>;

	template<typename IteratorType>
	using TIsConstIterator = TIsCallable<EIterationDecision, IteratorType, ConstReferenceType>;

	/**
	 * @brief Creates a new, empty array.
	 */
	TArray() = default;

	/**
	 * @brief Creates a new array, copying the data from another.
	 *
	 * @param other The other array to copy data from.
	 */
	TArray(const TArray& other)
	{
		CopyFrom(other);
	}

	/**
	 * @brief Creates a new array, transferring ownership of another's data
	 *
	 * @param other The array whose data is to be transferred.
	 */
	TArray(TArray&& other) noexcept
	{
		MoveFrom(MoveTemp(other));
	}

	/**
	 * @brief Creates a new array, copying values from a C-style array.
	 *
	 * @param elements The C-style array elements.
	 * @param numElements The number of elements to copy.
	 */
	TArray(const ElementType* elements, const SizeType numElements)
	{
		if (elements == nullptr)
		{
			UM_ASSERT(numElements == 0, "Cannot construct array with more than zero elements if given raw array is null");
		}
		else
		{
			UM_ASSERT(numElements >= 0, "Cannot construct array with a negative number of elements");
		}

		Append(elements, numElements);
	}

	/**
	 * @brief Creates a new array, copying values from a span.
	 *
	 * @param elements The span elements.
	 */
	explicit TArray(const TSpan<const ElementType> elements)
	{
		if (elements.IsEmpty())
		{
			return;
		}

		Append(elements);
	}

	/**
	 * @brief Creates a new array from an initializer list.
	 *
	 * @param initializer The initializer list.
	 */
	TArray(std::initializer_list<ElementType> initializer)
	{
		if (initializer.size() > 0)
		{
			m_NumElements = static_cast<SizeType>(initializer.size());
			Reserve(m_NumElements);
			CopyConstructElements(m_Data, initializer.begin(), m_NumElements);
		}
	}

	/**
	 * @brief Destroys this array.
	 */
	~TArray()
	{
		Clear();
	}

	/**
	 * @brief Adds a new element to this array.
	 *
	 * @param value The new element's value.
	 * @return The index of the added value.
	 */
	[[maybe_unused]] SizeType Add(const ElementType& value)
	{
		// TODO Verify that `value' is not located within this array

		const SizeType elementIndex = AddUninitialized(1);
		CopyConstructElements(m_Data + elementIndex, &value, 1);
		return elementIndex;
	}

	/**
	 * @brief Adds a new element to this array.
	 *
	 * @param value The new element's value.
	 * @return The index of the added value.
	 */
	[[maybe_unused]] SizeType Add(ElementType&& value)
	{
		// TODO Verify that `value' is not located within this array

		const SizeType elementIndex = AddUninitialized(1);
		MoveConstructElements(m_Data + elementIndex, &value, 1);
		return elementIndex;
	}

	/**
	 * @brief Adds a number of default-constructed elements.
	 *
	 * @param numElements The number of elements to add.
	 * @return The index of the first newly added value.
	 */
	[[maybe_unused]] SizeType AddDefault(const SizeType numElements = 1)
	{
		const SizeType firstElementIndex = AddUninitialized(numElements);
		DefaultConstructElements(m_Data + firstElementIndex, numElements);
		return firstElementIndex;
	}

	/**
	 * @brief Adds a default-constructed element to this array and gets a reference to it.
	 *
	 * @return A reference to the default-constructed element.
	 */
	[[nodiscard]] ElementType& AddDefaultGetRef()
	{
		const SizeType elementIndex = AddDefault(1);
		return m_Data[elementIndex];
	}

	/**
	 * @brief Adds a number of uninitialized elements to this array.
	 *
	 * @param numElementsToAdd The number of uninitialized elements to add.
	 * @return The index of the first uninitialized value.
	 */
	[[maybe_unused]] SizeType AddUninitialized(const SizeType numElementsToAdd)
	{
		UM_ASSERT(numElementsToAdd >= 0, "Must add at least one element to the array");

		if (UNLIKELY(numElementsToAdd == 0))
		{
			return INDEX_NONE;
		}

		ConditionalGrow(numElementsToAdd);

		const SizeType firstElementIndex = m_NumElements;
		m_NumElements += numElementsToAdd;

		UM_ASSERT(m_NumElements <= m_Capacity, "Array grow failed to accommodate all new elements");

		return firstElementIndex;
	}

	/**
	 * @brief Adds a number of zero-initialized elements.
	 *
	 * @param numElements The number of elements to add.
	 * @return The index of the first newly added element.
	 */
	[[maybe_unused]] SizeType AddZeroed(const SizeType numElements)
	{
		static_assert(TIsZeroConstructible<ElementType>::Value);

		const SizeType firstElementIndex = AddUninitialized(numElements);
		FMemory::ZeroOutArray(m_Data + firstElementIndex, numElements);
		return firstElementIndex;
	}

	/**
	 * @brief Adds a zero-initialized element element to this array and gets a reference to it.
	 *
	 * @return A reference to the zero-initialized element.
	 */
	[[nodiscard]] ElementType& AddZeroedGetRef()
	{
		const SizeType elementIndex = AddZeroed(1);
		return m_Data[elementIndex];
	}

	/**
	 * @brief Appends elements to this array, copying values from a C-style array.
	 *
	 * @param elements The C-style array elements.
	 * @param numElements The number of elements to append.
	 */
	void Append(const ElementType* elements, const SizeType numElements)
	{
		if (numElements == 0)
		{
			return;
		}

		UM_ASSERT(numElements > 0, "Cannot append a negative number of elements to an array");

		Append(TSpan<const ElementType> { elements, numElements });
	}

	/**
	 * @brief Appends the given span to this array.
	 *
	 * @param elements The element span to append.
	 */
	void Append(const TSpan<const ElementType> elements)
	{
		if (elements.IsEmpty())
		{
			return;
		}

		const SizeType firstElementIndex = AddUninitialized(elements.Num());
		CopyConstructElements(m_Data + firstElementIndex, elements.GetData(), elements.Num());
	}

	/**
	 * @brief Gets this array as a span.
	 *
	 * @return This array as a span.
	 */
	[[nodiscard]] ConstSpanType AsSpan() const
	{
		return ConstSpanType { m_Data, m_NumElements };
	}

	/**
	 * @brief Gets this array as a span.
	 *
	 * @return This array as a span.
	 */
	[[nodiscard]] SpanType AsSpan()
	{
		return SpanType { m_Data, m_NumElements };
	}

	/**
	 * @brief Gets the element at the given index.
	 *
	 * @param index The index of the element to get.
	 * @return The element at the given index.
	 */
	[[nodiscard]] ConstReferenceType At(const SizeType index) const
	{
		UM_ASSERT(IsValidIndex(index), "Attempting to retrieve element with invalid index");
		return m_Data[index];
	}

	/**
	 * @brief Gets the element at the given index.
	 *
	 * @param index The index of the element to get.
	 * @return The element at the given index.
	 */
	[[nodiscard]] ReferenceType At(const SizeType index)
	{
		UM_ASSERT(IsValidIndex(index), "Attempting to retrieve element with invalid index");
		return m_Data[index];
	}

	/**
	 * @brief Clears this array and frees any memory taken up by it.
	 */
	void Clear()
	{
		if (m_Data)
		{
			DestructElements(m_Data, m_NumElements);
			FMemory::Free(m_Data);
		}

		m_Data = nullptr;
		m_NumElements = 0;
		m_Capacity = 0;
	}

	/**
	 * @brief Checks to see if this array contains a value using the given predicate to check for the value.
	 *
	 * @tparam PredicateType The type of the predicate.
	 * @param predicate The predicate function to use to check for value equality.
	 * @return True if this array contains a value as indicated by \p predicate, otherwise false.
	 */
	template<typename PredicateType>
	[[nodiscard]] bool ContainsByPredicate(PredicateType predicate) const
	{
		for (SizeType idx = 0; idx < m_NumElements; ++idx)
		{
			if (predicate(static_cast<ConstReferenceType>(m_Data[idx])))
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * @brief Checks to see if this array contains the given element to search for.
	 *
	 * @param elementToSearchFor The element to search for.
	 * @return True if this array contains \p elementToSearchFor, otherwise false.
	 */
	[[nodiscard]] bool Contains(const ElementType& elementToSearchFor) const
	{
		return ContainsByPredicate([&elementToSearchFor](ConstReferenceType element)
		{
			return ComparisonTraits::Equals(element, elementToSearchFor);
		});
	}

	/**
	 * @brief Creates an array with a given number of default-constructed elements.
	 *
	 * @param numElements The number of elements.
	 * @return The array.
	 */
	[[nodiscard]] static TArray CreateWithDefaultElements(const SizeType numElements)
	{
		TArray result;
		result.AddDefault(numElements);
		return result;
	}

	/**
	 * @brief Constructs a new element in-place at the end of this array.
	 *
	 * @tparam ConstructTypes The types of the arguments to forward to the new element's constructor.
	 * @param args The arguments to forward to the new element's constructor.
	 * @return The newly added element.
	 */
	template<typename... ConstructTypes>
	[[nodiscard]] ElementType& Emplace(ConstructTypes&&... args)
	{
		const int32 elementIndex = AddUninitialized(1);
		return EmplaceAt(elementIndex, Forward<ConstructTypes>(args)...);
	}

	/**
	 * @brief Constructs a new element in-place at the given index.
	 *
	 * @tparam ConstructTypes The types of the arguments to forward to the new element's constructor.
	 * @param index The index to emplace the item at.
	 * @param args The arguments to forward to the new element's constructor.
	 * @return The emplaced element.
	 */
	template<typename... ConstructTypes>
	[[maybe_unused]] ElementType& EmplaceAt(const SizeType index, ConstructTypes&&... args)
	{
		UM_ASSERT(IsValidIndex(index), "Attempting to emplace value at invalid index");

		FMemory::ConstructObjectAt<ElementType>(m_Data + index, Forward<ConstructTypes>(args) ...);
		return m_Data[index];
	}

	/**
	 * @brief Finds an element by a predicate.
	 *
	 * @tparam PredicateType The predicate's type.
	 * @param predicate The predicate.
	 * @return The found element, or nullptr if it was not found.
	 */
	template<typename PredicateType>
	[[nodiscard]] ConstPointerType FindByPredicate(PredicateType predicate) const
	{
		return AsSpan().template FindByPredicate<PredicateType>(MoveTemp(predicate));
	}

	/**
	 * @brief Finds an element by a predicate.
	 *
	 * @tparam PredicateType The predicate's type.
	 * @param predicate The predicate.
	 * @return The found element, or nullptr if it was not found.
	 */
	template<typename PredicateType>
	[[nodiscard]] PointerType FindByPredicate(PredicateType predicate)
	{
		return AsSpan().template FindByPredicate<PredicateType>(MoveTemp(predicate));
	}

	/**
	 * @brief Gets this array's capacity, which is the total number of elements it can contain before it must grow.
	 *
	 * @return This array's capacity.
	 */
	[[nodiscard]] SizeType GetCapacity() const
	{
		return m_Capacity;
	}

	/**
	 * @brief Gets this array's data.
	 *
	 * @return This array's data.
	 */
	[[nodiscard]] ConstPointerType GetData() const
	{
		return m_Data;
	}

	/**
	 * @brief Gets this array's data.
	 *
	 * @return This array's data.
	 */
	[[nodiscard]] PointerType GetData()
	{
		return UNLIKELY(IsEmpty()) ? nullptr : m_Data;
	}

	/**
	 * @brief Finds the index of an element based on a predicate callback.
	 *
	 * @tparam PredicateType The predicate's type.
	 * @param predicate The predicate.
	 * @return The index of the element specified by \p predicate, or INDEX_NONE if the element was not found.
	 */
	template<typename PredicateType>
	[[nodiscard]] SizeType IndexOfByPredicate(PredicateType predicate) const
	{
		for (SizeType idx = 0; idx < m_NumElements; ++idx)
		{
			if (predicate(static_cast<ConstReferenceType>(m_Data[idx])))
			{
				return idx;
			}
		}
		return INDEX_NONE;
	}

	/**
	 * @brief Finds the index of an element.
	 *
	 * @param elementToSearchFor The element to find the index of.
	 * @return The index of the element, or INDEX_NONE if the element was not found.
	 */
	[[nodiscard]] SizeType IndexOf(const ElementType& elementToSearchFor) const
	{
		return IndexOfByPredicate([&elementToSearchFor](ConstReferenceType element)
		{
			return ComparisonTraits::Equals(element, elementToSearchFor);
		});
	}

	/**
	 * @brief Inserts a value into this array.
	 *
	 * @param index The index to insert the value at.
	 * @param value The value to insert.
	 */
	void Insert(const SizeType index, const ElementType& value)
	{
		// TODO Verify that `value' is not located within this array

		InsertUninitialized(index, 1);

		new (m_Data + index) ElementType(value);
	}

	/**
	 * @brief Inserts a value into this array.
	 *
	 * @param index The index to insert the value at.
	 * @param value The value to insert.
	 */
	void Insert(const SizeType index, ElementType&& value)
	{
		// TODO Verify that `value' is not located within this array

		InsertUninitialized(index, 1);

		new (m_Data + index) ElementType(MoveTemp(value));
	}

	/**
	 * @brief Inserts a number of default-constructed elements to this array.
	 *
	 * @param index The index to begin inserting the elements at.
	 * @param numElements The number of elements to insert.
	 */
	void InsertDefault(const SizeType index, const SizeType numElements)
	{
		if (IsEmpty())
		{
			AddDefault(numElements);
		}
		else
		{
			InsertUninitialized(index, numElements);
			DefaultConstructElements(m_Data + index, numElements);
		}
	}

	/**
	 * @brief Inserts a number of uninitialized elements to this array.
	 *
	 * @param index The index to begin inserting the elements at.
	 * @param numElementsToInsert The number of elements to insert.
	 */
	void InsertUninitialized(const SizeType index, const SizeType numElementsToInsert)
	{
		if (IsEmpty() && index == 0)
		{
			AddUninitialized(numElementsToInsert);
			return;
		}

		UM_ASSERT(IsValidIndex(index), "Invalid index to insert elements at");

		AddUninitialized(numElementsToInsert);

		// Need to move construct the elements after the insertion point, but need to do it in reverse
		const SizeType numElementsToMove = m_NumElements - numElementsToInsert - index;
		for (SizeType moveOffset = 0; moveOffset < numElementsToMove; ++moveOffset)
		{
			const SizeType dstIndex = m_NumElements - 1 - moveOffset;
			const SizeType srcIndex = dstIndex - numElementsToInsert;

			new (m_Data + dstIndex) ElementType(MoveTemp(m_Data[srcIndex]));
		}

		DestructElements(m_Data + index, numElementsToInsert);
		FMemory::ZeroOut(m_Data + index, sizeof(ElementType) * numElementsToInsert);
	}

	/**
	 * @brief Checks to see if this array is empty.
	 *
	 * @return True if this array is empty, otherwise false.
	 */
	[[nodiscard]] bool IsEmpty() const
	{
		return m_NumElements == 0;
	}

	/**
	 * @brief Checks to see if the given index is valid for this array.
	 *
	 * @param index The index.
	 * @return True if \p index is a valid index for this array, otherwise false.
	 */
	[[nodiscard]] bool IsValidIndex(const SizeType index) const
	{
		return index >= 0 && index < m_NumElements;
	}

	/**
	 * @brief Iterates all elements in this array.
	 *
	 * @tparam IteratorType The iterator type.
	 * @param iterator The iterator.
	 */
	template<typename IteratorType>
	void Iterate(IteratorType iterator) const
		requires TIsIterator<IteratorType>::Value
	{
		for (SizeType idx = 0; idx < m_NumElements; ++idx)
		{
			const EIterationDecision decision = iterator(operator[](idx));
			if (decision == EIterationDecision::Break)
			{
				break;
			}
		}
	}

	/**
	 * @brief Iterates all elements in this array.
	 *
	 * @tparam IteratorType The iterator type.
	 * @param iterator The iterator.
	 */
	template<typename IteratorType>
	void Iterate(IteratorType iterator)
		requires TIsConstIterator<IteratorType>::Value
	{
		for (SizeType idx = 0; idx < m_NumElements; ++idx)
		{
			const EIterationDecision decision = iterator(operator[](idx));
			if (decision == EIterationDecision::Break)
			{
				break;
			}
		}
	}

	/**
	 * @brief Gets the last element of this array.
	 *
	 * @return The last element of this array.
	 */
	[[nodiscard]] ConstReferenceType Last() const
	{
		UM_ASSERT(m_NumElements >= 1, "Cannot get the last element from an empty array");

		return m_Data[m_NumElements - 1];
	}

	/**
	 * @brief Gets the last element of this array.
	 *
	 * @return The last element of this array.
	 */
	[[nodiscard]] ReferenceType Last()
	{
		UM_ASSERT(m_NumElements >= 1, "Cannot get the last element from an empty array");

		return m_Data[m_NumElements - 1];
	}

	/**
	 * @brief Gets the number of elements in this array.
	 *
	 * @return The number of elements in this array.
	 */
	[[nodiscard]] SizeType Num() const
	{
		return m_NumElements;
	}

	// TODO Prepend

	/**
	 * @brief Removes a given number of elements from this array.
	 *
	 * @param index The index of the first element to be removed.
	 * @param numElementsToRemove The number of elements to be removed.
	 */
	void RemoveAt(const SizeType index, const SizeType numElementsToRemove = 1)
	{
		UM_ASSERT(numElementsToRemove > 0, "Must remove at least one element to the array");

		const SizeType endIndex = index + numElementsToRemove;
		UM_ASSERT(IsValidIndex(index), "Given index to remove at is invalid");
		UM_ASSERT(IsValidIndex(endIndex - 1), "Given number of elements to remove is too large");

		DestructElements(m_Data + index, numElementsToRemove);

		// TODO Can this just be replaced by a simple memory move?
		for (SizeType dstIndex = index; dstIndex < m_NumElements - numElementsToRemove; ++dstIndex)
		{
			const SizeType offset = dstIndex - index;
			const SizeType srcIndex = index + numElementsToRemove + offset;

			new (m_Data + dstIndex) ElementType(MoveTemp(m_Data[srcIndex]));
			DestructElements(m_Data + srcIndex, 1);
		}

		m_NumElements -= numElementsToRemove;
	}

	/**
	 * @brief Removes all elements from this array that match the given predicate.
	 *
	 * @tparam PredicateType The type of the predicate.
	 * @param predicate The predicate.
	 */
	template<typename PredicateType>
	void RemoveByPredicate(PredicateType predicate)
	{
		for (SizeType idx = Num() - 1; idx >= 0; --idx)
		{
			const ElementType& value = operator[](idx);
			if (predicate(value))
			{
				RemoveAt(idx, 1);
			}
		}
	}

	/**
	 * @brief Removes an element from this array matching the given value.
	 *
	 * @param value The value to remove.
	 */
	void Remove(const ElementType& value)
	{
		const SizeType valueIndex = IndexOf(value);
		if (valueIndex != INDEX_NONE)
		{
			RemoveAt(valueIndex);
		}
	}

	/**
	 * @brief Ensures this array has at least the given desired capacity.
	 *
	 * @param desiredCapacity The desired capacity.
	 */
	void Reserve(const SizeType desiredCapacity)
	{
		if (desiredCapacity > m_Capacity)
		{
			GrowToCapacity(desiredCapacity);
		}
	}

	/**
	 * @brief Clears this array without making any changes to already reserved memory.
	 */
	void Reset()
	{
		DestructElements(m_Data, m_NumElements);
		m_NumElements = 0;
	}

	/**
	 * @brief Reverses this array.
	 */
	void Reverse()
	{
		const SizeType halfNum = Num() / 2;
		for (SizeType idx = 0; idx < halfNum; ++idx)
		{
			const SizeType mirroredIdx = Num() - 1 - idx;
			Swap(At(idx), At(mirroredIdx));
		}
	}

	/**
	 * @brief Gets a copy of this array that has been reversed.
	 *
	 * @return A copy of this array that has been reversed.
	 */
	TArray Reversed() const
	{
		TArray copy = *this;
		copy.Reverse();
		return copy;
	}

	/**
	 * @brief Sets the number of elements in this array.
	 *
	 * @param newNumElements The new number of elements.
	 */
	void SetNum(const SizeType newNumElements)
	{
		UM_ASSERT(newNumElements >= 0, "Cannot set array num to a negative number");

		if (newNumElements > m_Capacity)
		{
			GrowToCapacity(newNumElements);

			if (newNumElements > m_NumElements)
			{
				DefaultConstructElements(m_Data + m_NumElements, newNumElements - m_NumElements);
				m_NumElements = newNumElements;
			}
		}
		else if (newNumElements < m_NumElements)
		{
			DestructElements(m_Data + newNumElements, m_NumElements - newNumElements);
			m_NumElements = newNumElements;

			// TODO Add some way to shrink the amount of memory used too?
		}
	}

	/**
	 * @brief Attempts to shrink this array's memory to only use what is absolutely needed.
	 */
	void Shrink()
	{
		if (UNLIKELY(m_NumElements == m_Capacity))
		{
			return;
		}
	}

	/**
	 * @brief Sorts this array in-place.
	 */
	void Sort()
	{
		Sort(ComparisonTraits::Compare);
	}

	/**
	 * @brief Sorts this array using a custom comparer.
	 *
	 * @tparam ComparerType The comparer type.
	 * @param comparer The comparer.
	 */
	template<typename ComparerType>
	void Sort(ComparerType comparer)
		requires TIsCallable<ECompareResult, ComparerType, ConstReferenceType, ConstReferenceType>::Value
	{
		if (Num() < 2)
		{
			return;
		}

		const SizeType lastIndex = Num() - 1;
		Private::QuickSort(comparer, m_Data, 0, lastIndex);
	}

	/**
	 * @brief Gets a sorted copy of this array.
	 *
	 * @return A sorted copy of this array.
	 */
	TArray Sorted() const
	{
		TArray copy = *this;
		copy.Sort();
		return copy;
	}

	/**
	 * @brief Gets a sorted copy of this array.
	 *
	 * @tparam ComparerType The comparer type.
	 * @param comparer The comparer.
	 * @return A sorted copy of this array.
	 */
	template<typename ComparerType>
	TArray Sorted(ComparerType comparer) const
		requires TIsCallable<ECompareResult, ComparerType, ConstReferenceType, ConstReferenceType>::Value
	{
		TArray copy = *this;
		copy.Sort(MoveTemp(comparer));
		return copy;
	}

	/**
	 * @brief Removes the last element from this array and returns it.
	 *
	 * @return The last element.
	 */
	[[nodiscard]] ElementType TakeLast()
	{
		UM_ASSERT(m_NumElements >= 1, "Cannot take last element from an empty array");

		ElementType element = MoveTemp(m_Data[m_NumElements - 1]);
		RemoveAt(m_NumElements - 1, 1);
		return MoveTemp(element);
	}

	/**
	 * @brief Implicitly converts this array to a span.
	 *
	 * @return This array as a span.
	 */
	[[nodiscard]] constexpr explicit operator ConstSpanType() const
	{
		return AsSpan();
	}

	/**
	 * @brief Implicitly converts this array to a span.
	 *
	 * @return This array as a span.
	 */
	[[nodiscard]] constexpr explicit operator SpanType()
	{
		return AsSpan();
	}

	/**
	 * @brief Gets the element at the given index.
	 *
	 * @param index The index of the element to get.
	 * @return The element at the given index.
	 */
	[[nodiscard]] ConstReferenceType operator[](const SizeType index) const
	{
		return At(index);
	}

	/**
	 * @brief Gets the element at the given index.
	 *
	 * @param index The index of the element to get.
	 * @return The element at the given index.
	 */
	[[nodiscard]] ReferenceType operator[](const SizeType index)
	{
		return At(index);
	}

	/**
	 * @brief Copies another array's elements.
	 *
	 * @param other The other array to copy.
	 * @return This array.
	 */
	TArray& operator=(const TArray& other)
	{
		if (&other != this)
		{
			CopyFrom(other);
		}
		return *this;
	}

	/**
	 * @brief Assumes ownership of another array's elements.
	 *
	 * @param other The array whose elements should be moved.
	 * @return This array.
	 */
	TArray& operator=(TArray&& other) noexcept
	{
		if (&other != this)
		{
			MoveFrom(MoveTemp(other));
		}
		return *this;
	}

	// STL compatibility BEGIN
	IteratorType      begin()        { return GetData(); }
	ConstIteratorType begin()  const { return GetData(); }
	ConstIteratorType cbegin() const { return begin(); }
	IteratorType      end()          { return GetData() + Num(); }
	ConstIteratorType end()    const { return GetData() + Num(); }
	ConstIteratorType cend()   const { return end(); }
	// STL compatibility END

private:

	using LargeSizeType = int64;

	static constexpr LargeSizeType MaxCapacity { TNumericLimits<SizeType>::MaxValue };
	static constexpr LargeSizeType MinGrowCapacity { 4 };
	static constexpr SizeType      InitialCapacity { 4 };

	/**
	 * @brief Calculates the next logical capacity for a growing array.
	 *
	 * @param currentCapacity The current capacity.
	 * @param numElementsBeingAdded The number of elements being added to the array.
	 * @return The new capcity.
	 */
	static SizeType CalculateNextCapacity(const LargeSizeType currentCapacity, const LargeSizeType numElementsBeingAdded)
	{
		LargeSizeType desiredCapacity = 0;
		if (currentCapacity == 0)
		{
			desiredCapacity = numElementsBeingAdded;
			if (desiredCapacity < InitialCapacity)
			{
				desiredCapacity = InitialCapacity;
			}
		}
		else
		{
			// When viewing growth on Desmos, this is: y = x + 3x / 5 + {MinGrowCapacity}
			// Original function was "y = 5x / 3 + {MinGrowCapacity}" but that grows much faster
			desiredCapacity = currentCapacity + currentCapacity * 3 / 5 + MinGrowCapacity;
			if (desiredCapacity < currentCapacity + numElementsBeingAdded)
			{
				desiredCapacity = currentCapacity + numElementsBeingAdded;
			}
		}

		// Ensure array sizes cannot overflow
		if (desiredCapacity > MaxCapacity)
		{
			desiredCapacity = MaxCapacity;
		}

		return static_cast<SizeType>(desiredCapacity);
	}

	/**
	 * @brief Grows this array to the given capacity.
	 *
	 * @param newCapacity The new capacity for this array.
	 */
	void GrowToCapacity(const SizeType newCapacity)
	{
		UM_ASSERT(newCapacity > 0 && newCapacity > m_Capacity, "New array capacity is invalid");

		ElementType* newData = FMemory::AllocateArray<ElementType>(newCapacity);
		UM_ASSERT(newData != nullptr, "Failed to allocate new memory for array");

		if (m_Data != nullptr)
		{
			MoveConstructElements(newData, m_Data, m_NumElements);
			DestructElements(m_Data, m_NumElements);
			FMemory::Free(m_Data);
		}

		m_Data = newData;
		m_Capacity = newCapacity;
	}

	/**
	 * @brief Grows this array's capacity, if necessary, when adding the given number of elements.
	 *
	 * @param numElementsBeingAdded The number of elements being added.
	 */
	void ConditionalGrow(const SizeType numElementsBeingAdded)
	{
		const LargeSizeType numElementsAfterAdd = static_cast<LargeSizeType>(m_NumElements) + numElementsBeingAdded;
		if (numElementsAfterAdd <= static_cast<LargeSizeType>(m_Capacity))
		{
			return;
		}

		const SizeType newCapacity = CalculateNextCapacity(m_Capacity, numElementsBeingAdded);
		UM_ASSERT(newCapacity > m_Capacity, "Attempting to grow array beyond the max capacity");

		GrowToCapacity(newCapacity);
	}

	/**
	 * @brief Frees this array's elements, and then copies the given array's elements.
	 *
	 * @param other The array to copy.
	 */
	void CopyFrom(const TArray& other)
	{
		if (m_Data)
		{
			DestructElements(m_Data, m_NumElements);
			FMemory::Free(m_Data);
			m_Data = nullptr;
		}

		m_NumElements = other.m_NumElements;
		m_Capacity = m_NumElements;

		if (other.m_Data)
		{
			m_Data = FMemory::AllocateArray<ElementType>(m_Capacity);
			CopyConstructElements(m_Data, other.m_Data, m_NumElements);
		}
	}

	/**
	 * @brief Frees this array's elements, and then assumes ownership of the given array's elements.
	 *
	 * @param other The array whose elements should have their ownership transferred.
	 */
	void MoveFrom(TArray&& other) noexcept
	{
		if (m_Data)
		{
			::DestructElements(m_Data, m_NumElements);
			FMemory::Free(m_Data);
			m_Data = nullptr;
		}

		m_Data = other.m_Data;
		m_NumElements = other.m_NumElements;
		m_Capacity = other.m_Capacity;

		other.m_Data = nullptr;
		other.m_NumElements = 0;
		other.m_Capacity = 0;
	}

	ElementType* m_Data = nullptr;
	SizeType m_NumElements = 0;
	SizeType m_Capacity = 0;
};

template<typename T>
struct TIsZeroConstructible<TArray<T>> : FTrueType
{
};

/**
 * @brief Makes an array with the give number of default-constructed elements.
 *
 * @tparam T The element type.
 * @param numElements The number of elements.
 * @return The array.
 */
template<typename T>
inline TArray<T> MakeArrayWithDefaultElements(const typename TArray<T>::SizeType numElements)
{
	return TArray<T>::CreateWithDefaultElements(numElements);
}

/**
 * @brief Gets the hash code for the given array.
 *
 * @tparam T The underlying type of the array.
 * @param value The array value.
 * @return The hash code.
 */
template<typename T>
inline uint64 GetHashCode(const TArray<T>& value)
{
	const typename TArray<T>::ConstSpanType valueAsSpan = value;
	return GetHashCode(valueAsSpan);
}
