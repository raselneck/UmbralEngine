#pragma once

#include "Containers/String.h"
#include "Containers/StaticArray.h"
#include "ImGui/ImGui.h"
#include "ImGui/ImGuiSystem.h"
#include "ImGuiSystemSDL.Generated.h"

class UApplicationSDL;
class UEngineWindowSDL;
class UInputManagerSDL;
struct SDL_Cursor;
struct SDL_KeyboardEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseWheelEvent;
struct SDL_TextInputEvent;
struct SDL_Window;
struct SDL_WindowEvent;

UM_CLASS(ChildOf=UApplicationSDL)
class UImGuiSystemSDL : public UImGuiSystem
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets the clipboard's current text.
	 *
	 * @return The clipboard's current text.
	 */
	[[nodiscard]] const char* GetClipboardText() const;

	/**
	 * @brief Gets the main window's OpenGL context, if there is one.
	 *
	 * @return The main window's OpenGL context.
	 */
	[[nodiscard]] void* GetMainOpenGLContext() const;

	/**
	 * @brief Gets the main window.
	 *
	 * @return The main window.
	 */
	[[nodiscard]] TObjectPtr<UEngineWindowSDL> GetMainWindow() const;

	/** @copydoc UImGuiSystem::NewFrame */
	virtual void NewFrame(const FGameTime& gameTime) override;

	// See ImGui_ImplSDL2_ProcessEvent
	// TODO ProcessControllerEvent
	// TODO ProcessDisplayEvent

	/**
	 * @brief Processes a keyboard event.
	 *
	 * @param event The keyboard event.
	 */
	void ProcessKeyboardEvent(const SDL_KeyboardEvent& event);

	/**
	 * @brief Processes a mouse button event.
	 *
	 * @param event The mouse button event.
	 */
	void ProcessMouseButtonEvent(const SDL_MouseButtonEvent& event);

	/**
	 * @brief Processes a mouse motion event.
	 *
	 * @param event The mouse motion event.
	 */
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& event);

	/**
	 * @brief Processes a mouse wheel event.
	 *
	 * @param event The mouse wheel event.
	 */
	void ProcessMouseWheelEvent(const SDL_MouseWheelEvent& event);

	/**
	 * @brief Processes a text input event.
	 *
	 * @param event The text input event.
	 */
	void ProcessTextInputEvent(const SDL_TextInputEvent& event);

	/**
	 * @brief Processes a window event.
	 *
	 * @param event The window event.
	 */
	void ProcessWindowEvent(const SDL_WindowEvent& event);

	/** @copydoc UImGuiSystem::Render */
	virtual void Render(const FGameTime& gameTime) override;

	/**
	 * @brief Sets the clipboard's text.
	 *
	 * @param clipboardText The new clipboard text.
	 */
	void SetClipboardText(const char* clipboardText);

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UObject::Destroyed */
	virtual void Destroyed() override;

private:

	/**
	 * @brief Initializes the ImGui platform interface.
	 */
	void InitializePlatformInterface();

	/**
	 * @brief Checks to see if the platform interface is supported.
	 *
	 * @return True if the platform interface is supported, otherwise false.
	 */
	[[nodiscard]] bool IsPlatformInterfaceSupported() const;

	/**
	 * @brief Loads fonts for ImGui.
	 */
	void LoadFonts();

	/**
	 * @brief Shuts down the ImGui platform interface.
	 */
	void ShutdownPlatformInterface();

	// TODO UpdateControllers

	/**
	 * @brief Updates ImGui monitor information.
	 */
	void UpdateMonitors();

	/**
	 * @brief Updates the mouse's cursor based on what ImGui wants.
	 */
	void UpdateMouseCursor();

	/**
	 * @brief Updates the mouse's position in ImGui.
	 */
	void UpdateMousePosition();

	UM_PROPERTY()
	TObjectPtr<UApplicationSDL> m_Application;

	// See ImGui_ImplSDL2_Data
	mutable char* m_ClipboardText = nullptr;
	uint32 m_MouseWindowId = 0;
	uint32 m_MouseButtonsDown = 0;
	TStaticArray<SDL_Cursor*, ImGuiMouseCursor_COUNT> m_MouseCursors;
	SDL_Cursor* m_MouseLastCursor = nullptr;
	int32 m_MouseLastLeaveFrame = 0;
	bool m_MouseCanUseGlobalState = false;
	bool m_MouseCanReportHoveredViewport = false;
	bool m_NeedToUpdateMonitors = false;
};