#pragma once

#include "Engine/GameViewport.h"
#include "BocksViewport.Generated.h"

class UIndexBuffer;
class UShaderProgram;
class UTexture2D;
class UVertexBuffer;

/**
 * @brief Defines the viewport used by Bocks.
 */
UM_CLASS()
class UBocksViewport : public UGameViewport
{
	UM_GENERATED_BODY();

public:

	/** @copydoc UEngine::Draw */
	virtual void Draw(const FGameTime& gameTime) override;

	/** @copydoc UEngine::Update */
	virtual void Update(const FGameTime& gameTime) override;

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

private:

	/**
	 * @brief Initialize the vertex and index buffers for a cube using position-color vertices.
	 *
	 * @param graphicsDevice The graphics device.
	 */
	TErrorOr<void> InitializePositionColorCube(const TObjectPtr<UGraphicsDevice>& graphicsDevice);

	/**
	 * @brief Initialize the vertex and index buffers for a cube using position-texture vertices.
	 *
	 * @param graphicsDevice The graphics device.
	 */
	TErrorOr<void> InitializePositionTextureCube(const TObjectPtr<UGraphicsDevice>& graphicsDevice);

	/**
	 * @brief Initialize the shader program to draw meshes using position-color vertices.
	 *
	 * @param graphicsDevice The graphics device.
	 */
	TErrorOr<void> InitializePositionColorShaderProgram(const TObjectPtr<UGraphicsDevice>& graphicsDevice);

	/**
	 * @brief Initialize the shader program to draw meshes using position-texture vertices.
	 *
	 * @param graphicsDevice The graphics device.
	 */
	TErrorOr<void> InitializePositionTextureShaderProgram(const TObjectPtr<UGraphicsDevice>& graphicsDevice);

	UM_PROPERTY()
	TObjectPtr<UShaderProgram> m_Program;

	UM_PROPERTY()
	TObjectPtr<UVertexBuffer> m_VertexBuffer;

	UM_PROPERTY()
	TObjectPtr<UIndexBuffer> m_IndexBuffer;

	UM_PROPERTY()
	TObjectPtr<UTexture2D> m_Texture;

	bool m_ShowDemoWindow = true;
};