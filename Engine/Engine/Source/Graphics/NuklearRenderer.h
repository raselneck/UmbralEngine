#pragma once

#include "Engine/EngineWindow.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/VertexDeclaration.h"
#include "Math/Vector2.h"
#include <nuklear.h>

/**
 * @brief Defines the vertex used for rendering the GUI.
 */
struct FGuiVertex
{
	FVector2 Position;
	FVector2 UV;
	FColor Color;

	/**
	 * @brief Gets this vertex type's declaration.
	 *
	 * @return This vertex type's declaration.
	 */
	static const FVertexDeclaration& GetVertexDeclaration();
};

/**
 * @brief Defines a GUI renderer.
 */
class FGuiRenderer final
{
public:

	/**
	 * @brief Destroys this GUI renderer.
	 */
	~FGuiRenderer();

	/**
	 * @brief Gets this GUI renderer's context.
	 *
	 * @return This GUI renderer's context.
	 */
	[[nodiscard]] const struct nk_context* GetContext() const
	{
		return &m_DrawContext;
	}

	/**
	 * @brief Gets this GUI renderer's context.
	 *
	 * @return This GUI renderer's context.
	 */
	[[nodiscard]] struct nk_context* GetContext()
	{
		return &m_DrawContext;
	}

	/**
	 * @brief Initializes this GUI renderer.
	 *
	 * @param gameWindow The game window to use for rendering.
	 * @return True if this GUI renderer was initialized, otherwise false.
	 */
	TErrorOr<void> Initialize(TSharedPtr<FGameWindow> gameWindow);

	/**
	 * @brief Renders all of the GUI.
	 */
	void Render();

private:

	struct nk_buffer m_DrawCommands;
	struct nk_context m_DrawContext;
	struct nk_font_atlas m_FontAtlas;
	struct nk_draw_null_texture m_NullTexture;
	FDynamicVertexBuffer m_Vertices;
	FDynamicIndexBuffer m_Indices;
	FShaderProgram m_Program;
	FTexture2D m_FontAtlasTexture;
	TSharedPtr<FGameWindow> m_GameWindow;
};