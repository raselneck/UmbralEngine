#include "Graphics/Vulkan/GraphicsDeviceVK.h"
#include "Graphics/Vulkan/ShaderVK.h"

void UShaderVK::Created(const FObjectCreationContext& context)
{
	Super::Created(context);
}

void UShaderVK::Destroyed()
{
	Super::Destroyed();
}

TObjectPtr<UGraphicsDeviceVK> UShaderVK::GetGraphicsDevice() const
{
	return FindAncestorOfType<UGraphicsDeviceVK>();
}