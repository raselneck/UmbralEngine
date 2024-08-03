#pragma once

#include "Templates/IsEnum.h"
#include "Templates/IsInt.h"
#include "Templates/UnderlyingType.h"
#include <compare> /* For std::strong_ordering */

/**
 * @brief Defines comparison operators for an enumeration type.
 */
#define ENUM_COMPARISON_OPERATORS(EnumType)                                                                         \
	constexpr auto operator<=>(const EnumType first, const EnumType second)                                         \
	{                                                                                                               \
		using UnderlyingType = TUnderlyingType<EnumType>::Type;                                                     \
		return static_cast<UnderlyingType>(first) <=> static_cast<UnderlyingType>(second);                          \
	}

/**
 * @brief Defines flag-based operators for an enumeration type.
 */
#define ENUM_FLAG_OPERATORS(EnumType)                                                                               \
	ENUM_COMPARISON_OPERATORS(EnumType)                                                                             \
	constexpr EnumType operator|(const EnumType first, const EnumType second)                                       \
	{                                                                                                               \
		using UnderlyingType = TUnderlyingType<EnumType>::Type;                                                     \
		UnderlyingType result = static_cast<UnderlyingType>(first) | static_cast<UnderlyingType>(second);           \
		return static_cast<EnumType>(result);                                                                       \
	}                                                                                                               \
	constexpr EnumType operator|=(EnumType& first, const EnumType second)                                           \
	{                                                                                                               \
		using UnderlyingType = TUnderlyingType<EnumType>::Type;                                                     \
		UnderlyingType result = static_cast<UnderlyingType>(first) | static_cast<UnderlyingType>(second);           \
		first = static_cast<EnumType>(result);                                                                      \
		return first;                                                                                               \
	}                                                                                                               \
	constexpr EnumType operator&(const EnumType first, const EnumType second)                                       \
	{                                                                                                               \
		using UnderlyingType = TUnderlyingType<EnumType>::Type;                                                     \
		UnderlyingType result = static_cast<UnderlyingType>(first) & static_cast<UnderlyingType>(second);           \
		return static_cast<EnumType>(result);                                                                       \
	}                                                                                                               \
	constexpr EnumType operator&=(EnumType& first, const EnumType second)                                           \
	{                                                                                                               \
		using UnderlyingType = TUnderlyingType<EnumType>::Type;                                                     \
		UnderlyingType result = static_cast<UnderlyingType>(first) & static_cast<UnderlyingType>(second);           \
		first = static_cast<EnumType>(result);                                                                      \
		return first;                                                                                               \
	}                                                                                                               \
	constexpr EnumType operator~(const EnumType value)                                                              \
	{                                                                                                               \
		using UnderlyingType = TUnderlyingType<EnumType>::Type;                                                     \
		UnderlyingType result = ~static_cast<UnderlyingType>(value);                                                \
		return static_cast<EnumType>(result);                                                                       \
	}                                                                                                               \
	constexpr bool HasFlag(const EnumType value, const EnumType flagToCheck)                                        \
	{                                                                                                               \
		return ::HasFlag<EnumType>(value, flagToCheck);                                                             \
	}                                                                                                               \
	constexpr EnumType AddFlag(const EnumType value, const EnumType flagToAdd)                                      \
	{                                                                                                               \
		return ::AddFlag<EnumType>(value, flagToAdd);                                                               \
	}                                                                                                               \
	constexpr EnumType RemoveFlag(const EnumType value, const EnumType flagToRemove)                                \
	{                                                                                                               \
		return ::RemoveFlag<EnumType>(value, flagToRemove);                                                         \
	}

/**
 * @brief Adds a flag to a value.
 *
 * @tparam ValueType The value's type.
 * @tparam FlagType The flag's type.
 * @param value The value.
 * @param flagToAdd The flag to add.
 * @return \p value with the flag(s) \p flagToAdd added.
 */
template<typename ValueType, typename FlagType>
constexpr ValueType AddFlag(const ValueType value, const FlagType flagToAdd)
{
	static_assert(Or<TIsInt<ValueType>, TIsEnum<ValueType>>, "AddFlag expects int or enum types");

	using ComparisonType = typename TUnderlyingType<ValueType>::Type;
	const ComparisonType result = static_cast<ComparisonType>(value) | static_cast<ComparisonType>(flagToAdd);
	return static_cast<ValueType>(result);
}

/**
 * @brief Adds a flag to a value.
 *
 * @tparam ValueType The value's type.
 * @tparam FlagType The flag's type.
 * @param value The value.
 * @param flagToAdd The flag to add.
 */
template<typename ValueType, typename FlagType>
constexpr void AddFlagTo(ValueType& value, const FlagType flagToAdd)
{
	using UnderlyingType = typename TUnderlyingType<ValueType>::Type;
	static_cast<UnderlyingType&>(value) |= static_cast<UnderlyingType>(flagToAdd);
}

/**
 * @brief Checks to see if the given value has a flag.
 *
 * @tparam ValueType The value's type.
 * @tparam FlagType The flag's type.
 * @param value The value.
 * @param flagToCheck The flag to check for.
 * @return True if \p value has the flag(s) \p valueToCheck, otherwise false.
 */
template<typename ValueType, typename FlagType>
constexpr bool HasFlag(const ValueType value, const FlagType flagToCheck)
{
	static_assert(Or<TIsInt<ValueType>, TIsEnum<ValueType>>, "HasFlag expects int or enum types");

	using ComparisonType = typename TUnderlyingType<ValueType>::Type;
	return (static_cast<ComparisonType>(value) & static_cast<ComparisonType>(flagToCheck)) != 0;
}

/**
 * @brief Removes a flag from a value.
 *
 * @tparam ValueType The value's type.
 * @tparam FlagType The flag's type.
 * @param value The value.
 * @param flagToRemove The flag to remove.
 * @return \p value with the flag(s) \p flagToRemove removed.
 */
template<typename ValueType, typename FlagType>
constexpr ValueType RemoveFlag(const ValueType value, const FlagType flagToRemove)
{
	static_assert(Or<TIsInt<ValueType>, TIsEnum<ValueType>>, "RemoveFlag expects int or enum types");

	using ComparisonType = typename TUnderlyingType<ValueType>::Type;
	const ComparisonType result = static_cast<ComparisonType>(value) & ~static_cast<ComparisonType>(flagToRemove);
	return static_cast<ValueType>(result);
}

/**
 * @brief Removes a flag from a value.
 *
 * @tparam ValueType The value's type.
 * @tparam FlagType The flag's type.
 * @param value The value.
 * @param flagToRemove The flag to remove.
 */
template<typename ValueType, typename FlagType>
constexpr void RemoveFlagFrom(ValueType& value, const FlagType flagToRemove)
{
	using UnderlyingType = typename TUnderlyingType<ValueType>::Type;
	static_cast<UnderlyingType&>(value) &= ~static_cast<UnderlyingType>(flagToRemove);
}