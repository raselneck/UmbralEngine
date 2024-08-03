#pragma once

#include "Graphics/CullMode.h"
#include "Graphics/FillMode.h"

/**
 * @brief Defines a rasterizer state for a graphics device, which describes how to convert vector data (shapes) into raster data (pixels).
 */
class FRasterizerState
{
public:

	/**
	 * @brief Specifies the conditions for culling or removing triangles.
	 */
	ECullMode CullMode = ECullMode::CullCounterClockwiseFace;

	/**
	 * @brief The depth bias for polygons, which is the amount of bias to apply to the depth of a
	 */
	float DepthBias = 0.0f;

	/**
	 * @brief The fill mode, which defines how a triangle is filled during rendering.
	 */
	EFillMode FillMode = EFillMode::Solid;

	/**
	 * @brief Whether or not to enable multisample antialiasing.
	 */
	bool MultiSampleAntiAlias = true;

	/**
	 * @brief Whether or not to enable scissor testing.
	 */
	bool ScissorTestEnable =  false;

	/**
	 * @brief A bias value that takes into account the slope of a polygon. This bias value is applied to coplanar primitives to reduce aliasing and other rendering artifacts caused by Z-fighting.
	 */
	float SlopeScaleDepthBias = 0.0f;
};

namespace ERasterizerState
{
	/**
	 * @brief A built-in rasterizer state with settings for culling primitives with clockwise winding order.
	 */
	constexpr FRasterizerState CullClockwise
	{
		.CullMode = ECullMode::CullClockwiseFace,
		.DepthBias = 0.0f,
		.FillMode = EFillMode::Solid,
		.MultiSampleAntiAlias = true,
		.ScissorTestEnable = false,
		.SlopeScaleDepthBias = 0.0f,
	};

	/**
	 * @brief A built-in rasterizer state with settings for culling primitives with counter-clockwise winding order.
	 */
	constexpr FRasterizerState CullCounterClockwise
	{
		.CullMode = ECullMode::CullCounterClockwiseFace,
		.DepthBias = 0.0f,
		.FillMode = EFillMode::Solid,
		.MultiSampleAntiAlias = true,
		.ScissorTestEnable = false,
		.SlopeScaleDepthBias = 0.0f,
	};

	/**
	 * @brief A built-in rasterizer state with settings for not culling any primitives.
	 */
	constexpr FRasterizerState CullNone
	{
		.CullMode = ECullMode::None,
		.DepthBias = 0.0f,
		.FillMode = EFillMode::Solid,
		.MultiSampleAntiAlias = true,
		.ScissorTestEnable = false,
		.SlopeScaleDepthBias = 0.0f,
	};

	/**
	 * @brief A built-in rasterizer state with settings for culling primitives with clockwise winding order while also rasterizing the shape in wireframe.
	 */
	constexpr FRasterizerState CullClockwiseWireframe
	{
		.CullMode = ECullMode::CullClockwiseFace,
		.DepthBias = 0.0f,
		.FillMode = EFillMode::WireFrame,
		.MultiSampleAntiAlias = true,
		.ScissorTestEnable = false,
		.SlopeScaleDepthBias = 0.0f,
	};

	/**
	 * @brief A built-in rasterizer state with settings for culling primitives with counter-clockwise winding order while also rasterizing the shape in wireframe.
	 */
	constexpr FRasterizerState CullCounterClockwiseWireframe
	{
		.CullMode = ECullMode::CullCounterClockwiseFace,
		.DepthBias = 0.0f,
		.FillMode = EFillMode::WireFrame,
		.MultiSampleAntiAlias = true,
		.ScissorTestEnable = false,
		.SlopeScaleDepthBias = 0.0f,
	};

	/**
	 * @brief A built-in rasterizer state with settings for not culling any primitives while also rasterizing the shape in wireframe.
	 */
	constexpr FRasterizerState CullNoneWireframe
	{
		.CullMode = ECullMode::None,
		.DepthBias = 0.0f,
		.FillMode = EFillMode::Solid,
		.MultiSampleAntiAlias = true,
		.ScissorTestEnable = false,
		.SlopeScaleDepthBias = 0.0f,
	};
}