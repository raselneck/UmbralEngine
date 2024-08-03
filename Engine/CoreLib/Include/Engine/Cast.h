#pragma once

#include "Engine/Assert.h"
#include "Engine/Platform.h"
#include "Templates/AndNotOr.h"
#include "Templates/IsBaseOf.h"
#include "Templates/IsConstVolatile.h"
#include "Templates/IsVoid.h"

namespace Private
{
	template<typename FromType, typename ToType>
	struct TCastHelper
	{
		static ToType* Cast(FromType* value)
		{
			if (value == nullptr)
			{
				return nullptr;
			}

			// If the destination type is a base of the source type, it's an easy conversion
			if constexpr (TIsBaseOf<ToType, FromType>::Value)
			{
				return value;
			}

			// If the destination type derives from the source type, we can dynamic cast
			// HACK: This needs to be a ternary, otherwise we may run into "unreachable code" warnings from MSVC
			return TIsBaseOf<FromType, ToType>::Value ? dynamic_cast<ToType*>(value) : nullptr;
		}
	};

	template<typename FromType, typename ToType>
	struct TCastHelper<const FromType, const ToType>
	{
		static const ToType* Cast(const FromType* value)
		{
			using NonConstCastType = TCastHelper<FromType, ToType>;
			return NonConstCastType::Cast(const_cast<FromType*>(value));
		}
	};
}

/**
 * @brief Attempts to cast a value from one type to another.
 *
 * @tparam ToType The destination type.
 * @tparam FromType The source type.
 * @param value The value to cast.
 * @return \p value as \p ToType, or nullptr if the cast was not possible.
 */
template<typename ToType, typename FromType>
inline decltype(auto) Cast(FromType* value)
{
	static_assert(Not<TIsVoid<ToType>>, "Casting to a void pointer is unnecessary");

	using ResultType = ConditionalAddConst<IsConst<FromType>, ToType>;
	return ::Private::TCastHelper<FromType, ResultType>::Cast(value);
}

/**
 * @brief Casts a value from one type to another. Will assert if the value does not cast to a valid value.
 *
 * @tparam ToType The destination type.
 * @tparam FromType The source type.
 * @param value The value to cast.
 * @return \p value as \p ToType, or nullptr if the cast was not possible.
 */
template<typename ToType, typename FromType>
inline decltype(auto) CastChecked(FromType* value)
{
	auto* result = ::Cast<ToType, FromType>(value);

	UM_ASSERT(result != nullptr, "Failed to cast value to destination type");

	return result;
}

/**
 * @brief Checks to see if a value is of the given type.
 *
 * @tparam TestType The type to test for.
 * @tparam ValueType The type of the value being tested.
 * @param value The value to test.
 * @return True if \p value is of the given type, otherwise false.
 */
template<typename TestType, typename ValueType>
inline bool Is(const ValueType* value)
	requires(Not<TIsVoid<TestType>>)
{
	return Cast<TestType>(value) != nullptr;
}

/**
 * @brief Checks to see if a value is of the given type.
 *
 * @tparam TestType The type to test for.
 * @tparam ValueType The type of the value being tested.
 * @param value The value to test.
 * @return True if \p value is of the given type, otherwise false.
 */
template<typename TestType, typename ValueType>
inline bool Is(const ValueType& value)
	requires(Not<TIsVoid<TestType>>)
{
	return Is<TestType>(&value);
}