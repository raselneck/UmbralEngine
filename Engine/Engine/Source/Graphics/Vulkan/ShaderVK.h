#pragma once

#include "Graphics/Shader.h"
#include <vulkan/vulkan.h>
#include "ShaderVK.Generated.h"

class UGraphicsDeviceVK;

/**
 * @brief Defines a Vulkan-backed shader.
 */
UM_CLASS(Abstract, ChildOf=UGraphicsDeviceVK)
class UShaderVK : public UShader
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets this shader's shader module.
	 *
	 * @return This shader's shader module.
	 */
	[[nodiscard]] VkShaderModule GetShaderModule() const
	{
		return m_ShaderModule;
	}

protected:

	/** @copydoc UShaderGL::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UShaderGL::Destroyed */
	virtual void Destroyed() override;

	/**
	 * @brief Gets the graphics device this shader is associated with.
	 *
	 * @return The graphics device this shader is associated with.
	 */
	TObjectPtr<UGraphicsDeviceVK> GetGraphicsDevice() const;

private:

	VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
};