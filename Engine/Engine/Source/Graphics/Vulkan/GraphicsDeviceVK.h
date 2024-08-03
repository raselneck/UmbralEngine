#pragma once

#include "Containers/Array.h"
#include "Engine/EngineWindow.h"
#include "Graphics/GraphicsDevice.h"
#include <vulkan/vulkan.h>
#include "GraphicsDeviceVK.Generated.h"

class UEngineWindowSDL;
struct SDL_Window;

/**
 * @brief Defines an OpenGL-backed graphics device.
 */
UM_CLASS(ChildOf=UEngineWindowSDL)
class UGraphicsDeviceVK final : public UGraphicsDevice
{
	UM_GENERATED_BODY();

public:

	/** @copydoc UGraphicsDevice::Clear */
	virtual void Clear(EClearOptions clearOptions, const FLinearColor& color, float depth, int32 stencil) override;

	/** @copydoc UGraphicsDevice::CreateIndexBuffer */
	[[nodiscard]] virtual TObjectPtr<UIndexBuffer> CreateIndexBuffer(EIndexBufferUsage usage) override;

	/** @copydoc UGraphicsDevice::CreateShader */
	[[nodiscard]] virtual TObjectPtr<UShader> CreateShader(EShaderType shaderType) override;

	/** @copydoc UGraphicsDevice::CreateShaderProgram */
	[[nodiscard]] virtual TObjectPtr<UShaderProgram> CreateShaderProgram() override;

	/** @copydoc UGraphicsDevice::CreateVertexBuffer */
	[[nodiscard]] virtual TObjectPtr<UVertexBuffer> CreateVertexBuffer(EVertexBufferUsage usage) override;

	/** @copydoc UGraphicsDevice::GetApi */
	[[nodiscard]] virtual EGraphicsApi GetApi() const override;

	/**
	 * @brief Gets this graphics device's physical device.
	 *
	 * @return This graphics device's physical device.
	 */
	[[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	/**
	 * @brief Gets this graphics device's swap chain.
	 *
	 * @return This graphics device's swap chain.
	 */
	[[nodiscard]] VkSwapchainKHR GetSwapChain() const
	{
		return m_SwapChain;
	}

	/**
	 * @brief Gets the associated window.
	 *
	 * @return The associated window.
	 */
	[[nodiscard]] TObjectPtr<UEngineWindow> GetWindow() const;

	/**
	 * @brief Gets the associated window's handle.
	 *
	 * @return The associated window's handle.
	 */
	[[nodiscard]] SDL_Window* GetWindowHandle() const;

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UObject::Destroyed */
	virtual void Destroyed() override;

private:

	UM_PROPERTY()
	TObjectPtr<UEngineWindowSDL> m_Window;

	VkInstance m_Instance = VK_NULL_HANDLE;
	VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	VkDevice m_GraphicsDevice = VK_NULL_HANDLE;
	VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
	VkQueue m_PresentQueue = VK_NULL_HANDLE;
	VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
	TArray<VkImage> m_SwapChainImages;
	TArray<VkImageView> m_SwapChainImageViews;
	VkFormat m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D m_SwapChainExtent {};
};