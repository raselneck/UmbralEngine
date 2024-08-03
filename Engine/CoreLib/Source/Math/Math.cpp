#include "Math/Math.h"
#include <cmath>

// Do not include SSE headers on ARM builds
#if !UMBRAL_ARCH_IS_ARM
#include <immintrin.h> /* For _mm_load_ss */
#include <emmintrin.h> /* For _mm_sqrt_pd */
#include <xmmintrin.h> /* For _mm_sqrt_ss */
#endif

/**
 * Wraps a float value to be within the given range.
 * @param value The value to wrap.
 * @param minValue The minimum value in the range.
 * @param maxValue The maximum value in the range.
 * @return The wrapped value.
 */
static float WrapFloatValue(float value, const float minValue, const float maxValue) noexcept
{
	if ((value > minValue) && (value <= maxValue))
	{
		return value;
	}

	const float valueRange = maxValue - minValue;
	value = ::fmodf(value, valueRange);

	if (value <= minValue)
	{
		return value + valueRange;
	}
	else if (value > maxValue)
	{
		return value - valueRange;
	}
	else
	{
		return value;
	}
}

float FMath::Barycentric(const float value1,
                         const float value2,
                         const float value3,
                         const float amount1,
                         const float amount2) noexcept
{
	// FIXME: This can be made constexpr in the header
	return value1 + (value2 - value1) * amount1 + (value3 - value1) * amount2;
}

float FMath::CatmullRom(const float value1,
                        const float value2,
                        const float value3,
                        const float value4,
                        const float amount) noexcept
{
	// Using formula from http://www.mvps.org/directx/articles/catmull/
	// Internally using doubles not to lose precision

	const double amountSquared = amount * amount;
	const double amountCubed = amountSquared * amount;
	const double result = (2.0 * value2 + (value3 - value1) * amount * 0.5 +
	                      (2.0 * value1 - 5.0 * value2 + 4.0 * value3 - value4) * amountSquared +
	                      (3.0 * value2 - value1 - 3.0 * value3 + value4) * amountCubed);
	return static_cast<float>(result);
}

double FMath::Cos(double radians)
{
	return ::cos(radians);
}

float FMath::Cos(float radians)
{
	return ::cosf(radians);
}

float FMath::Hermite(const float value1,
                     const float tangent1,
                     const float value2,
                     const float tangent2,
                     const float amount) noexcept
{
	if (IsNearlyEqual(amount, 0.0f))
	{
		return value1;
	}
	else if (IsNearlyEqual(amount, 1.0f))
	{
		return value2;
	}

	// All transformed to double not to lose precision
	// Otherwise, for high numbers of amount the result is NaN instead of Infinity

	const double v1 = value1;
	const double t1 = tangent1;
	const double v2 = value2;
	const double t2 = tangent2;
	const double s = amount;
	const double sSquared = s * s;
	const double sCubed = sSquared * s;

	const double result = (2.0 * v1 - 2.0 * v2 + t2 + t1) * sCubed +
	                      (3.0 * v2 - 3.0 * v1 - 2 * t1 - t2) * sSquared +
	                      (t1 * s) + v1;
	return static_cast<float>(result);
}

float FMath::SmoothStep(const float from, const float to, const float amount) noexcept
{
	// It is expected that 0 < amount < 1
	// If amount < 0, return from
	// If amount > 1, return to
	float result = FMath::Clamp(amount, 0.0f, 1.0f);
	result = FMath::Hermite(from, 0.0f, to, 0.0f, result);

	return result;
}

float FMath::Sqrt(const float value) noexcept
{
#if UMBRAL_ARCH_IS_ARM
	return ::sqrtf(value);
#else
	__m128 packedValue = _mm_load_ss(&value);
	packedValue = _mm_sqrt_ss(packedValue);

	float result = 0.0f;
	_mm_store_ss(&result, packedValue);

	return result;
#endif
}

double FMath::Sqrt(const double value) noexcept
{
#if UMBRAL_ARCH_IS_ARM
	return ::sqrt(value);
#else
	__m128d packedValue = _mm_load_sd(&value);
	packedValue = _mm_sqrt_pd(packedValue); // FIXME: Is this the function we really want?

	double result = 0.0;
	_mm_store_sd(&result, packedValue);

	return result;
#endif
}

double FMath::Sin(double radians)
{
	return ::sin(radians);
}

float FMath::Sin(float radians)
{
	return ::sinf(radians);
}

float FMath::WrapAngle(const float angle) noexcept
{
	return WrapFloatValue(angle, -FMath::Pi, FMath::Pi);
}

float FMath::WrapAngleDegrees(const float angle) noexcept
{
	return WrapFloatValue(angle, -180.0f, 180.0f);
}