#include "ImGui/ImGuiRenderer.h"

void UImGuiRenderer::Draw(const FGameTime& gameTime, const ImDrawData* drawData)
{
	(void)gameTime;
	(void)drawData;

	UM_ASSERT_NOT_REACHED();
}

ImGuiViewport* UImGuiRenderer::GetImGuiViewport() const
{
	UM_ASSERT_NOT_REACHED();
}

void UImGuiRenderer::NewFrame(const FGameTime& gameTime)
{
	(void)gameTime;

	UM_ASSERT_NOT_REACHED();
}