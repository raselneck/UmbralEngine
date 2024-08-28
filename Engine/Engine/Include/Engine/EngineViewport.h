#pragma once

#include "Engine/GameTime.h"
#include "Input/ButtonState.h"
#include "Input/Key.h"
#include "Input/MouseButton.h"
#include "Object/Object.h"
#include "EngineViewport.Generated.h"

class UApplication;
class UContentManager;
class UEngine;
class UEngineWindow;
class UGraphicsDevice;
class UInputManager;

/**
 * @brief Defines the base for all engine viewports.
 */
UM_CLASS(ChildOf=UEngineWindow)
class UEngineViewport : public UObject
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Allows this viewport to perform custom draw commands.
	 *
	 * @param gameTime Defines current game time values.
	 */
	virtual void Draw(const FGameTime& gameTime);

	/**
	 * @brief Gets the content manager associated with this viewport.
	 *
	 * @return The content manager associated with this viewport.
	 */
	[[nodiscard]] TObjectPtr<UContentManager> GetContentManager() const
	{
		return m_ContentManager;
	}

	/**
	 * @brief Gets the engine associated with this viewport.
	 *
	 * @return The engine associated with this viewport.
	 */
	[[nodiscard]] TObjectPtr<UEngine> GetEngine() const;

	/**
	 * @brief Gets the input manager associated with this viewport.
	 *
	 * @return The input manager associated with this viewport.
	 */
	[[nodiscard]] TObjectPtr<UInputManager> GetInputManager() const
	{
		return m_InputManager;
	}

	/**
	 * @brief Gets the graphics device associated with this viewport.
	 *
	 * @return The graphics device associated with this viewport.
	 */
	[[nodiscard]] TObjectPtr<UGraphicsDevice> GetGraphicsDevice() const
	{
		return m_GraphicsDevice;
	}

	/**
	 * @brief Gets the engine window associated with this viewport.
	 *
	 * @return The engine window associated with this viewport.
	 */
	[[nodiscard]] TObjectPtr<UEngineWindow> GetWindow() const
	{
		return m_Window;
	}

	/**
	 * @brief Gets the engine window associated with this viewport.
	 *
	 * @tparam WindowType The window's type.
	 * @return The engine window associated with this viewport.
	 */
	template<typename WindowType>
	[[nodiscard]] TObjectPtr<WindowType> GetWindow() const
	{
		return Cast<WindowType>(GetWindow());
	}

	/**
	 * @brief Checks to see if the given mouse button is down.
	 *
	 * @param button The mouse button.
	 * @return True if the mouse button is down, false if it is not.
	 */
	[[nodiscard]] bool IsButtonDown(EMouseButton button) const;

	/**
	 * @brief Checks to see if the given mouse button is up.
	 *
	 * @param button The mouse button.
	 * @return True if the mouse button is up, false if it is not.
	 */
	[[nodiscard]] bool IsButtonUp(EMouseButton button) const;

	/**
	 * @brief Checks to see if the given key is down.
	 *
	 * @param key The key.
	 * @return True if the key is down, false if it is not.
	 */
	[[nodiscard]] bool IsKeyDown(EKey key) const;

	/**
	 * @brief Checks to see if the given key is up.
	 *
	 * @param key The key.
	 * @return True if the key is up, false if it is not.
	 */
	[[nodiscard]] bool IsKeyUp(EKey key) const;

	/**
	 * @brief Updates this viewport.
	 *
	 * @param gameTime Defines current game time values.
	 */
	virtual void Update(const FGameTime& gameTime);

	/**
	 * @brief Checks to see if the given mouse button was just pressed.
	 *
	 * @param button The mouse button.
	 * @return True if \p button was just pressed, otherwise false.
	 */
	[[nodiscard]] bool WasButtonPressed(EMouseButton button) const;

	/**
	 * @brief Checks to see if the given mouse button was just released.
	 *
	 * @param button The mouse button.
	 * @return True if \p button was just released, otherwise false.
	 */
	[[nodiscard]] bool WasButtonReleased(EMouseButton button) const;

	/**
	 * @brief Checks to see if the given key was just pressed.
	 *
	 * @param key The key.
	 * @return True if \p key was just pressed, otherwise false.
	 */
	[[nodiscard]] bool WasKeyPressed(EKey key) const;

	/**
	 * @brief Checks to see if the given key was just released.
	 *
	 * @param key The key.
	 * @return True if \p key was just released, otherwise false.
	 */
	[[nodiscard]] bool WasKeyReleased(EKey key) const;

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

private:

	UM_PROPERTY()
	TObjectPtr<UEngineWindow> m_Window;

	UM_PROPERTY()
	TObjectPtr<UGraphicsDevice> m_GraphicsDevice;

	UM_PROPERTY()
	TObjectPtr<UInputManager> m_InputManager;

	UM_PROPERTY()
	TObjectPtr<UContentManager> m_ContentManager;
};