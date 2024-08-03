#pragma once

#include "Graphics/GraphicsApi.h"
#include "Math/Point.h"
#include "Math/Size.h"
#include "Misc/Badge.h"
#include "Object/Object.h"
#include "EngineWindow.Generated.h"

class UApplication;
class UEngineViewport;

/**
 * @brief Defines a way to interact with an engine window.
 */
UM_CLASS(Abstract)
class UEngineWindow : public UObject
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Closes this window.
	 */
	virtual void Close();

	/**
	 * @brief Focuses this window.
	 */
	virtual void Focus();

	/**
	 * @brief Gets the application that owns this window.
	 *
	 * @return The application that owns this window.
	 */
	[[nodiscard]] TObjectPtr<UApplication> GetApplication() const;

	/**
	 * @brief Gets the size of this window's drawable area.
	 *
	 * @return The size of this window's drawable area.
	 */
	[[nodiscard]] virtual FIntSize GetDrawableSize() const;

	/**
	 * @brief Gets this window's position.
	 *
	 * @return This window's position.
	 */
	[[nodiscard]] virtual FIntPoint GetPosition() const;

	/**
	 * @brief Gets the size of this window.
	 *
	 * @return The size of this window.
	 */
	[[nodiscard]] virtual FIntSize GetSize() const;

	/**
	 * @brief Gets this window's title.
	 *
	 * @return This window's title.
	 */
	[[nodiscard]] virtual FStringView GetTitle() const;

	/**
	 * @brief Gets this window's registered viewport.
	 *
	 * @return This window's registered viewport.
	 */
	[[nodiscard]] TObjectPtr<UEngineViewport> GetViewport() const;

	/**
	 * @brief Hides this window if it is shown.
	 */
	virtual void HideWindow();

	/**
	 * @brief Checks to see if this window is focused.
	 *
	 * @return True if this window is focused, otherwise false.
	 */
	[[nodiscard]] virtual bool IsFocused() const;

	/**
	 * @brief Checks to see if this window is minimized.
	 *
	 * @return True if this window is minimized, otherwise false.
	 */
	[[nodiscard]] virtual bool IsMinimized() const;

	/**
	 * @brief Gets a value indicating whether or not this window is open.
	 *
	 * @return True if this window is open, otherwise false.
	 */
	[[nodiscard]] virtual bool IsOpen() const;

	/**
	 * @brief Sets this window's opacity.
	 *
	 * @param opacity The opacity.
	 */
	virtual void SetOpacity(float opacity);

	/**
	 * @brief Sets this window's position.
	 *
	 * @param position The new position.
	 */
	virtual void SetPosition(const FIntPoint& position);

	/**
	 * @brief Sets this window's size.
	 *
	 * @param size The new size.
	 */
	virtual void SetSize(const FIntSize& size);

	/**
	 * @brief Sets this window's title.
	 *
	 * @param title The new title.
	 */
	virtual void SetTitle(FStringView title);

	/**
	 * @brief Shows this window if it is hidden.
	 */
	virtual void ShowWindow();
};