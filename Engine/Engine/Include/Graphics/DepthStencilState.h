#pragma once

#include "Engine/IntTypes.h"
#include "Graphics/CompareFunction.h"
#include "Graphics/StencilOperation.h"
#include "Templates/NumericLimits.h"

/**
 * @brief Defines a depth-stencil state for a graphics device.
 */
class FDepthStencilState
{
public:

	/** @brief Whether or not to enable depth buffering. */
	bool DepthBufferEnable = true;

	/** @brief Whether or not to enable writing to the depth buffer. */
	bool DepthBufferWriteEnable = true;

	/** @brief The comparison function for the depth-buffer test. */
	ECompareFunction DepthBufferFunction = ECompareFunction::LessEqual;

	/** @brief Whether or not to enable stencil buffering. */
	bool StencilEnable = false;

	/** @brief The comparison function for the stencil test. */
	ECompareFunction StencilFunction = ECompareFunction::Always;

	/** @brief The stencil operation to perform if the stencil test passes. */
	EStencilOperation StencilPass = EStencilOperation::Keep;

	/** @brief The stencil operation to perform if the stencil test fails. */
	EStencilOperation StencilFail = EStencilOperation::Keep;

	/** @brief The stencil operation to perform if the stencil test passes and the depth-test fails. */
	EStencilOperation StencilDepthBufferFail = EStencilOperation::Keep;

	/** @brief Whether or not to enable two-sided stenciling. */
	bool TwoSidedStencilMode = false;

	/** @brief The comparison function to use for counterclockwise stencil tests. */
	ECompareFunction CounterClockwiseStencilFunction = ECompareFunction::Always;

	/** @brief The stencil operation to perform if the stencil and depth-tests pass for a counterclockwise triangle. */
	EStencilOperation CounterClockwiseStencilPass = EStencilOperation::Keep;

	/** @brief The stencil operation to perform if the stencil test fails for a counterclockwise triangle. */
	EStencilOperation CounterClockwiseStencilFail = EStencilOperation::Keep;

	/** @brief The stencil operation to perform if the stencil test passes and the depth-buffer test fails for a counterclockwise triangle. */
	EStencilOperation CounterClockwiseStencilDepthBufferFail = EStencilOperation::Keep;

	/** @brief The mask applied to the reference value and each stencil buffer entry to determine the significant bits for the stencil test. */
	int32 StencilMask = TNumericLimits<int32>::MaxValue;

	/** @brief The write mask applied to values written into the stencil buffer. */
	int32 StencilWriteMask = TNumericLimits<int32>::MaxValue;

	/** @brief The reference value to use for the stencil test. */
	int32 ReferenceStencil = 0;
};

namespace EDepthStencilState
{
	/** @brief A built-in depth-stencil state with default settings for using a depth-stencil buffer. */
	constexpr FDepthStencilState Default
	{
		.DepthBufferEnable = true,
		.DepthBufferWriteEnable = true,
		.DepthBufferFunction = ECompareFunction::LessEqual,
		.StencilEnable = false,
		.StencilFunction = ECompareFunction::Always,
		.StencilPass = EStencilOperation::Keep,
		.StencilFail = EStencilOperation::Keep,
		.StencilDepthBufferFail = EStencilOperation::Keep,
		.TwoSidedStencilMode = false,
		.CounterClockwiseStencilFunction = ECompareFunction::Always,
		.CounterClockwiseStencilPass = EStencilOperation::Keep,
		.CounterClockwiseStencilFail = EStencilOperation::Keep,
		.CounterClockwiseStencilDepthBufferFail = EStencilOperation::Keep,
		.StencilMask = TNumericLimits<int32>::MaxValue,
		.StencilWriteMask = TNumericLimits<int32>::MaxValue,
		.ReferenceStencil = 0
	};

	/** @brief A built-in depth-stencil state with settings for enabling a read-only depth-stencil buffer. */
	constexpr FDepthStencilState DepthRead
	{
		.DepthBufferEnable = true,
		.DepthBufferWriteEnable = false,
		.DepthBufferFunction = ECompareFunction::LessEqual,
		.StencilEnable = false,
		.StencilFunction = ECompareFunction::Always,
		.StencilPass = EStencilOperation::Keep,
		.StencilFail = EStencilOperation::Keep,
		.StencilDepthBufferFail = EStencilOperation::Keep,
		.TwoSidedStencilMode = false,
		.CounterClockwiseStencilFunction = ECompareFunction::Always,
		.CounterClockwiseStencilPass = EStencilOperation::Keep,
		.CounterClockwiseStencilFail = EStencilOperation::Keep,
		.CounterClockwiseStencilDepthBufferFail = EStencilOperation::Keep,
		.StencilMask = TNumericLimits<int32>::MaxValue,
		.StencilWriteMask = TNumericLimits<int32>::MaxValue,
		.ReferenceStencil = 0,
	};

	/** @brief A built-in depth-stencil state with settings for not using a depth-stencil buffer. */
	constexpr FDepthStencilState None
	{
		.DepthBufferEnable = false,
		.DepthBufferWriteEnable = false,
		.DepthBufferFunction = ECompareFunction::LessEqual,
		.StencilEnable = false,
		.StencilFunction = ECompareFunction::Always,
		.StencilPass = EStencilOperation::Keep,
		.StencilFail = EStencilOperation::Keep,
		.StencilDepthBufferFail = EStencilOperation::Keep,
		.TwoSidedStencilMode = false,
		.CounterClockwiseStencilFunction = ECompareFunction::Always,
		.CounterClockwiseStencilPass = EStencilOperation::Keep,
		.CounterClockwiseStencilFail = EStencilOperation::Keep,
		.CounterClockwiseStencilDepthBufferFail = EStencilOperation::Keep,
		.StencilMask = TNumericLimits<int32>::MaxValue,
		.StencilWriteMask = TNumericLimits<int32>::MaxValue,
		.ReferenceStencil = 0,
	};
}