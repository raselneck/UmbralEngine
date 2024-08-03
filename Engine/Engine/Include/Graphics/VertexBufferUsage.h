#pragma once

#include "Engine/IntTypes.h"
#include "Meta/MetaMacros.h"
#include "VertexBufferUsage.Generated.h"

/**
 * @brief An enumeration of possible vertex buffer usage types.
 */
UM_ENUM()
enum class EVertexBufferUsage : uint8
{
	/** @brief Denotes that the vertex buffer is invalid. */
	None,

	/** @brief The vertex buffer is optimized for having its data set once. */
	Static,

	/** @brief The vertex buffer is optimized for having its data set frequently. */
	Dynamic
};