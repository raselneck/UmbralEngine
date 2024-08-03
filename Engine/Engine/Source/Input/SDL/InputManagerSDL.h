#pragma once

#include "Containers/StaticArray.h"
#include "Input/InputManager.h"
#include "InputManagerSDL.Generated.h"

class UEngineWindowSDL;
struct SDL_KeyboardEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseButtonEvent;
struct SDL_MouseWheelEvent;
struct SDL_ControllerAxisEvent;
struct SDL_ControllerButtonEvent;
struct SDL_ControllerDeviceEvent;

UM_CLASS(ChildOf=UEngineWindowSDL)
class UInputManagerSDL : public UInputManager
{
	UM_GENERATED_BODY();

public:

	/** @copydoc UInputManager::GetButtonState */
	[[nodiscard]] virtual EButtonState GetButtonState(EMouseButton button) const override;

	/** @copydoc UInputManager::GetKeyState */
	[[nodiscard]] virtual EKeyState GetKeyState(EKey key) const override;

	// https://wiki.libsdl.org/SDL2/SDL_Event
	void ProcessGamePadAxisEvent(const SDL_ControllerAxisEvent& event);
	void ProcessGamePadButtonDownEvent(const SDL_ControllerButtonEvent& event);
	void ProcessGamePadButtonUpEvent(const SDL_ControllerButtonEvent& event);
	void ProcessGamePadAddedEvent(const SDL_ControllerDeviceEvent& event);
	void ProcessGamePadRemovedEvent(const SDL_ControllerDeviceEvent& event);
	void ProcessGamePadRemappedEvent(const SDL_ControllerDeviceEvent& event);
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& event);
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& event);
	void ProcessMouseMoveEvent(const SDL_MouseMotionEvent& event);
	void ProcessMouseButtonDownEvent(const SDL_MouseButtonEvent& event);
	void ProcessMouseButtonUpEvent(const SDL_MouseButtonEvent& event);
	void ProcessMouseWheelEvent(const SDL_MouseWheelEvent& event);

	/**
	 * @brief Updates this input manager before polling events from the engine loop.
	 */
	void UpdateBeforePollingEvents();

	/** @copydoc UInputManager::WasButtonPressed */
	[[nodiscard]] virtual bool WasButtonPressed(EMouseButton button) const override;

	/** @copydoc UInputManager::WasButtonReleased */
	[[nodiscard]] virtual bool WasButtonReleased(EMouseButton button) const override;

	/** @copydoc UInputManager::WasKeyPressed */
	[[nodiscard]] virtual bool WasKeyPressed(EKey key) const override;

	/** @copydoc UInputManager::WasKeyReleased */
	[[nodiscard]] virtual bool WasKeyReleased(EKey key) const override;

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

private:

	UM_PROPERTY()
	TObjectPtr<UEngineWindowSDL> m_Window;

	TStaticArray<EKeyState, 256> m_PreviousKeyStates;
	TStaticArray<EKeyState, 256> m_CurrentKeyStates;
	TStaticArray<EButtonState, 5> m_PreviousButtonStates;
	TStaticArray<EButtonState, 5> m_CurrentButtonStates;
};