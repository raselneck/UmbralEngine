#include "Input/InputManager.h"

EButtonState UInputManager::GetButtonState(const EMouseButton button) const
{
	(void)button;
	return EButtonState::Released;
}

EKeyState UInputManager::GetKeyState(const EKey key) const
{
	(void)key;
	return EKeyState::Released;
}

bool UInputManager::WasButtonPressed(const EMouseButton button) const
{
	(void)button;
	return false;
}

bool UInputManager::WasButtonReleased(const EMouseButton button) const
{
	(void)button;
	return false;
}

bool UInputManager::WasKeyPressed(const EKey key) const
{
	(void)key;
	return false;
}

bool UInputManager::WasKeyReleased(const EKey key) const
{
	(void)key;
	return false;
}