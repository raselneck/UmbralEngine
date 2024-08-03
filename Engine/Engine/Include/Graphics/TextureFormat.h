#pragma once

#include "Engine/IntTypes.h"
#include "Meta/MetaMacros.h"
#include "TextureFormat.Generated.h"

/**
 * Texture Format Name Meanings:
 *
 * Each texture format name (minus the suffix) defines which channel(s) texture data will take up
 * and be accessible as in the shader, as well as how large each channel's data is expected to be.
 *
 * For example:
 * - R8G8_UNORM would mean that the source data is expected to have 16 bits-per-pixel with one byte
 *   for the red channel and one byte for the green channel. In shaders, only the red and green
 *   channels would contain valid data.
 * - R32G32B32_FLOAT would mean that the source data is expected to have 96 bits-per-pixel with
 *   one 32-bit floating point number for each of the red, green, and blue channels. In shaders,
 *   the alpha channel would be the only channel to NOT contain valid data.
 *
 * Texture Format Name Suffix Meanings:
 * https://microsoft.github.io/DirectX-Specs/d3d/archive/D3D11_3_FunctionalSpec.htm#FormatModifiers
 *
 * _FLOAT    - Data in channels appearing to the left of _FLOAT in the format name
 *             are interpreted in the resource as floating point values (bit depth
 *             specified by format), and in the Shader as 32 bit floating point
 *             values, with appropriate conversions either way.
 * _UNORM    - Data in channels appearing on the left of _UNORM in the
 *             format name are interpreted in the resource as unsigned integers,
 *             and in the Shader as unsigned normalized float values,
 *             in the range [0,1].
 * _UINT     - Data in channels appearing to the left of _UINT in the
 *             format name are interpreted in the resource as unsigned integers,
 *             and also in the Shader as unsigned integers.
 * _SNORM    - Data in channels appearing on the left of _SNORM in the
 *             format name are interpreted in the resource as signed integers,
 *             and in the Shader as signed normalized float values
 *             in the range [-1,1].
 * _SINT     - Data in channels appearing to the left of _SINT in the
 *             format name are interpreted both in the resource and in the
 *             Shader as signed integers.
 */

/**
 * @brief An enumeration of possible texture formats.
 */
UM_ENUM()
enum class ETextureFormat : uint8
{
	R8_UNORM,
	R8_UINT,
	R8_SNORM,
	R8_SINT,

	R16_FLOAT,
	R16_UINT,
	R16_SINT,

	R32_FLOAT,
	R32_UINT,
	R32_SINT,

	R8G8_UNORM,
	R8G8_UINT,
	R8G8_SNORM,
	R8G8_SINT,

	R16G16_FLOAT,
	R16G16_UINT,
	R16G16_SINT,

	R32G32_FLOAT,
	R32G32_UINT,
	R32G32_SINT,

	R11G11B10_FLOAT,

	R32G32B32_FLOAT,
	R32G32B32_UINT,
	R32G32B32_SINT,

	R8G8B8A8_UNORM,
	R8G8B8A8_UINT,
	R8G8B8A8_SNORM,
	R8G8B8A8_SINT,

	R10G10B10A2_UNORM,
	R10G10B10A2_UINT,

	R16G16B16A16_FLOAT,
	R16G16B16A16_UINT,
	R16G16B16A16_SINT,

	R32G32B32A32_FLOAT,
	R32G32B32A32_UINT,
	R32G32B32A32_SINT,

	B5G6R5_UNORM,
	B5G5R5A1_UNORM,
	B8G8R8A8_UNORM,
	B4G4R4A4_UNORM,
};