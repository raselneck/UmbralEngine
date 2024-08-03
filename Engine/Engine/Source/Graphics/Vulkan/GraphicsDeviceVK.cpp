#include "Containers/Optional.h"
#include "Containers/StaticArray.h"
#include "Engine/Assert.h"
#include "Engine/Logging.h"
#include "Engine/ModuleManager.h"
#include "Engine/SDL/EngineWindowSDL.h"
#include "Graphics/Vulkan/GraphicsDeviceVK.h"
#include "Graphics/Vulkan/ShaderVK.h"
#include "Graphics/Vulkan/UmbralToVK.h"
#include "Misc/CString.h"
#include "Misc/Version.h"
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>
#if UMBRAL_PLATFORM_IS_WINDOWS
#include <vulkan/vulkan_win32.h>
#elif UMBRAL_PLATFORM_IS_MAC
#include <vulkan/vulkan_macos.h>
#include <vulkan/vulkan_metal.h>
#elif UMBRAL_PLATFORM_IS_IOS
#include <vulkan/vulkan_ios.h>
#include <vulkan/vulkan_metal.h>
#elif UMBRAL_PLATFORM_IS_ANDROID
#include <vulkan/vulkan_android.h>
#endif

#ifndef WITH_VK_INSTANCE_DEBUGGING
#define WITH_VK_INSTANCE_DEBUGGING 0
#endif

#ifndef WITH_SDL_VULKAN_CREATE_SURFACE
#define WITH_SDL_VULKAN_CREATE_SURFACE 1
#endif

/**
 * @brief Defines a set of queue family indices for Vulkan.
 */
class FQueueFamilyIndices
{
public:

	/** @brief The graphics queue family index. */
	TOptional<uint32> GraphicsFamily;

	/** @brief The presentation queue family index. */
	TOptional<uint32> PresentFamily;

	/** @brief The compute queue family index. */
	TOptional<uint32> ComputeFamily;

	/**
	 * @brief Checks to see if all queue family indices have been set.
	 *
	 * @return True if all queue family indices have been set, otherwise false.
	 */
	[[nodiscard]] bool IsComplete() const
	{
		return GraphicsFamily.HasValue() && PresentFamily.HasValue() && ComputeFamily.HasValue();
	}
};

/**
 * @brief Defines swap chain support details for a Vulkan physical device.
 */
class FSwapChainSupportDetails
{
public:

	/** @brief The surface capabilities. */
	VkSurfaceCapabilitiesKHR Capabilities {};

	/** @brief The supported surface formats. */
	TArray<VkSurfaceFormatKHR> Formats;

	/** @brief The supported present modes. */
	TArray<VkPresentModeKHR> PresentModes;
};

/**
 * @brief Defines the results of creating a swap chain.
 */
class FCreateSwapChainResult
{
public:

	/** @brief The swap chain. */
	VkSwapchainKHR SwapChain = VK_NULL_HANDLE;

	/** @brief The swap image format. */
	VkFormat ImageFormat = VK_FORMAT_UNDEFINED;

	/** @brief The swap extent. */
	VkExtent2D Extent {};

	/** @brief The swap chain's images. */
	TArray<VkImage> SwapChainImages;

	/** @brief The swap chain's image views. */
	TArray<VkImageView> SwapChainImageViews;
};

/**
 * @brief Defines the results of creating a graphics pipeline.
 */
class FCreateGraphicsPipelineResult
{
public:
};

namespace VK
{
	void* Alloc(void* userData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
	{
		// TODO Need to add aligned allocation support

		(void)userData;
		(void)alignment;
		(void)allocationScope;

		return FMemory::Allocate(static_cast<FMemory::SizeType>(size));
	}

	void Free(void* userData, void* memory)
	{
		(void)userData;

		FMemory::Free(memory);
	}

	void* Realloc(void* userData, void* original, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
	{
		(void)userData;
		(void)allocationScope;

		return FMemory::ReallocateAligned(original, static_cast<FMemory::SizeType>(size), static_cast<FMemory::SizeType>(alignment));
	}

	void OnAllocInternal(void* userData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
	{
		(void)userData;
		(void)size;
		(void)allocationType;
		(void)allocationScope;
	}

	void OnFreeInternal(void* userData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
	{
		(void)userData;
		(void)size;
		(void)allocationType;
		(void)allocationScope;
	}

	TArray<VkExtensionProperties> GetAvailableInstanceExtensions()
	{
		uint32 extensionCount = 0;
		VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));

		TArray<VkExtensionProperties> extensions = MakeArrayWithDefaultElements<VkExtensionProperties>(static_cast<int32>(extensionCount));
		VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.GetData()));

		return extensions;
	};

	TArray<VkLayerProperties> GetAvailableInstanceLayers()
	{
		uint32 layerCount = 0;
		VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

		TArray<VkLayerProperties> layers = MakeArrayWithDefaultElements<VkLayerProperties>(static_cast<int32>(layerCount));
		VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, layers.GetData()));

		return layers;
	};

	template<typename FunctionType>
	FunctionType GetInstanceProcAddr(VkInstance instance, const char* pName)
	{
		return reinterpret_cast<FunctionType>(vkGetInstanceProcAddr(instance, pName));
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
#if UMBRAL_DEBUG
		PFN_vkCreateDebugUtilsMessengerEXT createFunction = GetInstanceProcAddr<PFN_vkCreateDebugUtilsMessengerEXT>(instance, "vkCreateDebugUtilsMessengerEXT");
		if (createFunction == nullptr)
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		return createFunction(instance, pCreateInfo, pAllocator, pDebugMessenger);
#else
		(void)instance;
		(void)pCreateInfo;
		(void)pAllocator;

		*pDebugMessenger = VK_NULL_HANDLE;
		return VK_SUCCESS;
#endif
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
#if UMBRAL_DEBUG
		PFN_vkDestroyDebugUtilsMessengerEXT destroyFunction = GetInstanceProcAddr<PFN_vkDestroyDebugUtilsMessengerEXT>(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (destroyFunction != nullptr)
		{
			destroyFunction(instance, debugMessenger, pAllocator);
		}
#else
		(void)instance;
		(void)debugMessenger;
		(void)pAllocator;
#endif
	}

#if UMBRAL_DEBUG
	static constexpr FStringView GetDebugMessageTypeTag(const VkDebugUtilsMessageTypeFlagsEXT messageType)
	{
		switch (messageType)
		{
		case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:       return "[generic    ]"_sv;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:    return "[validation ]"_sv;
		case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:   return "[performance]"_sv;
		default:                                                return "[unknown    ]"_sv;
		}
	}

	static constexpr ELogLevel GetDebugMessageLogLevel(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:     return ELogLevel::Error;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:   return ELogLevel::Warning;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:   return ELogLevel::Verbose;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		default:                                                return ELogLevel::Info;
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL OnDebugMessage(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
	                                              const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	                                              void* userData)
	{
		(void)userData;

		const ELogLevel logLevel = GetDebugMessageLogLevel(messageSeverity);
		const FStringView typeTag = GetDebugMessageTypeTag(messageType);
		const FStringView debugMessage { callbackData->pMessage };
		const FString logMessage = FString::Format("Vulkan: {} {}"_sv, typeTag, debugMessage);
		::Private::FLogger::GetInstance().Write(logLevel, logMessage.AsStringView());

		return VK_FALSE;
	}
#endif

	constexpr FStringView GetDeviceTypeName(const VkPhysicalDeviceType deviceType)
	{
		switch (deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:    return "integrated GPU"_sv;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:      return "discrete GPU"_sv;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:       return "virtual GPU"_sv;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:               return "CPU"_sv;
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
		default:										return "other"_sv;
		}
	}

	TArray<VkExtensionProperties> GetAvailableDeviceExtensions(VkPhysicalDevice physicalDevice)
	{
		uint32 extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

		TArray<VkExtensionProperties> availableExtensions = MakeArrayWithDefaultElements<VkExtensionProperties>(static_cast<int32>(extensionCount));
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.GetData());

		return availableExtensions;
	}
}

static constexpr FVersion GUmbralVersion
{
	UMBRAL_VERSION_MAJOR,
	UMBRAL_VERSION_MINOR,
	UMBRAL_VERSION_PATCH
};

static constexpr FVersion GUmbralVulkanVersion
{
	VK_API_VERSION_MAJOR(VK_HEADER_VERSION_COMPLETE),
	VK_API_VERSION_MINOR(VK_HEADER_VERSION_COMPLETE),
	VK_API_VERSION_PATCH(VK_HEADER_VERSION_COMPLETE)
};

static VkAllocationCallbacks GVulkanAllocator
{
	.pUserData = nullptr,
	.pfnAllocation = VK::Alloc,
	.pfnReallocation = VK::Realloc,
	.pfnFree = VK::Free,
	.pfnInternalAllocation = VK::OnAllocInternal,
	.pfnInternalFree = VK::OnFreeInternal
};

/**
 * @brief Converts an Umbral version to a Vulkan version value.
 *
 * @param version The Umbral version.
 * @return The Vulkan version.
 */
static constexpr uint32 UmbralVersionToVulkan(const FVersion& version)
{
	return VK_MAKE_VERSION(version.GetMajor(), version.GetMinor(), version.GetPatch());
}

/**
 * @brief Converts a Vulkan version to an Umbral version value.
 *
 * @param version The Vulkan version.
 * @return The Umbral version.
 */
static constexpr FVersion VulkanVersionToUmbral(const uint32 version)
{
	return FVersion
	{
		VK_VERSION_MAJOR(version),
		VK_VERSION_MINOR(version),
		VK_VERSION_PATCH(version)
	};
}

/**
 * @brief Populates the given debug messenger create info.
 *
 * @param createInfo The debug messenger create info.
 */
static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = VK::OnDebugMessage;
	createInfo.pUserData = nullptr;
}

/**
 * @brief Gets an array of the required Vulkan extensions.
 *
 * @param window The window being used.
 * @return The required Vulkan validation layers.
 */
static TArray<const char*> GetRequiredVulkanInstanceExtensions(SDL_Window* window)
{
	uint32 extensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);

	TArray<const char*> extensions = MakeArrayWithDefaultElements<const char*>(static_cast<int32>(extensionCount));
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensions.GetData());

	extensions.Add(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

#if UMBRAL_DEBUG
	extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	//extensions.Add(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

	return extensions;
}

/**
 * @brief Gets an array of the required Vulkan validation layers.
 *
 * @return The required Vulkan validation layers.
 */
static TArray<const char*> GetRequiredVulkanInstanceValidationLayers()
{
	return TArray<const char*>
	{
#if UMBRAL_DEBUG
		"VK_LAYER_KHRONOS_validation"
#endif
	};
}

/**
 * @brief Creates a Vulkan instance representing the current Umbral module.
 *
 * @param window The window that the instance will be tied to.
 * @return A Vulkan instance representing the current Umbral module.
 */
static VkInstance CreateVulkanInstance(SDL_Window* window)
{
	IModule* currentModule = FModuleManager::GetCurrentModule();

	VkApplicationInfo appInfo {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = FModuleManager::GetCurrentModuleName().GetChars();
	appInfo.applicationVersion = UmbralVersionToVulkan(currentModule->GetModuleVersion());
	appInfo.pEngineName = "Umbral Engine";
	appInfo.engineVersion = UmbralVersionToVulkan(GUmbralVersion);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	const TArray<VkExtensionProperties> availableExtensions = VK::GetAvailableInstanceExtensions();

	const auto IsExtensionAvailable = [&availableExtensions](const FStringView extensionName)
	{
		for (const VkExtensionProperties& availableExtension : availableExtensions)
		{
			const FStringView availableExtensionName { availableExtension.extensionName };
			if (extensionName == availableExtensionName)
			{
				return true;
			}
		}
		return false;
	};

	const TArray<const char*> requiredExtensions = GetRequiredVulkanInstanceExtensions(window);

	// Ensure all required extensions are available
	for (const char* extension : requiredExtensions)
	{
		if (IsExtensionAvailable(FStringView { extension }))
		{
			continue;
		}

		UM_LOG(Error, "Vulkan extension {} is not available", extension);
		UM_ASSERT_NOT_REACHED();
	}

	const TArray<VkLayerProperties> availableLayers = VK::GetAvailableInstanceLayers();

	const auto IsLayerAvailable = [&availableLayers](const FStringView layerName)
	{
		for (const VkLayerProperties& availableLayer : availableLayers)
		{
			const FStringView availableLayerName { availableLayer.layerName };
			if (layerName == availableLayerName)
			{
				return true;
			}
		}
		return false;
	};

	const TArray<const char*> requiredLayers = GetRequiredVulkanInstanceValidationLayers();

	// Ensure all required layers are available
	for (const char* layer : requiredLayers)
	{
		if (IsLayerAvailable(FStringView { layer }))
		{
			continue;
		}

		UM_LOG(Error, "Vulkan layer {} is not available", layer);
		UM_ASSERT_NOT_REACHED();
	}

	VkInstanceCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32>(requiredExtensions.Num());
	createInfo.ppEnabledExtensionNames = requiredExtensions.GetData();
	createInfo.enabledLayerCount = static_cast<uint32>(requiredLayers.Num());
	createInfo.ppEnabledLayerNames = requiredLayers.GetData();
	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

#if UMBRAL_DEBUG
#if WITH_VK_INSTANCE_DEBUGGING
	VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreatInfo;
	PopulateDebugMessengerCreateInfo(debugMessengerCreatInfo);

	createInfo.pNext = &debugMessengerCreatInfo;
#endif

	uint32 instanceVersionAsInt = 0;
	VK_CHECK(vkEnumerateInstanceVersion(&instanceVersionAsInt));

	const FVersion instanceVersion = VulkanVersionToUmbral(instanceVersionAsInt);
	UM_LOG(Info, "Instance version: {}", instanceVersion.AsString(EVersionStringFlags::IncludePatch));

	UM_LOG(Info, "Required instance extensions:");
	for (const char* requiredExtension : requiredExtensions)
	{
		UM_LOG(Info, "-- {}", requiredExtension);
	}

	UM_LOG(Info, "Available instance extensions:");
	for (const VkExtensionProperties& availableExtension : availableExtensions)
	{
		UM_LOG(Info, "-- {}", availableExtension.extensionName);
	}

	UM_LOG(Info, "Required instance layers:");
	for (const char* requiredLayer : requiredLayers)
	{
		UM_LOG(Info, "-- {}", requiredLayer);
	}

	UM_LOG(Info, "Available instance layers:");
	for (const VkLayerProperties& availableLayer : availableLayers)
	{
		UM_LOG(Info, "-- {}", availableLayer.layerName);
	}
#endif

	VkInstance instance = VK_NULL_HANDLE;
	VK_CHECK(vkCreateInstance(&createInfo, &GVulkanAllocator, &instance));

	return instance;
}

/**
 * @brief Creates a Vulkan surface for a window.
 *
 * @param instance The current instance.
 * @param window The window.
 * @return The Vulkan surface.
 */
static VkSurfaceKHR CreateVulkanSurfaceForWindow(VkInstance instance, SDL_Window* window)
{
#if WITH_SDL_VULKAN_CREATE_SURFACE
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	if (SDL_Vulkan_CreateSurface(window, instance, &surface) == SDL_FALSE)
	{
		UM_LOG(Assert, "Failed to create Vulkan surface. Reason: {}", SDL_GetError());
	}

	return surface;
#else
#error Not sure how we are supposed to create the Vulkan surface
#endif
}

/**
 * @brief Creates a Vulkan debug messenger.
 *
 * @param instance The Vulkan instance to attach to.
 * @return A Vulkan debug messenger.
 */
static VkDebugUtilsMessengerEXT CreateVulkanDebugMessenger(VkInstance instance)
{
#if UMBRAL_DEBUG
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
	VK_CHECK(VK::CreateDebugUtilsMessengerEXT(instance, &createInfo, &GVulkanAllocator, &debugMessenger));

	return debugMessenger;
#else
	(void)instance;
	return VK_NULL_HANDLE;
#endif
}

/**
 * @brief Finds the queue family indices for a given physical device.
 *
 * @param surface The current Vulkan rendering surface.
 * @param device The physical device.
 * @return The device's queue family indices.
 */
static FQueueFamilyIndices FindVulkanPhysicalDeviceQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice device)
{
	uint32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	TArray<VkQueueFamilyProperties> queueFamilies = MakeArrayWithDefaultElements<VkQueueFamilyProperties>(static_cast<int32>(queueFamilyCount));
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.GetData());

	FQueueFamilyIndices result;
	for (int32 idx = 0; idx < queueFamilies.Num(); ++idx)
	{
		const VkQueueFamilyProperties& properties = queueFamilies[idx];

		if (result.GraphicsFamily.IsEmpty() && HasFlag(properties.queueFlags, VK_QUEUE_GRAPHICS_BIT))
		{
			result.GraphicsFamily = static_cast<uint32>(idx);
		}

		VkBool32 presentSupport = false;
		VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, static_cast<uint32>(idx), surface, &presentSupport));
		if (presentSupport)
		{
			result.PresentFamily = static_cast<uint32>(idx);
		}

		if (result.ComputeFamily.IsEmpty() && HasFlag(properties.queueFlags, VK_QUEUE_COMPUTE_BIT))
		{
			result.ComputeFamily = static_cast<uint32>(idx);
		}
	}
	return result;
}

/**
 * @brief Queries a Vulkan physical device for its swap chain support details.
 *
 * @param surface The current Vulkan rendering surface.
 * @param device The physical device.
 * @return The swap chain support details.
 */
static FSwapChainSupportDetails QueryVulkanSwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device)
{
	FSwapChainSupportDetails result;

	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &result.Capabilities));

	uint32 formatCount = 0;
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr));

	if (formatCount > 0)
	{
		result.Formats.AddZeroed(static_cast<int32>(formatCount));
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, result.Formats.GetData()));
	}

	uint32 presentModeCount = 0;
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr));

	if (presentModeCount > 0)
	{
		result.PresentModes.AddZeroed(static_cast<int32>(presentModeCount));
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, result.PresentModes.GetData()));
	}

	return result;
}

/**
 * @brief Selects a physical device on the system that is suitable to use.
 *
 * @remarks The device's queue families are guaranteed to be complete.
 *
 * @param instance The current Vulkan instance.
 * @param surface The current Vulkan rendering surface.
 * @return A suitable physical device.
 */
static VkPhysicalDevice SelectVulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
	uint32 deviceCount = 0;
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));

	UM_ASSERT(deviceCount > 0, "Failed to find any devices to use with Vulkan");

	TArray<VkPhysicalDevice> devices = MakeArrayWithDefaultElements<VkPhysicalDevice>(static_cast<int32>(deviceCount));
	VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.GetData()));

	UM_LOG(Info, "Found {} physical devices:", devices.Num());
	devices.Iterate([](VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties {};
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		const FStringView deviceName { deviceProperties.deviceName };
		UM_LOG(Info, "-- {} ({})", deviceName, VK::GetDeviceTypeName(deviceProperties.deviceType));

		return EIterationDecision::Continue;
	});

	// TODO If a more precise suitability calculation is necessary, check sample code here:
	// https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Physical_devices_and_queue_families

	const auto IsDeviceSuitable = [surface](VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties {};
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures {};
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		const FStringView deviceName { deviceProperties.deviceName };

		// The device must support the swap chain extension
		TArray<VkExtensionProperties> deviceExtensions = VK::GetAvailableDeviceExtensions(device);
		const bool swapChainSupported = deviceExtensions.ContainsByPredicate([](const VkExtensionProperties& extension)
		{
			constexpr FStringView swapChainExtensionName { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			const FStringView extensionName { extension.extensionName };
			return extensionName == swapChainExtensionName;
		});

		if (swapChainSupported == false)
		{
			UM_LOG(Warning, "{} does not support swap chains", deviceName);
			return false;
		}

		// The swap chain support must also be adequate
		const FSwapChainSupportDetails swapChainSupport = QueryVulkanSwapChainSupport(surface, device);
		if (swapChainSupport.Formats.IsEmpty())
		{
			UM_LOG(Warning, "{} has no swap surface formats", deviceName);
			return false;
		}
		if (swapChainSupport.PresentModes.IsEmpty())
		{
			UM_LOG(Warning, "{} has no swap present modes", deviceName);
			return false;
		}

		// TODO Need to use the above better suitability calculation because we want to prefer discrete over integrated
		if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		    deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			UM_LOG(Warning, "{} is not a discrete or integrated GPU", deviceName);
			return false;
		}

		if (deviceFeatures.tessellationShader == false)
		{
			UM_LOG(Warning, "{} does not support tessellation shaders", deviceName);
			return false;
		}

		FQueueFamilyIndices deviceQueueFamilies = FindVulkanPhysicalDeviceQueueFamilies(surface, device);
		return deviceQueueFamilies.IsComplete();
	};

	VkPhysicalDevice* suitableDevice = devices.FindByPredicate(IsDeviceSuitable);
	UM_ASSERT(suitableDevice != nullptr, "Failed to find a suitable device to use with Vulkan");

	return *suitableDevice;
}

/**
 * @brief Creates a logical Vulkan device from a physical Vulkan device.
 *
 * @param surface The current Vulkan rendering surface.
 * @param physicalDevice The physical device.
 * @return The logical device.
 */
static VkDevice CreateVulkanLogicalDevice(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice)
{
	const FQueueFamilyIndices queueFamilies = FindVulkanPhysicalDeviceQueueFamilies(surface, physicalDevice);
	const THashTable<uint32> uniqueQueueFamilies
	{{
		queueFamilies.GraphicsFamily.GetValue(),
		queueFamilies.PresentFamily.GetValue()
	}};

	float queuePriority = 1.0f;
	TArray<VkDeviceQueueCreateInfo> queueCreateInfos;
	//for (const uint32 queueFamily : uniqueQueueFamilies) // TODO Fix bug with this
	for (auto iter = uniqueQueueFamilies.CreateIterator(); iter; ++iter)
	{
		const uint32 queueFamily = *iter;

		VkDeviceQueueCreateInfo& queueCreateInfo = queueCreateInfos.AddZeroedGetRef();
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
	}

	VkPhysicalDeviceFeatures deviceFeatures {};

	VkDeviceCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.GetData();
	createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.Num());
	createInfo.pEnabledFeatures = &deviceFeatures;

	const TArray<const char*> deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	createInfo.enabledExtensionCount = static_cast<uint32>(deviceExtensions.Num());
	createInfo.ppEnabledExtensionNames = deviceExtensions.GetData();

#if UMBRAL_DEBUG
	const TArray<const char*> requiredLayers = GetRequiredVulkanInstanceValidationLayers();

	createInfo.enabledLayerCount = static_cast<uint32>(requiredLayers.Num());
	createInfo.ppEnabledLayerNames = requiredLayers.GetData();
#else
	createInfo.enabledLayerCount = 0;
#endif

	VkDevice device = VK_NULL_HANDLE;
	VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, &GVulkanAllocator, &device));

	return device;
}

/**
 * @brief Chooses the most preferred swap chain surface format from the given collection of formats.
 *
 * @param availableFormats The collection of available formats.
 * @return The most preferred swap chain surface format from the given collection.
 */
static VkSurfaceFormatKHR ChooseVulkanSwapSurfaceFormat(const TSpan<const VkSurfaceFormatKHR> availableFormats)
{
	UM_ASSERT(availableFormats.Num() > 0, "Cannot choose swap chain format when none are available");

	for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
		    availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

/**
 * @brief Chooses the most preferred swap chain present mode from the given collection of present modes.
 *
 * @param availablePresentModes The collection of available present modes.
 * @return The most preferred swap chain present mode from the given collection.
 */
static VkPresentModeKHR ChooseVulkanSwapPresentMode(const TSpan<const VkPresentModeKHR> availablePresentModes)
{
	/**
	 * VK_PRESENT_MODE_IMMEDIATE_KHR:
	 *     Images submitted by your application are transferred to the screen right away, which may result in tearing.
	 *
	 * VK_PRESENT_MODE_FIFO_KHR:
	 *     The swap chain is a queue where the display takes an image from the front of the queue when the display is
	 *     refreshed and the program inserts rendered images at the back of the queue. If the queue is full then the
	 *     program has to wait. This is most similar to vertical sync as found in modern games. The moment that the
	 *     display is refreshed is known as "vertical blank".
	 *
	 * VK_PRESENT_MODE_FIFO_RELAXED_KHR:
	 *     This mode only differs from the previous one if the application is late and the queue was empty at the
	 *     last vertical blank. Instead of waiting for the next vertical blank, the image is transferred right away
	 *     when it finally arrives. This may result in visible tearing.
	 *
	 * VK_PRESENT_MODE_MAILBOX_KHR:
	 *     This is another variation of the second mode. Instead of blocking the application when the queue is full,
	 *     the images that are already queued are simply replaced with the newer ones. This mode can be used to render
	 *     frames as fast as possible while still avoiding tearing, resulting in fewer latency issues than standard
	 *     vertical sync. This is commonly known as "triple buffering", although the existence of three buffers alone
	 *     does not necessarily mean that the framerate is unlocked.
	 */

	constexpr VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	if (availablePresentModes.Contains(preferredPresentMode))
	{
		return preferredPresentMode;
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

/**
 * @brief Chooses the preferred swap extent from the given surface capabilities.
 *
 * @param capabilities The surface capabilities.
 * @param window The window being rendered to.
 * @return The preferred swap extent to use.
 */
static VkExtent2D ChooseVulkanSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, SDL_Window* window)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}

	int32 width = 0, height = 0;
	SDL_Vulkan_GetDrawableSize(window, &width, &height);

	VkExtent2D actualExtent { static_cast<uint32>(width), static_cast<uint32>(height) };
	actualExtent.width = FMath::Clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	actualExtent.height = FMath::Clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

	return actualExtent;
}

/**
 * @brief Creates a Vulkan swap chain.
 *
 * @param window The window the swap chain is being created for.
 * @param surface The surface the swap chain is being created for.
 * @param physicalDevice The physical device being used for rendering.
 * @param device The logical device being used for rendering.
 * @return The swap chain.
 */
static FCreateSwapChainResult CreateVulkanSwapChain(SDL_Window* window, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice device)
{
	const FSwapChainSupportDetails swapChainSupport = QueryVulkanSwapChainSupport(surface, physicalDevice);

	const VkSurfaceFormatKHR surfaceFormat = ChooseVulkanSwapSurfaceFormat(swapChainSupport.Formats.AsSpan());
	const VkPresentModeKHR presentMode = ChooseVulkanSwapPresentMode(swapChainSupport.PresentModes.AsSpan());
	const VkExtent2D extent = ChooseVulkanSwapExtent(swapChainSupport.Capabilities, window);

	// We say "min image count plus one" here to get around potentially needing to wait for the driver to complete internal operations
	uint32 imageCount = swapChainSupport.Capabilities.minImageCount + 1;
	if (swapChainSupport.Capabilities.maxImageCount > 0 &&
	    imageCount > swapChainSupport.Capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.Capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	const FQueueFamilyIndices indices = FindVulkanPhysicalDeviceQueueFamilies(surface, physicalDevice);
	TStaticArray<uint32, 2> queueFamilyIndices {{ indices.GraphicsFamily.GetValue(), indices.PresentFamily.GetValue() }};

	/**
	 * VK_SHARING_MODE_EXCLUSIVE:
	 *     An image is owned by one queue family at a time and ownership must be explicitly transferred before using it
	 *     in another queue family. This option offers the best performance.
	 *
	 * VK_SHARING_MODE_CONCURRENT:
	 *     Images can be used across multiple queue families without explicit ownership transfers.
	 */

	if (queueFamilyIndices[0] != queueFamilyIndices[1])
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices.GetData();
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	FCreateSwapChainResult result;
	VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, &GVulkanAllocator, &result.SwapChain));

	result.ImageFormat = surfaceFormat.format;
	result.Extent = extent;

	uint32 swapChainImageCount = 0;
	vkGetSwapchainImagesKHR(device, result.SwapChain, &swapChainImageCount, nullptr);

	result.SwapChainImages.AddZeroed(static_cast<int32>(swapChainImageCount));
	result.SwapChainImageViews.AddZeroed(static_cast<int32>(swapChainImageCount));
	vkGetSwapchainImagesKHR(device, result.SwapChain, &swapChainImageCount, result.SwapChainImages.GetData());

	for (int32 idx = 0; idx < static_cast<int32>(swapChainImageCount); ++idx)
	{
		VkImageViewCreateInfo viewCreateInfo {};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = result.SwapChainImages[idx];
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = result.ImageFormat;
		viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewCreateInfo.subresourceRange.baseMipLevel = 0;
		viewCreateInfo.subresourceRange.levelCount = 1;
		viewCreateInfo.subresourceRange.baseArrayLayer = 0;
		viewCreateInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(device, &viewCreateInfo, &GVulkanAllocator, &result.SwapChainImageViews[idx]));
	}

	return result;
}

static FCreateGraphicsPipelineResult CreateVulkanGraphicsPipeline()
{
	// https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Introduction

	return {};
}

void UGraphicsDeviceVK::Clear(const EClearOptions clearOptions, const FLinearColor& color, const float depth, const int32 stencil)
{
	(void)clearOptions;
	(void)color;
	(void)depth;
	(void)stencil;
}

TObjectPtr<UIndexBuffer> UGraphicsDeviceVK::CreateIndexBuffer(const EIndexBufferUsage usage)
{
	(void)usage;
	return nullptr;
}

TObjectPtr<UShader> UGraphicsDeviceVK::CreateShader(const EShaderType shaderType)
{
	FObjectCreationContext context;
	context.SetParameter("type"_sv, shaderType);

	return MakeObject<UShaderVK>(this, nullptr, context);
}

TObjectPtr<UShaderProgram> UGraphicsDeviceVK::CreateShaderProgram()
{
	return nullptr;
}

TObjectPtr<UVertexBuffer> UGraphicsDeviceVK::CreateVertexBuffer(const EVertexBufferUsage usage)
{
	(void)usage;
	return nullptr;
}

EGraphicsApi UGraphicsDeviceVK::GetApi() const
{
	return EGraphicsApi::Vulkan;
}

TObjectPtr<UEngineWindow> UGraphicsDeviceVK::GetWindow() const
{
	return m_Window;
}

SDL_Window* UGraphicsDeviceVK::GetWindowHandle() const
{
	return m_Window->GetWindowHandle();
}

void UGraphicsDeviceVK::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	UM_LOG(Info, "Creating Vulkan {} graphics device", GUmbralVulkanVersion.AsString());

	m_Window = GetTypedParent<UEngineWindowSDL>();
	UM_ENSURE(m_Window.IsValid());

	m_Instance = CreateVulkanInstance(m_Window->GetWindowHandle());
	m_Surface = CreateVulkanSurfaceForWindow(m_Instance, m_Window->GetWindowHandle());
	m_DebugMessenger = CreateVulkanDebugMessenger(m_Instance);
	m_PhysicalDevice = SelectVulkanPhysicalDevice(m_Instance, m_Surface);
	m_GraphicsDevice = CreateVulkanLogicalDevice(m_Surface, m_PhysicalDevice);

	const FQueueFamilyIndices queueIndices = FindVulkanPhysicalDeviceQueueFamilies(m_Surface, m_PhysicalDevice);
	vkGetDeviceQueue(m_GraphicsDevice, queueIndices.GraphicsFamily.GetValue(), 0, &m_GraphicsQueue);
	vkGetDeviceQueue(m_GraphicsDevice, queueIndices.PresentFamily.GetValue(), 0, &m_PresentQueue);

	FCreateSwapChainResult swapChainCreateResult = CreateVulkanSwapChain(m_Window->GetWindowHandle(), m_Surface, m_PhysicalDevice, m_GraphicsDevice);
	m_SwapChain = swapChainCreateResult.SwapChain;
	m_SwapChainImages = MoveTemp(swapChainCreateResult.SwapChainImages);
	m_SwapChainImageViews = MoveTemp(swapChainCreateResult.SwapChainImageViews);
	m_SwapChainImageFormat = swapChainCreateResult.ImageFormat;
	m_SwapChainExtent = swapChainCreateResult.Extent;

	CreateVulkanGraphicsPipeline();
}

void UGraphicsDeviceVK::Destroyed()
{
	for (VkImageView& swapChainImageView : m_SwapChainImageViews)
	{
		vkDestroyImageView(m_GraphicsDevice, swapChainImageView, &GVulkanAllocator);
		swapChainImageView = VK_NULL_HANDLE;
	}

	m_SwapChainImages.Reset();
	m_SwapChainImageViews.Reset();
	m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
	m_SwapChainExtent = {};

	if (m_SwapChain != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(m_GraphicsDevice, m_SwapChain, &GVulkanAllocator);
		m_SwapChain = VK_NULL_HANDLE;
	}

	m_PresentQueue = VK_NULL_HANDLE;
	m_GraphicsQueue = VK_NULL_HANDLE;

	if (m_GraphicsDevice != VK_NULL_HANDLE)
	{
		vkDestroyDevice(m_GraphicsDevice, &GVulkanAllocator);
		m_GraphicsDevice = VK_NULL_HANDLE;
	}

	if (m_DebugMessenger != VK_NULL_HANDLE)
	{
		VK::DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, &GVulkanAllocator);
		m_DebugMessenger = VK_NULL_HANDLE;
	}

	if (m_Surface != VK_NULL_HANDLE)
	{
#if WITH_SDL_VULKAN_CREATE_SURFACE
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
#else
		vkDestroySurfaceKHR(m_Instance, m_Surface, &GVulkanAllocator);
#endif
		m_Surface = VK_NULL_HANDLE;
	}

	if (m_Instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(m_Instance, &GVulkanAllocator);
		m_Instance = VK_NULL_HANDLE;
	}

	Super::Destroyed();
}