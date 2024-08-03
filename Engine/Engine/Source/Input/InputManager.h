#pragma once

#include "Input/ButtonState.h"
#include "Input/Key.h"
#include "Input/MouseButton.h"
#include "Object/Object.h"
#include "InputManager.Generated.h"

UM_CLASS(Abstract)
class UInputManager : public UObject
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets the state of the given mouse button.
	 *
	 * @param button The mouse button.
	 * @return The mouse button's state.
	 */
	[[nodiscard]] virtual EButtonState GetButtonState(EMouseButton button) const;

	/**
	 * @brief Gets the state of the given key.
	 *
	 * @param key The key.
	 * @return The key's state.
	 */
	[[nodiscard]] virtual EKeyState GetKeyState(EKey key) const;

	/**
	 * @brief Gets a value indicating whether or not a button was pressed this frame.
	 *
	 * @param button The mouse button.
	 * @return True if the mouse button was pressed this frame, otherwise false.
	 */
	[[nodiscard]] virtual bool WasButtonPressed(EMouseButton button) const;

	/**
	 * @brief Gets a value indicating whether or not a button was released this frame.
	 *
	 * @param button The mouse button.
	 * @return True if the mouse button was released this frame, otherwise false.
	 */
	[[nodiscard]] virtual bool WasButtonReleased(EMouseButton button) const;

	/**
	 * @brief Gets a value indicating whether or not a key was pressed this frame.
	 *
	 * @param key The key.
	 * @return True if the key was pressed this frame, otherwise false.
	 */
	[[nodiscard]] virtual bool WasKeyPressed(EKey key) const;

	/**
	 * @brief Gets a value indicating whether or not a key was release this frame.
	 *
	 * @param key The key.
	 * @return True if the key was release this frame, otherwise false.
	 */
	[[nodiscard]] virtual bool WasKeyReleased(EKey key) const;
};