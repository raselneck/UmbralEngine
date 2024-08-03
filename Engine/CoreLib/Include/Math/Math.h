#pragma once

#include "Engine/IntTypes.h"
#include "Templates/TypeTraits.h"

/**
 * @brief A collection of mathematical constants.
 */
template<typename FloatType>
	requires TIsFloat<FloatType>::Value
struct TMathConstants
{
#define DECLARE_CONSTANT(Name, Value) static constexpr FloatType Name = static_cast<FloatType>(Value)

	/** @brief The constant E (Euler's number). */
	DECLARE_CONSTANT(Euler, 2.7182818284590452353602874713526624977572470936999595749669676277);

	/** @brief The constant log(E, 10) (log base 10 of E). */
	DECLARE_CONSTANT(Log10E, 0.4342944819032518276511289189166050822943970058036665661144537831);

	/** @brief The constant log(E, 2) (log base 2 of E). */
	DECLARE_CONSTANT(Log2E, 1.4426950408889634073599246810018921374266459541529859341354494069);

	/** @brief The constant pi. */
	DECLARE_CONSTANT(Pi, 3.1415926535897932384626433832795028841971693993751058209749445923);

	/** @brief The constant pi/2. */
	DECLARE_CONSTANT(HalfPi, 1.5707963267948966192313216916397514420985846996875529104874722961);

	/** @brief The constant pi/4. */
	DECLARE_CONSTANT(QuarterPi, 0.7853981633974483096156608458198757210492923498437764552437361480);

	/** @brief The constant tau. */
	DECLARE_CONSTANT(Tau, 6.2831853071795864769252867665590057683943387987502116419498891846);

	/** @brief The constant pi*2. */
	DECLARE_CONSTANT(TwoPi, Tau);

	/** @brief The constant used to convert a value from degrees to radians. */
	DECLARE_CONSTANT(DegreesToRadians, 0.0174532925199432957692369076848861271344287188854172545609719144);

	/** @brief The constant used to convert a value from radians to degrees. */
	DECLARE_CONSTANT(RadiansToDegrees, 57.295779513082320876798154814105170332405472466564321549160243861);

	/** @brief The constant sqrt(2). */
	DECLARE_CONSTANT(Sqrt2, 1.4142135623730950488016887242096980785696718753769480731766797379);

	/** @brief The constant sqrt(3). */
	DECLARE_CONSTANT(Sqrt3, 1.7320508075688772935274463415058723669428052538103806280558069794);

	/** @brief The constant sqrt(5). */
	DECLARE_CONSTANT(Sqrt5, 2.2360679774997896964091736687312762354406183596115257242708972454);

	/** @brief A kind of small number, a-la Unreal. */
	DECLARE_CONSTANT(KindaSmallNumber, 1.0E-4);

	/** @brief A small number, a-la Unreal. */
	DECLARE_CONSTANT(SmallNumber, 1.0E-6);

#undef DECLARE_CONSTANT
};

/**
 * @brief Defines a collection of math utility functions and constants.
 */
class FMath final : public TMathConstants<float>
{
	using Super = TMathConstants<float>;

public:

	/**
	 * @brief Returns the absolute value of the given value.
	 *
	 * @param value The value.
	 * @return The absolute value.
	 */
	static constexpr int32 Abs(const int32 value) noexcept
	{
		return value < 0 ? -value : value;
	}

	/**
	 * @brief Returns the absolute value of the given value.
	 *
	 * @param value The value.
	 * @return The absolute value.
	 */
	static constexpr float Abs(const float value) noexcept
	{
		return value < 0.0f ? -value : value;
	}

	/**
	 * @brief Returns the absolute value of the given value.
	 *
	 * @param value The value.
	 * @return The absolute value.
	 */
	static constexpr double Abs(const double value) noexcept
	{
		return value < 0.0 ? -value : value;
	}

	/**
	 * @brief Returns the Cartesian coordinate for one axis of a point that is defined by a given triangle and two
	 *        normalized barycentric (areal) coordinates.
	 *
	 * @param value1 The coordinate on one axis of vertex 1 of the defining triangle.
	 * @param value2 The coordinate on the same axis of vertex 2 of the defining triangle.
	 * @param value3 The coordinate on the same axis of vertex 3 of the defining triangle.
	 * @param amount1 The normalized barycentric (areal) coordinate \b b2, equal to the weighting factor for vertex
	 * 2, the coordinate of which is specified in \em value2.
	 * @param amount2 The normalized barycentric (areal) coordinate \b b3, equal to the weighting factor for vertex
	 * 3, the coordinate of which is specified in \em value3.
	 * @return Cartesian coordinate of the specified point with respect to the axis being used.
	 */
	static float Barycentric(float value1, float value2, float value3, float amount1, float amount2) noexcept;

	/**
	 * @brief Performs a Catmull-Rom interpolation using the specified positions.
	 *
	 * @param value1 The first position in the interpolation.
	 * @param value2 The second position in the interpolation.
	 * @param value3 The third position in the interpolation.
	 * @param value4 The fourth position in the interpolation.
	 * @param amount The weighting factor.
	 * @return A position that is the result of the Catmull-Rom interpolation.
	 */
	static float CatmullRom(float value1, float value2, float value3, float value4, float amount) noexcept;

	/**
	 * @brief Rounds \p value upward, returning the smallest integral value that is not less than \p value.
	 *
	 * @param value The value to round up.
	 * @return The smallest integral value that is not less than \p value.
	 */
	static constexpr float Ceil(const float value)
	{
		// https://stackoverflow.com/a/54213092
		const float truncated = FMath::Truncate(value);
		return truncated + static_cast<float>(truncated < value);
	}

	/**
	 * @brief Rounds \p value upward, returning the smallest integral value that is not less than \p value.
	 *
	 * @param value The value to round up.
	 * @return The smallest integral value that is not less than \p value.
	 */
	static constexpr double Ceil(const double value)
	{
		// https://stackoverflow.com/a/54213092
		const double truncated = FMath::Truncate(value);
		return truncated + (truncated < value);
	}

	/**
	 * @brief Gets the cosine of a given radian value.
	 *
	 * @param radians The value in radians.
	 * @return The cosine of \p radians.
	 */
	static double Cos(double radians);

	/**
	 * @brief Gets the cosine of a given radian value.
	 *
	 * @param radians The value in radians.
	 * @return The cosine of \p radians.
	 */
	static float Cos(float radians);

	/**
	 * @brief Restricts a value to be within a specified range.
	 *
	 * @param value The value to clamp.
	 * @param minValue The minimum value.
	 * @param maxValue The maximum value.
	 * @return The clamped value.
	 */
	template<typename T>
	static constexpr T Clamp(const T value, const T minValue, const T maxValue) noexcept
	{
		if (value < minValue)
		{
			return minValue;
		}
		if (value > maxValue)
		{
			return maxValue;
		}
		return value;
	}

	/**
	 * @brief Rounds \p value downward, returning the largest integral value that is not greater than \p value.
	 *
	 * @param value The value to round down.
	 * @return The value of value rounded downward.
	 */
	static constexpr float Floor(const float value)
	{
		// Adapted from Ceil implementation
		const float truncated = FMath::Truncate(value);
		return truncated - static_cast<float>(truncated > value);
	}

	/**
	 * @brief Rounds \p value downward, returning the largest integral value that is not greater than \p value.
	 *
	 * @param value The value to round down.
	 * @return The value of \p value rounded downward.
	 */
	static constexpr double Floor(const double value)
	{
		// Adapted from Ceil implementation
		const double truncated = FMath::Truncate(value);
		return truncated - (truncated > value);
	}

	/**
	 * @brief Performs a Hermite spline interpolation.
	 *
	 * @param value1 The first source position.
	 * @param tangent1 The first source tangent.
	 * @param value2 The second source position.
	 * @param tangent2 The second source tangent.
	 * @param amount The weighting factor.
	 * @return The result of the Hermite spline interpolation.
	 */
	static float Hermite(float value1, float tangent1, float value2, float tangent2, float amount) noexcept;

	/**
	 * @brief Checks to see if the two values are nearly equal.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @param smallNumber The maximum allowed difference between the two values.
	 * @return True if the two values differ by less than the smallNumber, otherwise false.
	 */
	static constexpr bool IsNearlyEqual(float first, float second, float smallNumber = FMath::SmallNumber) noexcept
	{
		return FMath::Abs(first - second) <= smallNumber;
	}

	/**
	 * @brief Checks to see if the two values are nearly equal.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @param tolerance The maximum allowed difference between the two values.
	 * @return True if the two values differ by less than the tolerance, otherwise false.
	 */
	static constexpr bool IsNearlyEqual(double first, double second, double tolerance = FMath::SmallNumber) noexcept
	{
		return FMath::Abs(first - second) <= tolerance;
	}

	/**
	 * @brief Checks to see if the given value is nearly zero.
	 *
	 * @param value The value.
	 * @param tolerance The maximum allowed difference between the value and zero.
	 * @return True if the value differs from zero by less than the tolerance, otherwise false.
	 */
	static constexpr bool IsNearlyZero(float value, float tolerance = FMath::SmallNumber) noexcept
	{
		return FMath::Abs(value) <= tolerance;
	}

	/**
	 * @brief Checks to see if the given value is nearly zero.
	 *
	 * @param value The value.
	 * @param tolerance The maximum allowed difference between the value and zero.
	 * @return True if the value differs from zero by less than the tolerance, otherwise false.
	 */
	static constexpr bool IsNearlyZero(const double value, const double tolerance = FMath::SmallNumber) noexcept
	{
		return FMath::Abs(value) <= tolerance;
	}

	/**
	 * @brief Determines if value is powered by two.
	 *
	 * @param value The value.
	 * @return True if the value is a power of two, otherwise false.
	 */
	static constexpr bool IsPowerOfTwo(const int32 value) noexcept
	{
		return (value > 0) && ((value & (value - 1)) == 0);
	}

	/**
	 * @brief Determines if value is powered by two.
	 *
	 * @param value The value.
	 * @return True if the value is a power of two, otherwise false.
	 */
	static constexpr bool IsPowerOfTwo(const uint32 value) noexcept
	{
		return (value & (value - 1)) == 0;
	}

	/**
	 * @brief Determines if value is powered by two.
	 *
	 * @param value The value.
	 * @return True if the value is a power of two, otherwise false.
	 */
	static constexpr bool IsPowerOfTwo(const int64 value) noexcept
	{
		return (value > 0) && ((value & (value - 1)) == 0);
	}

	/**
	 * @brief Determines if value is powered by two.
	 *
	 * @param value The value.
	 * @return True if the value is a power of two, otherwise false.
	 */
	static constexpr bool IsPowerOfTwo(const uint64 value) noexcept
	{
		return (value & (value - 1)) == 0;
	}

	/**
	 * @brief Checks to see if a value is within a range, excluding the extremes.
	 *
	 * @tparam T The value type.
	 * @param value The value.
	 * @param minValue The minimum value.
	 * @param maxValue The maximum value.
	 * @return True if \p value is between \p minValue and \p maxValue, but is also neither of those values.
	 */
	template<typename T>
	static constexpr bool IsWithinRangeExclusive(const T value, const T minValue, const T maxValue)
	{
		return value > minValue && value < maxValue;
	}

	/**
	 * @brief Checks to see if a value is within a range, including the extremes.
	 *
	 * @tparam T The value type.
	 * @param value The value.
	 * @param minValue The minimum value.
	 * @param maxValue The maximum value.
	 * @return True if \p value is between \p minValue and \p maxValue, or is exactly one of those values.
	 */
	template<typename T>
	static constexpr bool IsWithinRangeInclusive(const T value, const T minValue, const T maxValue)
	{
		return value >= minValue && value <= maxValue;
	}

	/**
	 * @brief Linearly interpolates between two values.
	 *
	 * @param from The source value.
	 * @param to The destination value.
	 * @param amount The weighting factor.
	 * @return Interpolated value.
	 */
	static constexpr float Lerp(const float from, const float to, const float amount) noexcept
	{
		return from + (to - from) * amount;
	}

	/**
	 * @brief Linearly interpolates between two values. This method is a less efficient, more precise version of Lerp.
	 *
	 * @param from The source value.
	 * @param to The destination value.
	 * @param amount The weighting factor.
	 * @return The interpolated value.
	 */
	static constexpr float LerpPrecise(const float from, const float to, const float amount) noexcept
	{
		return ((1.0f - amount) * from) + (to * amount);
	}

	/**
	 * @brief Gets the maximum of two values.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return The maximum of the two values.
	 */
	template<typename T>
	static constexpr T Max(const T first, const T second) noexcept
	{
		return first > second ? first : second;
	}

	/**
	 * @brief Gets the minimum of two values.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return The minimum of the two values.
	 */
	template<typename T>
	static constexpr T Min(const T first, const T second) noexcept
	{
		return first < second ? first : second;
	}

	/**
	 * @brief Returns the floating-point remainder of numer/denom (rounded towards zero):
	 *
	 * Modf = numerator - tquot * denominator
	 *
	 * Where tquot is the truncated (i.e. rounded towards zero) result of: numerator/denominator.
	 *
	 * @param numerator Value of the quotient numerator.
	 * @param denominator Value of the quotient denominator.
	 * @return The remainder of dividing the arguments.
	 */
	static constexpr float Modf(const float numerator, const float denominator)
	{
		// https://cplusplus.com/reference/cmath/fmod/
		return numerator - FMath::Truncate(numerator / denominator) * denominator;
	}

	/**
	 * @brief Returns the floating-point remainder of numer/denom (rounded towards zero):
	 *
	 * Modf = numerator - tquot * denominator
	 *
	 * Where tquot is the truncated (i.e. rounded towards zero) result of: numerator/denominator.
	 *
	 * @param numerator Value of the quotient numerator.
	 * @param denominator Value of the quotient denominator.
	 * @return The remainder of dividing the arguments.
	 */
	static constexpr double Modf(const double numerator, const double denominator)
	{
		// https://cplusplus.com/reference/cmath/fmod/
		return numerator - FMath::Truncate(numerator / denominator) * denominator;
	}

	/**
	 * @brief Computes the nearest integer value to \p value (in floating-point format), rounding
	 *        halfway cases away from zero, regardless of the current rounding mode.
	 *
	 * @param value The value to round.
	 * @return The rounded value.
	 */
	static constexpr float Round(const float value) noexcept
	{
		if (value > 0.0f)
		{
			return FMath::Floor(value + 0.5f);
		}
		return FMath::Ceil(value - 0.5f);
	}

	/**
	 * @brief Computes the nearest integer value to \p value (in floating-point format), rounding
	 *        halfway cases away from zero, regardless of the current rounding mode.
	 *
	 * @param value The value to round.
	 * @return The rounded value.
	 */
	static constexpr double Round(const double value) noexcept
	{
		if (value > 0.0)
		{
			return FMath::Floor(value + 0.5);
		}
		return FMath::Ceil(value - 0.5);
	}

	/**
	 * @brief Computes the nearest integer value to \p value, rounding
	 *        halfway cases away from zero, regardless of the current rounding mode.
	 *
	 * @param value The value to round.
	 * @return The rounded value as an integer.
	 */
	static constexpr int32 RoundToInt(const float value) noexcept
	{
		return static_cast<int32>(FMath::Round(value));
	}

	/**
	 * @brief Computes the nearest integer value to \p value, rounding
	 *        halfway cases away from zero, regardless of the current rounding mode.
	 *
	 * @param value The value to round.
	 * @return The rounded value as an integer.
	 */
	static constexpr int64 RoundToInt(const double value) noexcept
	{
		return static_cast<int64>(FMath::Round(value));
	}

	/**
	 * @brief Performs a safe division of two values.
	 *
	 * @param value The numerator of the division.
	 * @param divisor The denominator of the division.
	 * @return The result of the division, or zero if divisor is approximately zero.
	 */
	static constexpr int32 SafeDivide(const int32 numerator, const int32 denominator) noexcept
	{
		return denominator == 0 ? 0 : numerator / denominator;
	}

	/**
	 * @brief Performs a safe division of two values.
	 *
	 * @param numerator The numerator of the division.
	 * @param denominator The denominator of the division.
	 * @return The result of the division, or zero if denominator is approximately zero.
	 */
	static constexpr float SafeDivide(const float numerator, const float denominator) noexcept
	{
		return IsNearlyEqual(denominator, 0.0f) ? 0.0f : (numerator / denominator);
	}

	/**
	 * @brief Saturates the given value. (Clamps it to be between zero and one.)
	 *
	 * @param value The value to saturate.
	 * @return \p value saturated.
	 */
	static constexpr float Saturate(const float value)
	{
		return Clamp(value, 0.0f, 1.0f);
	}

	/**
	 * @brief Saturates the given value. (Clamps it to be between zero and one.)
	 *
	 * @param value The value to saturate.
	 * @return \p value saturated.
	 */
	static constexpr double Saturate(const double value)
	{
		return Clamp(value, 0.0, 1.0);
	}

	/**
	 * @brief Gets the sine of a given radian value.
	 *
	 * @param radians The value in radians.
	 * @return The sine of \p radians.
	 */
	static double Sin(double radians);

	/**
	 * @brief Gets the sine of a given radian value.
	 *
	 * @param radians The value in radians.
	 * @return The sine of \p radians.
	 */
	static float Sin(float radians);

	/**
	 * @brief Calculates the square root of the given value.
	 *
	 * @param value The value to calculate the square root of.
	 * @return The square root of \p value.
	 */
	static float Sqrt(float value) noexcept;

	/**
	 * @brief Calculates the square root of the given value.
	 *
	 * @param value The value to calculate the square root of.
	 * @return The square root of \p value.
	 */
	static double Sqrt(double value) noexcept;

	/**
	 * @brief Interpolates between two values using a cubic equation.
	 *
	 * @param from The source value.
	 * @param to The destination value.
	 * @param amount The weighting value.
	 * @return The interpolated value.
	 */
	static float SmoothStep(float from, float to, float amount) noexcept;

	/**
	 * @brief Converts the given value from radians to degrees.
	 *
	 * @param radians The value in radians.
	 * @return The value in degrees.
	 */
	static constexpr float ToDegrees(const float radians) noexcept
	{
		return static_cast<float>(radians * TMathConstants<double>::RadiansToDegrees);
	}

	/**
	 * @brief Converts the given value from degrees to radians.
	 *
	 * @param degrees The value in degrees.
	 * @return The value in radians.
	 */
	static constexpr float ToRadians(const float degrees) noexcept
	{
		return static_cast<float>(degrees * TMathConstants<double>::DegreesToRadians);
	}

	/**
	 * @brief Rounds \p value toward zero, returning the nearest integral value that is not larger in magnitude than \p value.
	 *
	 * @param value The value to truncate.
	 * @return The nearest integral value that is not larger in magnitude than \p value.
	 */
	static constexpr float Truncate(const float value)
	{
		return static_cast<float>(static_cast<int32>(value));
	}

	/**
	 * @brief Rounds \p value toward zero, returning the nearest integral value that is not larger in magnitude than \p value.
	 *
	 * @param value The value to truncate.
	 * @return The nearest integral value that is not larger in magnitude than \p value.
	 */
	static constexpr double Truncate(const double value)
	{
		return static_cast<double>(static_cast<int64>(value));
	}

	/**
	 * @brief Reduces a given angle, in radians, to a value between pi and negative pi.
	 *
	 * @param angle The angle to reduce.
	 * @return The wrapped angle.
	 */
	static float WrapAngle(float angle) noexcept;

	/**
	 * @brief Reduces a given angle, in radians, to a value between 180 and -180.
	 *
	 * @param angle The angle to reduce.
	 * @return The wrapped angle.
	 */
	static float WrapAngleDegrees(float angle) noexcept;
};