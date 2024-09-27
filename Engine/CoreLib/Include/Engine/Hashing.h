#pragma once

#include "Containers/Span.h"
#include "Engine/Assert.h"
#include "Templates/Forward.h"
#include "Templates/IsChar.h"
#include "Templates/IsEnum.h"
#include "Templates/IsPOD.h"
#include "Templates/NumericLimits.h"
#include "Templates/UnderlyingType.h"
#include "Templates/VariadicTraits.h"

enum : uint64 { INVALID_HASH = static_cast<uint64>(-1) };

namespace Private
{
	/**
	 * @brief Hashes a byte array with an initial hash value.
	 *
	 * @param bytes The byte array.
	 * @param initialHash The initial hash.
	 * @return The hash code.
	 */
	uint64 HashBytesWithInitialHash(TSpan<const uint8> bytes, uint64 initialHash);

	/**
	 * @brief Hashes a byte array.
	 *
	 * @param bytes The byte array.
	 * @return The hash code.
	 */
	uint64 HashBytes(TSpan<const uint8> bytes);

	/**
	 * @brief Combines two hash codes.
	 *
	 * @param firstHash The first hash code.
	 * @param secondHash The second hash code.
	 * @return The combined hash.
	 */
	uint64 HashCombine(uint64 firstHash, uint64 secondHash); // TODO Rename to CombineHashCodes

	/**
	 * @brief Casts a POD type to bytes.
	 *
	 * @tparam T The POD type.
	 * @param value The value to cast.
	 * @return The span of bytes for the value.
	 */
	template<typename T>
	TSpan<const uint8> CastToBytes(const T& value)
		requires TIsPOD<T>::Value
	{
		constexpr int32 spanLength = static_cast<int32>(sizeof(T));
		return TSpan<const uint8> { reinterpret_cast<const uint8*>(&value), spanLength };
	}

	/**
	 * @brief Casts a span of characters to a span of bytes.
	 *
	 * @tparam T The character type.
	 * @param value The value to cast.
	 * @return The span of bytes for the value.
	 */
	template<typename T>
	TSpan<const uint8> CastToBytes(const TSpan<const T> value)
		requires TIsChar<T>::Value
	{
		const int32 spanLength = static_cast<int32>(sizeof(T)) * value.Num();
		return TSpan<const uint8> { reinterpret_cast<const uint8*>(value.GetData()), spanLength };
	}
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const int8 value)
{
	return static_cast<uint64>(value);
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const int16 value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const int32 value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const int64 value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const uint8 value)
{
	return static_cast<uint64>(value);
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const uint16 value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const uint32 value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const uint64 value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const float value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const double value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const char value)
{
	return static_cast<uint64>(value);
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const char8_t value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const char16_t value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const char32_t value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const wchar_t value)
{
	return Private::HashBytes(Private::CastToBytes(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @param value The value.
 * @return The hash code.
 */
inline uint64 GetHashCode(const void* value)
{
	static_assert(sizeof(uint64) >= sizeof(void*), "uint64 cannot hold a pointer");
	return GetHashCode(reinterpret_cast<uint64>(value));
}

/**
 * @brief Gets a hash code for the given value.
 *
 * @tparam EnumType The type of the enum.
 * @param value The value.
 * @return The hash code.
 */
template<typename EnumType>
inline uint64 GetHashCode(const EnumType value)
	requires TIsEnum<EnumType>::Value
{
	const auto valueAsUnderlying = static_cast<typename TUnderlyingType<EnumType>::Type>(value);
	return GetHashCode(Private::CastToBytes(valueAsUnderlying));
}

/**
 * @brief Gets the hash code for the given span of items.
 *
 * @tparam T The type of each item.
 * @param value The span of items.
 * @return The hash code.
 */
template<typename T>
inline uint64 GetHashCode(const TSpan<const T> value)
{
	if (value.IsEmpty())
	{
		return 0;
	}

	uint64 hash = GetHashCode(value[0]);
	for (int32 idx = 1; idx < value.Num(); ++idx)
	{
		hash = Private::HashCombine(hash, GetHashCode(value[idx]));
	}

	return hash;
}

// TODO Rename the below overloads to HashCombine or just make them overloads of GetHashCode

/**
 * @brief Hashes the given collection of items.
 *
 * @tparam ElementType The type of each item.
 * @tparam SizeType The type of the size parameter.
 * @param items The items to hash.
 * @param numItems The number of items to hash.
 * @return The hash code.
 */
template<typename ElementType, typename SizeType>
inline uint64 HashItems(const ElementType* items, const SizeType numItems)
{
	constexpr uint64 maxNumItems = static_cast<uint64>(TNumericLimits<int32>::MaxValue);
	UM_ASSERT(numItems >= 0, "Given a negative number of items to hash");
	UM_ASSERT(numItems <= maxNumItems, "Given too many items to hash");

	return GetHashCode(TSpan<const ElementType> { items, static_cast<int32>(numItems) });
}

/**
 * @brief Hashes the given collection of items.
 *
 * @tparam ElementType The type of each item.
 * @tparam N The number of items to hash.
 * @param items The items to hash.
 * @return The hash code.
 */
template<typename ElementType, size_t N>
inline uint64 HashItems(const ElementType (&items)[N])
{
	constexpr uint64 maxNumItems = static_cast<uint64>(TNumericLimits<int32>::MaxValue);
	static_assert(N <= maxNumItems, "Given too many items to hash");

	return GetHashCode(TSpan<const ElementType> { items, static_cast<int32>(N) });
}

/**
 * @brief Hashes the given collection of items.
 *
 * @tparam FirstType The type of the first item.
 * @tparam OtherTypes The types of the other items.
 * @param firstValue The first item.
 * @param otherValues The other items.
 * @return The hash code.
 */
template<typename FirstType, typename... OtherTypes>
inline uint64 HashItems(const FirstType& firstValue, OtherTypes&&... otherValues)
{
	uint64 hash = GetHashCode(firstValue);
	([&]()
	{
		hash = Private::HashCombine(hash, GetHashCode(otherValues));
	}, ...);
	return hash;
}