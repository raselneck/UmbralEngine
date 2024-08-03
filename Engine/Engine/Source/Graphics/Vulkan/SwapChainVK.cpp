#include "Graphics/Vulkan/GraphicsDeviceVK.h"
#include "Graphics/Vulkan/SwapChainVK.h"

TObjectPtr<UGraphicsDeviceVK> USwapChainVK::GetGraphicsDevice() const
{
	return GetTypedParent<UGraphicsDeviceVK>();
}

TErrorOr<void> USwapChainVK::SetSwapInterval(const ESwapInterval swapInterval)
{
	(void)swapInterval;
	return {};
}

void USwapChainVK::SwapBuffers()
{
}