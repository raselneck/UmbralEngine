#pragma once

#include "Engine/IntTypes.h"
#include "Meta/MetaMacros.h"
#include "ShaderType.Generated.h"

/**
 * @brief An enumeration of possible shader types.
 */
UM_ENUM()
enum class EShaderType : uint8
{
	/** @brief Denotes that the shader is invalid. */
	None,

	/** @brief A vertex shader. */
	Vertex,

	/** @brief A fragment / pixel shader. */
	Fragment,

	/** @brief A compute shader. */
	Compute
};