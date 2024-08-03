#pragma once

#include "Graphics/GraphicsResource.h"
#include "ImGuiRenderer.Generated.h"

class FGameTime;
struct ImDrawData;
struct ImGuiViewport;

/**
 * @brief Defines the base for ImGui renderers.
 */
UM_CLASS()
class UImGuiRenderer : public UGraphicsResource
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Tells this renderer to draw.
	 *
	 * @param gameTime Provides game time information.
	 */
	virtual void Draw(const FGameTime& gameTime, const ImDrawData* drawData);

	/**
	 * @brief Gets the ImGui viewport associated with this renderer.
	 *
	 * @return The ImGui viewport associated with this renderer.
	 */
	[[nodiscard]] virtual ImGuiViewport* GetImGuiViewport() const;

	/**
	 * @brief Notifies this renderer that a new frame has begun.
	 *
	 * @param gameTime Provides game time information.
	 */
	virtual void NewFrame(const FGameTime& gameTime);
};