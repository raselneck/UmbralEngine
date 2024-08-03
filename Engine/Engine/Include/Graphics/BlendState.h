#pragma once

#include "Engine/IntTypes.h"
#include "Graphics/BlendFunction.h"
#include "Graphics/BlendMode.h"
#include "Graphics/ColorWriteChannels.h"
#include "Graphics/CompareFunction.h"
#include "Graphics/LinearColor.h"

/**
 * @brief Defines a blend state for a graphics device.
 */
class FBlendState
{
public:

	/** @brief The arithmetic operation when blending alpha values. */
	EBlendFunction AlphaBlendFunction = EBlendFunction::Add;

	/** @brief The blend factor for the destination alpha, which is the percentage of the destination alpha included in the blended result. */
	EBlendMode AlphaDestinationBlend = EBlendMode::Zero;

	/** @brief The alpha blend factor. */
	EBlendMode AlphaSourceBlend = EBlendMode::One;

	/** @brief The four-component (RGBA) blend factor for alpha blending. */
	FLinearColor BlendFactor { 1.0f, 1.0f, 1.0f, 1.0f };

	/** @brief The arithmetic operation when blending color values. */
	EBlendFunction ColorBlendFunction = EBlendFunction::Add;

	/** @brief The blend factor for the destination color. */
	EBlendMode ColorDestinationBlend = EBlendMode::Zero;

	/** @brief The blend factor for the source color. */
	EBlendMode ColorSourceBlend = EBlendMode::One;

	/** @brief Which color channels (RGBA) are enabled for writing during color blending. */
	EColorWriteChannels ColorWriteMask0 = EColorWriteChannels::All;

	/** @brief Which color channels (RGBA) are enabled for writing during color blending. */
	EColorWriteChannels ColorWriteMask1 = EColorWriteChannels::All;

	/** @brief Which color channels (RGBA) are enabled for writing during color blending. */
	EColorWriteChannels ColorWriteMask2 = EColorWriteChannels::All;

	/** @brief Which color channels (RGBA) are enabled for writing during color blending. */
	EColorWriteChannels ColorWriteMask3 = EColorWriteChannels::All;

	/** @brief A bitmask which defines which samples can be written during multisampling. */
	int32 MultiSampleMask = -1;
};

namespace EBlendState
{
	/**
	 * @brief A blend state with settings for additive blend that is adding the destination data to the source data without using alpha.
	 */
	constexpr FBlendState Additive
	{
		.AlphaBlendFunction    = EBlendFunction::Add,
		.AlphaDestinationBlend = EBlendMode::One,
		.AlphaSourceBlend      = EBlendMode::SourceAlpha,
		.BlendFactor           = FLinearColor { 1.0f, 1.0f, 1.0f, 1.0f },
		.ColorBlendFunction    = EBlendFunction::Add,
		.ColorDestinationBlend = EBlendMode::One,
		.ColorSourceBlend      = EBlendMode::SourceAlpha,
		.ColorWriteMask0       = EColorWriteChannels::All,
		.ColorWriteMask1       = EColorWriteChannels::All,
		.ColorWriteMask2       = EColorWriteChannels::All,
		.ColorWriteMask3       = EColorWriteChannels::All,
		.MultiSampleMask       = -1
	};

	/**
	 * @brief A blend state with settings for alpha blend that is blending the source and destination data using alpha.
	 */
	constexpr FBlendState AlphaBlend
	{
		.AlphaBlendFunction    = EBlendFunction::Add,
		.AlphaDestinationBlend = EBlendMode::InverseSourceAlpha,
		.AlphaSourceBlend      = EBlendMode::One,
		.BlendFactor           = FLinearColor { 1.0f, 1.0f, 1.0f, 1.0f },
		.ColorBlendFunction    = EBlendFunction::Add,
		.ColorDestinationBlend = EBlendMode::InverseSourceAlpha,
		.ColorSourceBlend      = EBlendMode::One,
		.ColorWriteMask0       = EColorWriteChannels::All,
		.ColorWriteMask1       = EColorWriteChannels::All,
		.ColorWriteMask2       = EColorWriteChannels::All,
		.ColorWriteMask3       = EColorWriteChannels::All,
		.MultiSampleMask       = -1,
	};

	/**
	 * @brief A blend state with settings for blending with non-premultipled alpha that is blending source and destination data by using alpha while assuming the color data contains no alpha information.
	 */
	constexpr FBlendState NonPremultiplied
	{
		.AlphaBlendFunction    = EBlendFunction::Add,
		.AlphaDestinationBlend = EBlendMode::InverseSourceAlpha,
		.AlphaSourceBlend      = EBlendMode::SourceAlpha,
		.BlendFactor           = FLinearColor { 1.0f, 1.0f, 1.0f, 1.0f },
		.ColorBlendFunction    = EBlendFunction::Add,
		.ColorDestinationBlend = EBlendMode::InverseSourceAlpha,
		.ColorSourceBlend      = EBlendMode::SourceAlpha,
		.ColorWriteMask0       = EColorWriteChannels::All,
		.ColorWriteMask1       = EColorWriteChannels::All,
		.ColorWriteMask2       = EColorWriteChannels::All,
		.ColorWriteMask3       = EColorWriteChannels::All,
		.MultiSampleMask       = -1,
	};

	/**
	 * @brief A blend state with settings for opaque blend that is overwriting the source with the destination data.
	 */
	constexpr FBlendState Opaque
	{
		.AlphaBlendFunction    = EBlendFunction::Add,
		.AlphaDestinationBlend = EBlendMode::Zero,
		.AlphaSourceBlend      = EBlendMode::One,
		.BlendFactor           = FLinearColor { 1.0f, 1.0f, 1.0f, 1.0f },
		.ColorBlendFunction    = EBlendFunction::Add,
		.ColorDestinationBlend = EBlendMode::Zero,
		.ColorSourceBlend      = EBlendMode::One,
		.ColorWriteMask0       = EColorWriteChannels::All,
		.ColorWriteMask1       = EColorWriteChannels::All,
		.ColorWriteMask2       = EColorWriteChannels::All,
		.ColorWriteMask3       = EColorWriteChannels::All,
		.MultiSampleMask       = -1,
	};
}