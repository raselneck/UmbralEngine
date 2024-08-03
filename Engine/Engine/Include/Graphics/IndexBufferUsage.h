#pragma once

#include "Engine/IntTypes.h"
#include "Meta/MetaMacros.h"
#include "IndexBufferUsage.Generated.h"

/**
 * @brief An enumeration of possible index buffer usage types.
 */
UM_ENUM()
enum class EIndexBufferUsage : uint8
{
	/** @brief Denotes that the index buffer is invalid. */
	None,

	/** @brief The index buffer is optimized for having its data set once. */
	Static,

	/** @brief The index buffer is optimized for having its data set frequently. */
	Dynamic
};