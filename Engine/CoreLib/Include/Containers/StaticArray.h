#pragma once

#include "Containers/Span.h"
#include "Engine/Assert.h"
#include "Engine/Hashing.h"
#include "Memory/Memory.h"
#include "Templates/ComparisonTraits.h"
#include "Templates/TypeTraits.h"

/**
 * @brief Defines an array with a static size.
 *
 * @tparam T The array's element type.
 * @tparam N The number of items in the array.
 */
template<typename T, int32 N>
class TStaticArray final
{
	static_assert(N > 0, "Cannot have a static array with zero or less elements");

public:

	using ElementType = T;
	using PointerType = T*;
	using ConstPointerType = const T*;
	using ReferenceType = typename TAddLValueReference<ElementType>::Type;
	using ConstReferenceType = typename TAddLValueReference<typename TAddConst<ElementType>::Type>::Type;
	using SizeType = int32;
	using IteratorType = PointerType;
	using ConstIteratorType = ConstPointerType;
	using SpanType = TSpan<T>;
	using ConstSpanType = TSpan<const T>;
	using ComparisonTraits = TComparisonTraits<ElementType>;

	static constexpr SizeType StaticNum = N;

	/**
	 * @brief Gets this static array as a span.
	 *
	 * @return This static array as a span.
	 */
	[[nodiscard]] ConstSpanType AsSpan() const
	{
		return ConstSpanType { m_Data, N };
	}

	/**
	 * @brief Gets this static array as a span.
	 *
	 * @return This static array as a span.
	 */
	[[nodiscard]] SpanType AsSpan()
	{
		return SpanType { m_Data, N };
	}

	/**
	 * @brief Checks to see if this static array contains a value using the given predicate to check for the value.
	 *
	 * @tparam PredicateType The type of the predicate.
	 * @param predicate The predicate function to use to check for value equality.
	 * @return True if this static array contains a value as indicated by \p predicate, otherwise false.
	 */
	template<typename PredicateType>
	[[nodiscard]] bool ContainsByPredicate(PredicateType predicate) const
	{
		for (SizeType idx = 0; idx < StaticNum; ++idx)
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
	 * @brief Fills this static array with the given value.
	 *
	 * @param value The value.
	 */
	void Fill(const ElementType& value)
	{
		for (SizeType idx = 0; idx < N; ++idx)
		{
			m_Data[idx] = value;
		}
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
		for (SizeType idx = 0; idx < N; ++idx)
		{
			if (predicate(static_cast<ConstReferenceType>(m_Data[idx])))
			{
				return m_Data + idx;
			}
		}
		return nullptr;
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
		for (SizeType idx = 0; idx < N; ++idx)
		{
			if (predicate(static_cast<ConstReferenceType>(m_Data[idx])))
			{
				return m_Data + idx;
			}
		}
		return nullptr;
	}

	/**
	 * @brief Iterates this static array with a callback that takes an index and the element at that index.
	 *
	 * @tparam CallbackType The callback type.
	 * @param callback The callback.
	 */
	template<typename CallbackType>
	constexpr void ForEach(CallbackType&& callback) const
	{
		for (int32 idx = 0; idx < N; ++idx)
		{
			const int32 callbackIndex = idx;
			if (callback(callbackIndex, static_cast<const ElementType&>(m_Data[idx])) == EIterationDecision::Break)
			{
				break;
			}
		}
	}

	/**
	 * @brief Iterates this static array with a callback that takes an index and the element at that index.
	 *
	 * @tparam CallbackType The callback type.
	 * @param callback The callback.
	 */
	template<typename CallbackType>
	constexpr void ForEach(CallbackType&& callback)
	{
		for (int32 idx = 0; idx < N; ++idx)
		{
			const int32 callbackIndex = idx;
			if (callback(callbackIndex, m_Data[idx]) == EIterationDecision::Break)
			{
				break;
			}
		}
	}

	/**
	 * @brief Gets this static array's data.
	 *
	 * @return This static array's data.
	 */
	[[nodiscard]] constexpr ConstPointerType GetData() const
	{
		return m_Data;
	}

	/**
	 * @brief Gets this static array's data.
	 *
	 * @return This static array's data.
	 */
	[[nodiscard]] constexpr PointerType GetData()
	{
		return m_Data;
	}

	/**
	 * @brief Finds the index of an element based on a predicate callback.
	 *
	 * @tparam PredicateType The predicate's type.
	 * @param predicate The predicate.
	 * @return The index of the element specified by \p predicate, or INDEX_NONE if the element was not found.
	 */
	template<typename PredicateType>
	[[nodiscard]] constexpr SizeType IndexOfByPredicate(PredicateType predicate) const
	{
		for (SizeType idx = 0; idx < N; ++idx)
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
	[[nodiscard]] constexpr SizeType IndexOf(const ElementType& elementToSearchFor) const
	{
		return IndexOfByPredicate([&elementToSearchFor](const ElementType& element)
		{
			return ComparisonTraits::Equals(element, elementToSearchFor);
		});
	}

	/**
	 * @brief Checks to see if the given index is valid for this array.
	 *
	 * @param index The index.
	 * @return True if \p index is a valid index for this array, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsValidIndex(const SizeType index) const
	{
		return index >= 0 && index < N;
	}

	/**
	 * @brief Gets the number of elements in this array.
	 *
	 * @return The number of elements in this array.
	 */
	[[nodiscard]] constexpr SizeType Num() const
	{
		return N;
	}

	/**
	 * @brief Transforms this static array into another static array.
	 *
	 * @tparam U The other array's type.
	 * @tparam TransformFuncType The type of the transformation function.
	 * @param transformFunc The transformation function.
	 * @return The transformed array.
	 */
	template<typename U, typename TransformFuncType>
	[[nodiscard]] constexpr TStaticArray<U, N> Transform(TransformFuncType transformFunc) const
	{
		TStaticArray<U, N> result;
		for (SizeType idx = 0; idx < N; ++idx)
		{
			result[idx] = transformFunc(static_cast<ConstReferenceType>(m_Data[idx]));
		}
		return result;
	}

	/**
	 * @brief Implicitly converts this static array to a span.
	 *
	 * @return This static array as a span.
	 */
	operator ConstSpanType() const
	{
		return AsSpan();
	}

	/**
	 * @brief Implicitly converts this static array to a span.
	 *
	 * @return This static array as a span.
	 */
	operator SpanType()
	{
		return AsSpan();
	}

	/**
	 * @brief Gets the element at the given index.
	 *
	 * @param index The index of the element to retrieve.
	 * @return The element at \p index.
	 */
	[[nodiscard]] constexpr ConstReferenceType operator[](const SizeType index) const
	{
		UM_CONSTEXPR_ASSERT(IsValidIndex(index), "Given static array index is not valid");
		return m_Data[index];
	}

	/**
	 * @brief Gets the element at the given index.
	 *
	 * @param index The index of the element to retrieve.
	 * @return The element at \p index.
	 */
	[[nodiscard]] constexpr ReferenceType operator[](const SizeType index)
	{
		UM_CONSTEXPR_ASSERT(IsValidIndex(index), "Given static array index is not valid");
		return m_Data[index];
	}

	// STL compatibility BEGIN
	IteratorType      begin()       { return GetData(); }
	ConstIteratorType begin() const { return GetData(); }
	IteratorType      end()         { return GetData() + Num(); }
	ConstIteratorType end()   const { return GetData() + Num(); }
	// STL compatibility END

	ElementType m_Data[N] {};
};

template<typename T, int32 N>
struct TIsZeroConstructible<TStaticArray<T, N>> : TIsZeroConstructible<T>
{
};

/**
 * @brief Gets the hash code for the given array.
 *
 * @tparam T The underlying type of the array.
 * @param value The array value.
 * @return The hash code.
 */
template<typename T, int32 N>
inline uint64 GetHashCode(const TStaticArray<T, N>& value)
{
	const typename TStaticArray<T, N>::ConstSpanType valueAsSpan = value;
	return GetHashCode(valueAsSpan);
}