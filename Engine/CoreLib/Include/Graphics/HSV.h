#pragma once

#include "Graphics/Color.h"
#include "Graphics/LinearColor.h"
#include "Math/Math.h"

/**
 * @brief Defines a color in terms of hue, saturation, and value.
 */
struct FColorHSV
{
	/** @brief This color's hue component, in the range [0, 360]. */
	float H = 0.0f;

	/** @brief This color's saturation component, in the range [0, 1]. */
	float S = 0.0f;

	/** @brief This color's value component, in the range [0, 1]. */
	float V = 0.0f;

	/** @brief This color's alpha component. */
	float A = 0.0f;

	/**
	 * @brief Sets default values for this HSV color's properties.
	 */
	constexpr FColorHSV() = default;

	/**
	 * @brief Sets default values for this HSV color's properties.
	 *
	 * @param r The initial hue value.
	 * @param g The initial saturation value.
	 * @param b The initial value value.
	 */
	constexpr FColorHSV(const float h, const float s, const float v)
		: H { h }
		, S { s }
		, V { v }
		, A { 1.0f }
	{
	}

	/**
	 * @brief Sets default values for this HSV color's properties.
	 *
	 * @param r The initial hue value.
	 * @param g The initial saturation value.
	 * @param b The initial value value.
	 * @param a The initial alpha value.
	 */
	constexpr FColorHSV(const float h, const float s, const float v, const float a)
		: H { h }
		, S { s }
		, V { v }
		, A { a }
	{
	}

	/**
	 * @brief Sets this HSV color's values based on the given linear color's.
	 *
	 * @param value The color.
	 */
	FColorHSV(const FLinearColor& value);

	/**
	 * @brief Converts this HSV color to a linear color.
	 *
	 * @return The converted linear color.
	 */
	[[nodiscard]] FLinearColor ToLinearColor() const;

	/**
	 * @brief Converts this HSV color to a color.
	 *
	 * @return The converted color.
	 */
	[[nodiscard]] FColor ToColor() const
	{
		return ToLinearColor().ToColor();
	}

	/**
	 * @brief Checks to see if this color is equivalent to another color.
	 *
	 * @param other The other color.
	 * @returns True if this color is equivalent to \p other, otherwise false.
	 */
	constexpr bool operator==(const FColorHSV& other) const
	{
		return FMath::IsNearlyEqual(H, other.H) &&
		       FMath::IsNearlyEqual(S, other.S) &&
		       FMath::IsNearlyEqual(V, other.V) &&
		       FMath::IsNearlyEqual(A, other.A);
	}

	/**
	 * @brief Checks to see if this color is not equivalent to another color.
	 *
	 * @param other The other color.
	 * @returns True if this color is not equivalent to \p other, otherwise false.
	 */
	constexpr bool operator!=(const FColorHSV& other) const
	{
		return this->operator==(other) == false;
	}

	template<typename StreamType>
	friend StreamType& operator<<(StreamType& stream, const FColorHSV& value)
	{
		return stream << "(H=" << value.H << ", S=" << value.S << ", V=" << value.V << ", A=" << value.A << ")";
	}
};