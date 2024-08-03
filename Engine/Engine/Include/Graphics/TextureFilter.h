#pragma once

#include "Engine/IntTypes.h"
#include "Meta/MetaMacros.h"
#include "TextureFilter.Generated.h"

/**
 * @brief Defines filtering types during texture sampling.
 */
UM_ENUM()
enum class ETextureFilter : uint8
{
	/**
	 * @brief Use linear filtering.
	 */
	Linear,
	/**
	 * @brief Use point filtering.
	 */
	Point,
	/**
	 * @brief Use anisotropic filtering.
	 */
	Anisotropic,
	/**
	 * @brief Use linear filtering to shrink or expand, and point filtering between mipmap levels (mip).
	 */
	LinearMipPoint,
	/**
	 * @brief Use point filtering to shrink (minify) or expand (magnify), and linear filtering between mipmap levels.
	 */
	PointMipLinear,
	/**
	 * @brief Use linear filtering to shrink, point filtering to expand, and linear filtering between mipmap levels.
	 */
	MinLinearMagPointMipLinear,
	/**
	 * @brief Use linear filtering to shrink, point filtering to expand, and point filtering between mipmap levels.
	 */
	MinLinearMagPointMipPoint,
	/**
	 * @brief Use point filtering to shrink, linear filtering to expand, and linear filtering between mipmap levels.
	 */
	MinPointMagLinearMipLinear,
	/**
	 * @brief Use point filtering to shrink, linear filtering to expand, and point filtering between mipmap levels.
	 */
	MinPointMagLinearMipPoint,
};