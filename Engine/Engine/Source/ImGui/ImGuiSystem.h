#pragma once

#include "Object/Object.h"
#include "ImGuiSystem.Generated.h"

class FGameTime;

UM_CLASS(Abstract)
class UImGuiSystem : public UObject
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Tells this ImGui system that a new frame has begun.
	 */
	virtual void NewFrame(const FGameTime& gameTime);

	/**
	 * @brief Renders queued ImGui commands to a draw list and then renders each viewports.
	 *
	 * @param gameTime Contains game time information.
	 */
	virtual void Render(const FGameTime& gameTime);
};