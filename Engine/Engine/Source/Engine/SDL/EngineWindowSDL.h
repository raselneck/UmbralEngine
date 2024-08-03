#pragma once

#include "Engine/EngineWindow.h"
#include "Graphics/GraphicsApi.h"
#include "EngineWindowSDL.Generated.h"

struct ImGuiViewport;
struct SDL_Window;
struct SDL_WindowEvent;

/**
 * @brief Defines the parameters used when creating an SDL engine window.
 */
class FEngineWindowParametersSDL
{
public:

	FString Title;
	EGraphicsApi GraphicsApi = EGraphicsApi::OpenGL;
	int32 WindowX = 0;
	int32 WindowY = 0;
	int32 WindowWidth = 1280;
	int32 WindowHeight = 720;
	uint32 WindowFlags = 0;
#if WITH_IMGUI
	ImGuiViewport* Viewport = nullptr;
#endif

	/**
	 * @brief Sets default values for the parameter values.
	 */
	FEngineWindowParametersSDL();

	/**
	 * @brief Sets parameter values on the given object creation context.
	 *
	 * @param context The object creation context.
	 */
	void ApplyToContext(FObjectCreationContext& context) const;

	/**
	 * @brief Retrieves parameter values from the given object creation context.
	 *
	 * @param context The object creation context.
	 */
	void RetrieveFromContext(const FObjectCreationContext& context);
};

/**
 * @brief Defines an SDL-backed engine window
 */
UM_CLASS()
class UEngineWindowSDL : public UEngineWindow
{
	UM_GENERATED_BODY();

public:

	/** @copydoc UEngineWindow::Close */
	virtual void Close() override;

	/** @copydoc UEngineWindow::Focus */
	virtual void Focus() override;

	/** @copydoc UEngineWindow::GetDrawableSize */
	[[nodiscard]] virtual FIntSize GetDrawableSize() const override;

	/**
	 * @brief Gets the graphics API this window was created for.
	 *
	 * @return The graphics API this window was created for.
	 */
	[[nodiscard]] EGraphicsApi GetGraphicsApi() const
	{
		return m_GraphicsApi;
	}

#if WITH_IMGUI
	/**
	 * @brief Gets the ImGui viewport associated with this window.
	 *
	 * @return The ImGui viewport associated with this window.
	 */
	[[nodiscard]] ImGuiViewport* GetImGuiViewport() const
	{
		return m_ImGuiViewport;
	}
#endif

	/**
	 * @brief Gets this window's underlying handle.
	 *
	 * @return This window's underlying handle.
	 */
	[[nodiscard]] SDL_Window* GetWindowHandle() const
	{
		return m_WindowHandle;
	}

	/** @copydoc UEngineWindow::GetPosition */
	[[nodiscard]] virtual FIntPoint GetPosition() const override;

	/** @copydoc UEngineWindow::GetSize */
	[[nodiscard]] virtual FIntSize GetSize() const override;

	/** @copydoc UEngineWindow::GetTitle */
	[[nodiscard]] virtual FStringView GetTitle() const override;

	/** @copydoc UEngineWindow::HideWindow */
	virtual void HideWindow() override;

	/** @copydoc UEngineWindow::IsFocused */
	[[nodiscard]] virtual bool IsFocused() const override;

	/** @copydoc UEngineWindow::IsMinimized */
	[[nodiscard]] virtual bool IsMinimized() const override;

	/** @copydoc UEngineWindow::IsOpen */
	[[nodiscard]] virtual bool IsOpen() const override;

	/**
	 * @brief Processes the given window event.
	 *
	 * @param event The window event.
	 */
	void ProcessEvent(const SDL_WindowEvent& event);

#if WITH_IMGUI
	/**
	 * @brief Sets the ImGui viewport associated with this window.
	 *
	 * @param viewport The viewport.
	 */
	void SetImGuiViewport(ImGuiViewport* viewport);
#endif

	/** @copydoc UEngineWindow::SetOpacity */
	virtual void SetOpacity(float opacity) override;

	/** @copydoc UEngineWindow::SetPosition */
	virtual void SetPosition(const FIntPoint& position) override;

	/** @copydoc UEngineWindow::SetSize */
	virtual void SetSize(const FIntSize& size) override;

	/** @copydoc UEngineWindow::SetTitle */
	virtual void SetTitle(FStringView title) override;

	/** @copydoc UEngineWindow::ShowWindow */
	virtual void ShowWindow() override;

protected:

	/** @copydoc UObject::Destroyed */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UObject::Destroyed */
	virtual void Destroyed() override;

private:

	FString m_Title;
	SDL_Window* m_WindowHandle = nullptr;
	EGraphicsApi m_GraphicsApi = EGraphicsApi::OpenGL;
	bool m_IsOpen = false;

#if WITH_IMGUI
	ImGuiViewport* m_ImGuiViewport = nullptr;
#endif
};