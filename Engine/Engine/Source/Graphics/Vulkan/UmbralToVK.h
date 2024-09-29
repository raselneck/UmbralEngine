#pragma once

#include "Containers/StaticArray.h"
#include "Containers/StringView.h"
#include "Graphics/VertexDeclaration.h"
#include "Engine/Assert.h"
#include "Misc/SourceLocation.h"
#include <vulkan/vulkan.h>

#if UMBRAL_DEBUG
#	define VK_CHECK(Call) \
		do                                                                                  \
		{                                                                                   \
			const VkResult resultOfVulkanCall= Call;                                        \
			if (resultOfVulkanCall != VK_SUCCESS)                                           \
			{                                                                               \
				::VK::ReportError(resultOfVulkanCall, UM_STRINGIFY_AS_VIEW(Call), UMBRAL_SOURCE_LOCATION); \
				UMBRAL_DEBUG_BREAK();                                                       \
			}                                                                               \
		} while (0)
#else
#	define VK_CHECK(Call) Call
#endif

namespace VK
{
	/**
	 * @brief Reports an error.
	 *
	 * @param error The error code.
	 * @param call The Vulkan API call.
	 * @param sourceLocation The source location of the call.
	 */
	void ReportError(VkResult error, FStringView call, FCppSourceLocation sourceLocation);
}