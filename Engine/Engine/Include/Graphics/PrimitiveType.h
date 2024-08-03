#pragma once

#include "Engine/IntTypes.h"

/**
 * @brief An enumeration of possible primitive types.
 */
enum class EPrimitiveType : uint8
{
	PointList,
	LineStrip,
	LineLoop,
	LineList,
	TriangleStrip,
	TriangleFan,
	TriangleList
};