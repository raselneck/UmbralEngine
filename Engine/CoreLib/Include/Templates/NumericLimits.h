#pragma once

#include "Engine/IntTypes.h"
#include <cfloat>

template<typename T>
struct TNumericLimits;

/**
 * @brief Defines numeric limits for an 8-bit signed integer.
 */
template<>
struct TNumericLimits<int8>
{
	using Type = int8;

	static constexpr Type MinValue = -127;
	static constexpr Type MaxValue =  127;
};

/**
 * @brief Defines numeric limits for an 8-bit unsigned integer.
 */
template<>
struct TNumericLimits<uint8>
{
	using Type = uint8;

	static constexpr Type MinValue = 0;
	static constexpr Type MaxValue = 255;
};

/**
 * @brief Defines numeric limits for a 16-bit signed integer.
 */
template<>
struct TNumericLimits<int16>
{
	using Type = int16;

	static constexpr Type MinValue = -32767;
	static constexpr Type MaxValue =  32767;
};

/**
 * @brief Defines numeric limits for a 16-bit unsigned integer.
 */
template<>
struct TNumericLimits<uint16>
{
	using Type = uint16;

	static constexpr Type MinValue = 0;
	static constexpr Type MaxValue = 65535;
};

/**
 * @brief Defines numeric limits for a 32-bit signed integer.
 */
template<>
struct TNumericLimits<int32>
{
	using Type = int32;

	static constexpr Type MinValue = -2147483647;
	static constexpr Type MaxValue =  2147483647;
};

/**
 * @brief Defines numeric limits for a 32-bit unsigned integer.
 */
template<>
struct TNumericLimits<uint32>
{
	using Type = uint32;

	static constexpr Type MinValue = 0U;
	static constexpr Type MaxValue = 4294967295U;
};

/**
 * @brief Defines numeric limits for a 64-bit signed integer.
 */
template<>
struct TNumericLimits<int64>
{
	using Type = int64;

	static constexpr Type MinValue = -9223372036854775807LL;
	static constexpr Type MaxValue =  9223372036854775807LL;
};

/**
 * @brief Defines numeric limits for a 64-bit unsigned integer.
 */
template<>
struct TNumericLimits<uint64>
{
	using Type = uint64;

	static constexpr Type MinValue = 0ULL;
	static constexpr Type MaxValue = 18446744073709551615ULL;
};

/**
 * @brief Defines numeric limits for a 32-bit floating-point number.
 */
template<>
struct TNumericLimits<float>
{
	using Type = float;

	static constexpr Type MinValue = -FLT_MAX;
	static constexpr Type MaxValue =  FLT_MAX;
	static constexpr Type Epsilon  =  FLT_EPSILON;
};

/**
 * @brief Defines numeric limits for a 64-bit floating-point number.
 */
template<>
struct TNumericLimits<double>
{
	using Type = double;

	static constexpr Type MinValue = -DBL_MAX;
	static constexpr Type MaxValue =  DBL_MAX;
	static constexpr Type Epsilon  =  DBL_EPSILON;
};