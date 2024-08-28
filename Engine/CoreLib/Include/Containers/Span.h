#pragma once

#include "Engine/Assert.h"
#include "Templates/ComparisonTraits.h"
#include "Templates/IsConstVolatile.h"
#include "Templates/IsPointer.h"
#include "Templates/IsReference.h"
#include "Templates/IsSame.h"
#include "Templates/IsZeroConstructible.h"
#include "Templates/Move.h"
#include <initializer_list>

/**
 * @brief Defines a view into a contiguous sequence of objects.
 *
 * @tparam T The type of the underlying objects.
 */
template<typename T>
class [[nodiscard]] TSpan final
{
public:

	using ElementType = T;
	using PointerType = AddPointer<ElementType>;
	using ConstPointerType = AddPointer<AddConst<ElementType>>;
	using ReferenceType = AddLValueReference<ElementType>;
	using ConstReferenceType = AddLValueReference<AddConst<ElementType>>;
	using IteratorType = T*;
	using ConstIteratorType = const T*;
	using SizeType = int32;
	using ComparisonTraits = TComparisonTraits<ElementType>;

	/**
	 * @brief Sets default values for this span's properties.
	 */
	constexpr TSpan() = default;

	/**
	 * @brief Sets the elements for this span's properties.
	 *
	 * @param elements The elements.
	 * @param numElements The number of elements.
	 */
	constexpr TSpan(ElementType* elements, const SizeType numElements)
		: m_Data { elements }
		, m_NumElements { numElements }
	{
		UM_CONSTEXPR_ASSERT(numElements >= 0, "Cannot define a span with a negative number of elements");

		if (elements == nullptr)
		{
			m_NumElements = 0;
		}
		else if (numElements == 0)
		{
			m_Data = nullptr;
		}
	}

	/**
	 * @brief Sets the elements for this span's properties.
	 *
	 * @param elements The elements.
	 */
	constexpr explicit TSpan(std::initializer_list<ElementType> elements)
		: m_Data { elements.begin() }
		, m_NumElements { static_cast<SizeType>(elements.size()) }
	{
	}

	/**
	 * @brief Gets the element at the given index.
	 *
	 * @param index The index of the element to get.
	 * @return The element at the given index.
	 */
	[[nodiscard]] constexpr ReferenceType At(const SizeType index) const
	{
		UM_CONSTEXPR_ASSERT(IsValidIndex(index), "Attempting to retrieve element with invalid index");
		return m_Data[index];
	}

	/**
	 * @brief Checks to see if a value is contained within this span.
	 *
	 * @param value The value.
	 * @return True if \p value is contained within this span, otherwise false.
	 */
	[[nodiscard]] constexpr bool Contains(const ElementType& value) const
	{
		return IndexOf(value) != INDEX_NONE;
	}

	/**
	 * @brief Checks to see if a value is contained within this span based on a predicate.
	 *
	 * @tparam PredicateType The type of the predicate.
	 * @param predicate The predicate.
	 * @return True if \p predicate has returned true for any value within this span, otherwise false.
	 */
	template<typename PredicateType>
	[[nodiscard]] constexpr bool ContainsByPredicate(PredicateType predicate) const
	{
		return IndexOfByPredicate(MoveTemp(predicate)) != INDEX_NONE;
	}

	/**
	 * @brief Finds an element by a predicate.
	 *
	 * @tparam PredicateType The predicate's type.
	 * @param predicate The predicate.
	 * @return The found element, or nullptr if it was not found.
	 */
	template<typename PredicateType>
	[[nodiscard]] constexpr ConstPointerType FindByPredicate(PredicateType predicate) const
	{
		for (SizeType idx = 0; idx < m_NumElements; ++idx)
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
	[[nodiscard]] constexpr PointerType FindByPredicate(PredicateType predicate)
	{
		for (SizeType idx = 0; idx < m_NumElements; ++idx)
		{
			if (predicate(static_cast<ConstReferenceType>(m_Data[idx])))
			{
				return m_Data + idx;
			}
		}
		return nullptr;
	}

	/**
	 * @brief Gets the index of the given value within this span.
	 *
	 * @param value The value to get the index of.
	 * @return The index of \p value, or INDEX_NONE if it was not found within this span.
	 */
	[[nodiscard]] constexpr SizeType IndexOf(const ElementType& value) const
	{
		return IndexOfByPredicate([&value](const ElementType& currentValue)
		{
			return ComparisonTraits::Equals(value, currentValue);
		});
	}

	/**
	 * @brief Gets the index of a value within this span based on a predicate.
	 *
	 * @tparam PredicateType The predicate's type.
	 * @param predicate The predicate.
	 * @return The index of the first value indicated by the \p predicate.
	 */
	template<typename PredicateType>
	[[nodiscard]] constexpr SizeType IndexOfByPredicate(PredicateType predicate) const
	{
		for (SizeType idx = 0; idx < Num(); ++idx)
		{
			if (predicate(static_cast<ConstReferenceType>(At(idx))))
			{
				return idx;
			}
		}
		return INDEX_NONE;
	}

	/**
	 * @brief Checks to see if this span is empty.
	 *
	 * @return True if this span is empty, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsEmpty() const
	{
		return m_Data == nullptr || m_NumElements == 0;
	}

	/**
	 * @brief Checks to see if an index is valid for this span.
	 *
	 * @param index The index.
	 * @return True if \p index is value for this span, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsValidIndex(const SizeType index) const
	{
		return index >= 0 && index < m_NumElements;
	}

	/**
	 * @brief Gets the data pointed to by this span.
	 *
	 * @return The data pointed to by this span.
	 */
	[[nodiscard]] constexpr PointerType GetData() const
	{
		return m_Data;
	}

	/**
	 * @brief Gets the last index of the given value within this span.
	 *
	 * @param value The value to get the index of.
	 * @return The last index of \p value, or INDEX_NONE if it was not found within this span.
	 */
	[[nodiscard]] constexpr SizeType LastIndexOf(const ElementType& value) const
	{
		return LastIndexOfByPredicate([&value](const ElementType& currentValue)
		{
			return ComparisonTraits::Equals(value, currentValue);
		});
	}

	/**
	 * @brief Gets the last index of a value within this span based on a predicate.
	 *
	 * @tparam PredicateType The predicate's type.
	 * @param predicate The predicate.
	 * @return The index of the first value indicated by the \p predicate.
	 */
	template<typename PredicateType>
	[[nodiscard]] constexpr SizeType LastIndexOfByPredicate(PredicateType predicate) const
	{
		for (SizeType idx = Num() - 1; idx >= 0; --idx)
		{
			if (predicate(static_cast<ConstReferenceType>(At(idx))))
			{
				return idx;
			}
		}
		return INDEX_NONE;
	}

	/**
	 * @brief Gets the number of elements in this span.
	 *
	 * @return The number of elements in this span.
	 */
	[[nodiscard]] constexpr SizeType Num() const
	{
		return m_NumElements;
	}

	/**
	 * @brief Gets the element at the given index.
	 *
	 * @param index The index.
	 * @return The element at the given index.
	 */
	[[nodiscard]] constexpr ReferenceType operator[](const SizeType index) const
	{
		return At(index);
	}

	/**
	 * @brief Implicitly converts this span to a const span.
	 *
	 * @return The const span.
	 */
	[[nodiscard]] constexpr operator TSpan<const ElementType>() const
	{
		return TSpan<const ElementType> { m_Data, m_NumElements };
	}

	// BEGIN STD COMPATIBILITY
	[[nodiscard]] IteratorType      begin()       { return GetData(); }
	[[nodiscard]] ConstIteratorType begin() const { return GetData(); }
	[[nodiscard]] IteratorType      end()         { return GetData() + Num(); }
	[[nodiscard]] ConstIteratorType end() const   { return GetData() + Num(); }
	// END STD COMPATIBILITY

private:

	ElementType* m_Data = nullptr;
	SizeType m_NumElements = 0;
};

template<typename T>
struct TIsZeroConstructible<TSpan<T>> : FTrueType
{
};

/**
 * @brief Casts a span from one type to another.
 *
 * @tparam OutType The output span type.
 * @tparam InType The input span type.
 * @param value The span value to cast.
 * @return The new span.
 */
template<typename OutType, typename InType>
inline TSpan<OutType> CastSpan(TSpan<InType> value)
{
	static_assert(sizeof(OutType) == sizeof(InType), "Cannot cast spans of differently sized types");
	return TSpan<OutType> { reinterpret_cast<OutType*>(value.GetData()), value.Num() };
}

/**
 * @brief Casts a span from one type to another.
 *
 * @tparam OutType The output span type.
 * @tparam InType The input span type.
 * @param value The span value to cast.
 * @return The new span.
 */
template<typename OutType, typename InType>
inline TSpan<const OutType> CastSpan(TSpan<const InType> value)
{
	static_assert(sizeof(OutType) == sizeof(InType), "Cannot cast spans of differently sized types");
	return TSpan<const OutType> { reinterpret_cast<const OutType*>(value.GetData()), value.Num() };
}