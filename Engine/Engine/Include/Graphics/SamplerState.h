#pragma once

#include "Graphics/TextureAddressMode.h"
#include "Graphics/TextureFilter.h"

/**
 * @brief Defines a sampler state, which determines how to sample texture data.
 */
class FSamplerState
{
public:

	/** @brief The texture-address mode for the U-coordinate. */
	ETextureAddressMode AddressU = ETextureAddressMode::Wrap;

	/** @brief The texture-address mode for the V-coordinate. */
	ETextureAddressMode AddressV = ETextureAddressMode::Wrap;

	/** @brief The texture-address mode for the W-coordinate. */
	ETextureAddressMode AddressW = ETextureAddressMode::Wrap;

	/**
	 * @brief The type of filtering during sampling.
	 */
	ETextureFilter Filter = ETextureFilter::Linear;
};

namespace ESamplerState
{
	/**
	 * @brief Sampling with anisotropic filtering and clamped address modes.
	 */
	constexpr FSamplerState AnisotropicClamp
	{
		.AddressU = ETextureAddressMode::Clamp,
		.AddressV = ETextureAddressMode::Clamp,
		.AddressW = ETextureAddressMode::Clamp,
		.Filter = ETextureFilter::Anisotropic
	};

	/**
	 * @brief Sampling with anisotropic filtering and wrapped address modes.
	 */
	constexpr FSamplerState AnisotropicWrap
	{
		.AddressU = ETextureAddressMode::Wrap,
		.AddressV = ETextureAddressMode::Wrap,
		.AddressW = ETextureAddressMode::Wrap,
		.Filter = ETextureFilter::Anisotropic
	};

	/**
	 * @brief Sampling with linear filtering and clamped address modes.
	 */
	constexpr FSamplerState LinearClamp
	{
		.AddressU = ETextureAddressMode::Clamp,
		.AddressV = ETextureAddressMode::Clamp,
		.AddressW = ETextureAddressMode::Clamp,
		.Filter = ETextureFilter::Linear
	};

	/**
	 * @brief Sampling with linear filtering and wrapped address modes.
	 */
	constexpr FSamplerState LinearWrap
	{
		.AddressU = ETextureAddressMode::Wrap,
		.AddressV = ETextureAddressMode::Wrap,
		.AddressW = ETextureAddressMode::Wrap,
		.Filter = ETextureFilter::Linear
	};

	/**
	 * @brief Sampling with point filtering and clamped address modes.
	 */
	constexpr FSamplerState PointClamp
	{
		.AddressU = ETextureAddressMode::Clamp,
		.AddressV = ETextureAddressMode::Clamp,
		.AddressW = ETextureAddressMode::Clamp,
		.Filter = ETextureFilter::Point
	};

	/**
	 * @brief Sampling with point filtering and wrapped address modes.
	 */
	constexpr FSamplerState PointWrap
	{
		.AddressU = ETextureAddressMode::Wrap,
		.AddressV = ETextureAddressMode::Wrap,
		.AddressW = ETextureAddressMode::Wrap,
		.Filter = ETextureFilter::Point
	};
}