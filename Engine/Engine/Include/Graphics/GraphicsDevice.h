#pragma once

#include "Graphics/ClearOptions.h"
#include "Graphics/Color.h"
#include "Graphics/GraphicsApi.h"
#include "Graphics/GraphicsContextState.h"
#include "Graphics/IndexBufferUsage.h"
#include "Graphics/LinearColor.h"
#include "Graphics/PrimitiveType.h"
#include "Graphics/ShaderType.h"
#include "Graphics/VertexBufferUsage.h"
#include "Object/Object.h"
#include "GraphicsDevice.Generated.h"

class FImage;
class UEngineWindow;
class UIndexBuffer;
class UShader;
class UShaderProgram;
class UTexture2D;
class UVertexBuffer;

/**
 * @brief Defines the base for all graphics devices.
 */
UM_CLASS(Abstract, ChildOf=UEngineWindow)
class UGraphicsDevice : public UObject
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Binds the given index buffer.
	 *
	 * @param indexBuffer The index buffer.
	 */
	virtual void BindIndexBuffer(TObjectPtr<const UIndexBuffer> indexBuffer);

	/**
	 * @brief Binds the given vertex buffer.
	 *
	 * @param vertexBuffer The vertex buffer.
	 */
	virtual void BindVertexBuffer(TObjectPtr<const UVertexBuffer> vertexBuffer);

	/**
	 * @brief Clears the currently bound color buffer.
	 *
	 * @param color The color to set the color buffer to.
	 */
	void Clear(const FLinearColor& color)
	{
		Clear(EClearOptions::Color, color, 1.0f, 0);
	}

	/**
	 * @brief Clears the currently bound color and depth buffers.
	 *
	 * @param color The color to set the color buffer to.
	 * @param depth The depth value to set the depth buffer to.
	 */
	void Clear(const FLinearColor& color, const float depth)
	{
		Clear(EClearOptions::Color | EClearOptions::Depth, color, depth, 0);
	}

	/**
	 * @brief Clears the currently bound color, depth, and stencil buffers.
	 *
	 * @param color The color to set the color buffer to.
	 * @param depth The depth value to set the depth buffer to.
	 * @param stencil The stencil value to set the stencil buffer to.
	 */
	void Clear(const FLinearColor& color, const float depth, const int32 stencil)
	{
		Clear(EClearOptions::All, color, depth, stencil);
	}

	/**
	 * @brief Clears the currently bound buffers.
	 *
	 * @param clearOptions The clear options.
	 * @param color The color to set the buffers to.
	 * @param depth The depth value to set the buffers to.
	 * @param stencil The stencil value to set the buffers to.
	 */
	virtual void Clear(EClearOptions clearOptions, const FLinearColor& color, float depth, int32 stencil);

	/**
	 * @brief Creates an index buffer.
	 *
	 * @param usage The index buffer usage.
	 * @return The index buffer.
	 */
	[[nodiscard]] virtual TObjectPtr<UIndexBuffer> CreateIndexBuffer(EIndexBufferUsage usage);

	/**
	 * @brief Creates a shader.
	 *
	 * @param shaderType The shader type.
	 * @return The shader.
	 */
	[[nodiscard]] virtual TObjectPtr<UShader> CreateShader(EShaderType shaderType);

	/**
	 * @brief Creates a shader program.
	 *
	 * @return The shader program.
	 */
	[[nodiscard]] virtual TObjectPtr<UShaderProgram> CreateShaderProgram();

	/**
	 * @brief Creates a 2D texture.
	 *
	 * @return The 2D texture.
	 */
	[[nodiscard]] virtual TObjectPtr<UTexture2D> CreateTexture2D();

	/**
	 * @brief Creates a vertex buffer.
	 *
	 * @param usage The vertex buffer usage.
	 * @return The vertex buffer.
	 */
	[[nodiscard]] virtual TObjectPtr<UVertexBuffer> CreateVertexBuffer(EVertexBufferUsage usage);

	/**
	 * @brief Draws the currently bound vertex buffer(s) using the currently bound index buffer.
	 *
	 * @param primitiveType The primitive type being drawn.
	 */
	virtual void DrawIndexedVertices(EPrimitiveType primitiveType);

	/**
	 * @brief Draws the currently bound vertex buffer(s).
	 *
	 * @param primitiveType The primitive type being drawn.
	 */
	virtual void DrawVertices(EPrimitiveType primitiveType);

	/**
	 * @brief Gets the graphics API that this graphics device uses.
	 *
	 * @return This graphics device's graphics API.
	 */
	[[nodiscard]] virtual EGraphicsApi GetApi() const;

	/**
	 * @brief Sets this graphics device's rendering context as the current one for the calling thread.
	 */
	[[nodiscard]] virtual EGraphicsContextState SetActiveContext() const;

	/**
	 * @brief Uses the given shader program for future draw calls.
	 *
	 * @param shaderProgram The shader program.
	 */
	virtual void UseShaderProgram(TObjectPtr<UShaderProgram> shaderProgram);
};