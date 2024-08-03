#include "Engine/Application.h"
#include "Engine/EngineWindow.h"
#include "Engine/Logging.h"

void UEngineWindow::Close()
{
	UM_ASSERT_NOT_REACHED();
}

void UEngineWindow::Focus()
{
	UM_ASSERT_NOT_REACHED();
}

TObjectPtr<UApplication> UEngineWindow::GetApplication() const
{
	return FindAncestorOfType<UApplication>();
}

FIntSize UEngineWindow::GetDrawableSize() const
{
	UM_ASSERT_NOT_REACHED();
}

FIntPoint UEngineWindow::GetPosition() const
{
	UM_ASSERT_NOT_REACHED();
}

FIntSize UEngineWindow::GetSize() const
{
	UM_ASSERT_NOT_REACHED();
}

FStringView UEngineWindow::GetTitle() const
{
	UM_ASSERT_NOT_REACHED();
}

TObjectPtr<UEngineViewport> UEngineWindow::GetViewport() const
{
	TObjectPtr<UApplication> application = FindAncestorOfType<UApplication>();
	return application->GetViewport(this);
}

void UEngineWindow::HideWindow()
{
	UM_ASSERT_NOT_REACHED();
}

bool UEngineWindow::IsFocused() const
{
	UM_ASSERT_NOT_REACHED();
}

bool UEngineWindow::IsMinimized() const
{
	UM_ASSERT_NOT_REACHED();
}

bool UEngineWindow::IsOpen() const
{
	UM_ASSERT_NOT_REACHED();
}

void UEngineWindow::SetOpacity(const float opacity)
{
	(void)opacity;
	UM_ASSERT_NOT_REACHED();
}

void UEngineWindow::SetPosition(const FIntPoint& position)
{
	(void)position;
	UM_ASSERT_NOT_REACHED();
}

void UEngineWindow::SetSize(const FIntSize& size)
{
	(void)size;
	UM_ASSERT_NOT_REACHED();
}

void UEngineWindow::SetTitle(const FStringView title)
{
	(void)title;
	UM_ASSERT_NOT_REACHED();
}

void UEngineWindow::ShowWindow()
{
	UM_ASSERT_NOT_REACHED();
}