#pragma once

#include "ImGui/ImGuiRenderer.h"
#include "ImGuiRendererGL.Generated.h"

struct ImGuiViewport;
class UEngineWindowSDL;
class UGraphicsDeviceGL;
class UIndexBuffer;
class UShaderProgram;
class UVertexBuffer;
class UTexture2D;

/**
 * @brief Defines an OpenGL-backed ImGui renderer.
 */
UM_CLASS()
class UImGuiRendererGL : public UImGuiRenderer
{
	UM_GENERATED_BODY();

public:

	/** @copydoc UImGuiRenderer::Draw */
	virtual void Draw(const FGameTime& gameTime, const ImDrawData* drawData) override;

	/** @copydoc UImGuiRenderer::GetImGuiViewport */
	[[nodiscard]] virtual ImGuiViewport* GetImGuiViewport() const override;

	/** @copydoc UImGuiRenderer::NewFrame */
	virtual void NewFrame(const FGameTime& gameTime) override;

protected:

	/** @copydoc UImGuiRenderer::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UImGuiRenderer::Destroyed */
	virtual void Destroyed() override;

private:

	/**
	 * @brief Sets up the render state.
	 *
	 * @param drawData The draw data.
	 * @param framebufferWidth The framebuffer's width.
	 * @param framebufferHeight The framebuffer's height.
	 */
	void SetupRenderState(const ImDrawData* drawData, int32 framebufferWidth, int32 framebufferHeight);

	/**
	 * @brief Uploads ImGui's font atlas to the font texture.
	 */
	void UploadFontAtlasToTexture();

	UM_PROPERTY()
	TObjectPtr<UEngineWindowSDL> m_Window;

	UM_PROPERTY()
	TObjectPtr<UGraphicsDeviceGL> m_GraphicsDevice;

	UM_PROPERTY()
	TObjectPtr<UTexture2D> m_FontTexture;

	UM_PROPERTY()
	TObjectPtr<UShaderProgram> m_ShaderProgram;

	UM_PROPERTY()
	TObjectPtr<UVertexBuffer> m_VertexBuffer;

	UM_PROPERTY()
	TObjectPtr<UIndexBuffer> m_IndexBuffer;
};