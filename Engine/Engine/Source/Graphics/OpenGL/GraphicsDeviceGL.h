#pragma once

#include "Engine/EngineWindow.h"
#include "Graphics/GraphicsDevice.h"
#include "GraphicsDeviceGL.Generated.h"

class UEngineWindowSDL;
class UIndexBufferGL;
class UTextureManagerGL;
class UVertexBufferGL;
struct SDL_Window;

/**
 * @brief Defines an OpenGL-backed graphics device.
 */
UM_CLASS(ChildOf=UEngineWindowSDL)
class UGraphicsDeviceGL final : public UGraphicsDevice
{
	UM_GENERATED_BODY();

public:

	/** @copydoc UGraphicsDevice::BindIndexBuffer */
	virtual void BindIndexBuffer(TObjectPtr<const UIndexBuffer> indexBuffer) override;

	/** @copydoc UGraphicsDevice::BindVertexBuffer */
	virtual void BindVertexBuffer(TObjectPtr<const UVertexBuffer> vertexBuffer) override;

	/** @copydoc UGraphicsDevice::Clear */
	virtual void Clear(EClearOptions clearOptions, const FLinearColor& color, float depth, int32 stencil) override;

	/** @copydoc UGraphicsDevice::CreateIndexBuffer */
	[[nodiscard]] virtual TObjectPtr<UIndexBuffer> CreateIndexBuffer(EIndexBufferUsage usage) override;

	/** @copydoc UGraphicsDevice::CreateShader */
	[[nodiscard]] virtual TObjectPtr<UShader> CreateShader(EShaderType shaderType) override;

	/** @copydoc UGraphicsDevice::CreateShaderProgram */
	[[nodiscard]] virtual TObjectPtr<UShaderProgram> CreateShaderProgram() override;

	/** @copydoc UGraphicsDevice::CreateTexture2D */
	[[nodiscard]] virtual TObjectPtr<UTexture2D> CreateTexture2D() override;

	/** @copydoc UGraphicsDevice::CreateVertexBuffer */
	[[nodiscard]] virtual TObjectPtr<UVertexBuffer> CreateVertexBuffer(EVertexBufferUsage usage) override;

	/** @copydoc UGraphicsDevice::DrawIndexedVertices */
	virtual void DrawIndexedVertices(EPrimitiveType primitiveType) override;

	/** @copydoc UGraphicsDevice::DrawArrays */
	virtual void DrawVertices(EPrimitiveType primitiveType) override;

	/** @copydoc UGraphicsDevice::GetApi */
	[[nodiscard]] virtual EGraphicsApi GetApi() const override;

	/**
	 * @brief Gets the OpenGL context.
	 *
	 * @return The OpenGL context.
	 */
	[[nodiscard]] void* GetContext() const;

	/**
	 * @brief Gets this graphics device's texture manager.
	 *
	 * @return This graphics device's texture manager.
	 */
	[[nodiscard]] TObjectPtr<UTextureManagerGL> GetTextureManager() const
	{
		return m_TextureManager;
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

	/** @copydoc UGraphicsDevice::SetActiveContext */
	[[nodiscard]] virtual EGraphicsContextState SetActiveContext() const override;

	/** @copydoc UGraphicsDevice::UseShaderProgram */
	virtual void UseShaderProgram(TObjectPtr<UShaderProgram> shaderProgram) override;

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UObject::Destroyed */
	virtual void Destroyed() override;

private:

	UM_PROPERTY()
	TObjectPtr<UEngineWindowSDL> m_Window;

	UM_PROPERTY()
	TObjectPtr<UTextureManagerGL> m_TextureManager;

	UM_PROPERTY()
	TObjectPtr<const UIndexBufferGL> m_BoundIndexBuffer;

	UM_PROPERTY()
	TObjectPtr<const UVertexBufferGL> m_BoundVertexBuffer;

	void* m_Context = nullptr;

	FLinearColor m_ClearColor { 0.0f, 0.0f, 0.0f, 0.0f };
	float m_ClearDepth = 1.0f;
	int32 m_ClearStencil = 0;
	uint32 m_CurrentProgram = 0;
};