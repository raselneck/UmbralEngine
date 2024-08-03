#include "Graphics/HSV.h"
#include "Graphics/Color.h"
#include "Graphics/LinearColor.h"
#include "Math/Math.h"
#include <cmath>

struct FMinMaxRgbInfo
{
	float MaxValue = 0.0f;
	float MinValue = 0.0f;
	bool IsMaxR = false;
	bool IsMaxG = false;
	bool IsMaxB = false;
};

/**
 * @brief Calculates min and max information from a set of RGB values.
 *
 * @param r The red value.
 * @param g The green value.
 * @param b The blue value.
 * @param info The output min-max info.
 */
static void CalculateMinMaxFromRgb(const float r, const float g, const float b, FMinMaxRgbInfo &info)
{
	// Check for max value
	if (r >= g && r >= b)
	{
		info.MaxValue = r;
		info.IsMaxR = true;
	}
	else if (g >= r && g >= b)
	{
		info.MaxValue = g;
		info.IsMaxG = true;
	}
	else
	{
		info.MaxValue = b;
		info.IsMaxB = true;
	}

	info.MinValue = FMath::Min(r, FMath::Min(g, b));
}

/**
 * @brief Checks to see if the given value is within a range.
 *
 * @param value The value to check.
 * @param minValue The range minimum.
 * @param maxValue The range maximum.
 * @return True if \p value is within the specified range, otherwise false.
 */
static constexpr bool IsInRange(const float value, const float minValue, const float maxValue)
{
	return (value >= minValue) && (value <= maxValue);
}

FColorHSV::FColorHSV(const FLinearColor& value)
	: A { value.A }
{
	// Algorithm from https://en.wikipedia.org/wiki/HSL_and_HSV#From_RGB

	FMinMaxRgbInfo info;
	CalculateMinMaxFromRgb(value.R, value.G, value.B, info);

	const float range = info.MaxValue - info.MinValue;
	const float inverseRange = range <= 0.001f ? 0.0f : 1.0f / range;

	// Calculate hue
	if (info.MaxValue == info.MinValue)
	{
		H = 0.0f;
	}
	else if (info.IsMaxR)
	{
		H = 60.0f * (0 + (value.G - value.B) * inverseRange);
	}
	else if (info.IsMaxG)
	{
		H = 60.0f * (2 + (value.B - value.R) * inverseRange);
	}
	else if (info.IsMaxB)
	{
		H = 60.0f * (4 + (value.R - value.G) * inverseRange);
	}

	if (H < 0.0f) // Ensure hue is [0, 360]
	{
		H += 360.0f;
	}

	// Calculate saturation
	if (info.MaxValue <= 0.001f)
	{
		S = 0.0f;
	}
	else
	{
		S = range / info.MaxValue;
	}

	// Calculate value
	V = info.MaxValue;
}

FLinearColor FColorHSV::ToLinearColor() const
{
	// Algorithm from https://en.wikipedia.org/wiki/HSL_and_HSV#HSV_to_RGB

	constexpr float inverseSixtyDegrees = 1.0f / 60.0f;

	const float c = V * S;
	const float hp = H * inverseSixtyDegrees;
	const float x = c * (1.0f - FMath::Abs(::fmodf(hp, 2.0f) - 1.0f));

	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;

	if (IsInRange(hp, 0, 1))
	{
		r = c;
		g = x;
	}
	else if (IsInRange(hp, 1, 2))
	{
		r = x;
		g = c;
	}
	else if (IsInRange(hp, 2, 3))
	{
		g = c;
		b = x;
	}
	else if (IsInRange(hp, 3, 4))
	{
		g = x;
		b = c;
	}
	else if (IsInRange(hp, 4, 5))
	{
		r = x;
		b = c;
	}
	else if (IsInRange(hp, 5, 6))
	{
		r = c;
		b = x;
	}

	const float m = V - c;
	return FLinearColor { r + m, g + m, b + m, A };
}