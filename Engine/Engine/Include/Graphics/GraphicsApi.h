#pragma once

#include "Meta/MetaMacros.h"
#include "GraphicsApi.Generated.h"

/**
 * @brief An enumeration of supported graphics APIs.
 */
UM_ENUM()
enum class EGraphicsApi : uint32
{
	OpenGL,
	Vulkan
};