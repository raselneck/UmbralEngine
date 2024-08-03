#pragma once

#include "Containers/StringView.h"

/**
 * @brief Defines a blend mode.
 */
enum class EBlendMode
{
	/**
	 * @brief Each component of the color is multiplied by {1, 1, 1, 1}.
	 */
	One,

	/**
	 * @brief Each component of the color is multiplied by {0, 0, 0, 0}.
	 */
	Zero,

	/**
	 * @brief Each component of the color is multiplied by the source color.
	 *        {Rs, Gs, Bs, As}, where Rs, Gs, Bs, As are color source values.
	 */
	SourceColor,

	/**
	 * @brief Each component of the color is multiplied by the inverse of the source color.
	 *        {1 - Rs, 1 - Gs, 1 - Bs, 1 - As}, where Rs, Gs, Bs, As are color source values.
	 */
	InverseSourceColor,

	/**
	 * @brief Each component of the color is multiplied by the alpha value of the source.
	 *        {As, As, As, As}, where As is the source alpha value.
	 */
	SourceAlpha,

	/**
	 * @brief Each component of the color is multiplied by the inverse of the alpha value of the source.
	 *        {1 - As, 1 - As, 1 - As, 1 - As}, where As is the source alpha value.
	 */
	InverseSourceAlpha,

	/**
	 * @brief Each component color is multiplied by the destination color.
	 *        {Rd, Gd, Bd, Ad}, where Rd, Gd, Bd, Ad are color destination values.
	 */
	DestinationColor,

	/**
	 * @brief Each component of the color is multiplied by the inversed destination color.
	 *        {1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad}, where Rd, Gd, Bd, Ad are color destination values.
	 */
	InverseDestinationColor,

	/**
	 * @brief Each component of the color is multiplied by the alpha value of the destination.
	 *        {Ad, Ad, Ad, Ad}, where Ad is the destination alpha value.
	 */
	DestinationAlpha,

	/**
	 * @brief Each component of the color is multiplied by the inversed alpha value of the destination.
	 *        {1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad}, where Ad is the destination alpha value.
	 */
	InverseDestinationAlpha,

	/**
	 * @brief Each component of the color is multiplied by a constant in the graphics device's BlendFactor.
	 */
	BlendFactor,

	/**
	 * @brief Each component of the color is multiplied by a inversed constant in the graphics device's BlendFactor.
	 */
	InverseBlendFactor,

	/**
	 * @brief Each component of the color is multiplied by either the alpha of the source color, or the inverse of the alpha of the source color, whichever is greater.
	 *        {f, f, f, 1}, where f = min(As, 1 - As), where As is the source alpha value.
	 */
	SourceAlphaSaturation
};

/**
 * @brief Converts a blend mode value to a string.
 *
 * @param value The blend mode value.
 * @return The blend mode value as a string.
 */
constexpr FStringView ToString(const EBlendMode value)
{
#define RETURN_CASE_NAME(Name) case EBlendMode::Name: return UM_JOIN(#Name, _sv)
	switch (value)
	{
	RETURN_CASE_NAME(One);
	RETURN_CASE_NAME(Zero);
	RETURN_CASE_NAME(SourceColor);
	RETURN_CASE_NAME(InverseSourceColor);
	RETURN_CASE_NAME(SourceAlpha);
	RETURN_CASE_NAME(InverseSourceAlpha);
	RETURN_CASE_NAME(DestinationColor);
	RETURN_CASE_NAME(InverseDestinationColor);
	RETURN_CASE_NAME(DestinationAlpha);
	RETURN_CASE_NAME(InverseDestinationAlpha);
	RETURN_CASE_NAME(BlendFactor);
	RETURN_CASE_NAME(InverseBlendFactor);
	RETURN_CASE_NAME(SourceAlphaSaturation);
	default: return "<unknown>"_sv;
	}
#undef RETURN_CASE_NAME
}