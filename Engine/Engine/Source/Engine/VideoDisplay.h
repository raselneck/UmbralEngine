#pragma once

#include "Containers/StringView.h"
#include "Math/Rectangle.h"
#include "Graphics/TextureFormat.h"

/**
 * @brief Defines a display mode for a video display.
 */
class FVideoDisplayMode final
{
public:

	/** @brief This display mode's width. */
	int32 Width = 0;

	/** @brief This display mode's height. */
	int32 Height = 0;

	/** @brief This display mode's refresh rate. */
	int32 RefreshRate = 0;

	/** @brief This display mode's surface / texture. */
	ETextureFormat Format;
};

/**
 * @brief Defines the interface for querying a video display.
 */
class IVideoDisplay
{
public:

	/**
	 * @brief Gets this video display's bounds.
	 *
	 * @return This video display's bounds.
	 */
	[[nodiscard]] virtual FIntRect GetBounds() const = 0;

	/**
	 * @brief Gets this video display's display modes.
	 *
	 * @return This video display's display modes.
	 */
	[[nodiscard]] virtual TSpan<const FVideoDisplayMode> GetDisplayModes() const = 0;

	/**
	 * @brief Gets this video display's name.
	 *
	 * @return This video display's name.
	 */
	[[nodiscard]] virtual FStringView GetName() const = 0;

	/**
	 * @brief Gets this video display's usable bounds.
	 *
	 * A display's "usable" bounds are similar to its bounds, but have system-reserved portions
	 * removed (such as the menu bar and dock on macOS).
	 *
	 * @return This video display's usable bounds.
	 */
	[[nodiscard]] virtual FIntRect GetUsableBounds() const = 0;

protected:

	/**
	 * @brief Destroys this video display.
	 */
	virtual ~IVideoDisplay() = default;
};