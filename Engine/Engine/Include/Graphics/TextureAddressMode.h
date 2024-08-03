#pragma once

#include "Engine/IntTypes.h"

/**
 * @brief Defines modes for addressing texels using texture coordinates that are outside of the typical range of 0.0 to 1.0.
 */
enum class ETextureAddressMode : uint8
{
	/**
	 * @brief Tile the texture at every integer junction. For example, for U values between 0 and 3, the texture is repeated three times; no mirroring is performed.
	 */
	Wrap,

	/**
	 * @brief Texture coordinates outside the range [0.0, 1.0] are set to the texture color at 0.0 or 1.0, respectively.
	 */
	Clamp,

	/**
	 * @brief Similar to Wrap, except that the texture is flipped at every integer junction. For U values between 0 and 1,
	 *        for example, the texture is addressed normally; between 1 and 2, the texture is flipped (mirrored);
	 *        between 2 and 3, the texture is normal again, and so on.
	 */
	Mirror
};