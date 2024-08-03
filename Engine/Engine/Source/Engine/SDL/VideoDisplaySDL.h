#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"
#include "Engine/VideoDisplay.h"

/**
 * @brief Defines an SDL-backed video display.
 */
class FVideoDisplaySDL : public IVideoDisplay
{
public:

	/**
	 * @brief Sets default values for this video display's properties.
	 *
	 * @param displayIndex The video display index.
	 */
	explicit FVideoDisplaySDL(int32 displayIndex);

	/**
	 * @brief Destroys this video display.
	 */
	virtual ~FVideoDisplaySDL() override = default;

	/** @copydoc IVideoDisplay::GetBounds */
	[[nodiscard]] virtual FIntRect GetBounds() const override
	{
		return m_Bounds;
	}

	/** @copydoc IVideoDisplay::GetDisplayModes */
	[[nodiscard]] virtual TSpan<const FVideoDisplayMode> GetDisplayModes() const override
	{
		return m_DisplayModes.AsSpan();
	}

	/** @copydoc IVideoDisplay::GetName */
	[[nodiscard]] virtual FStringView GetName() const override
	{
		return m_Name.AsStringView();
	}

	/** @copydoc IVideoDisplay::GetUsableBounds */
	[[nodiscard]] virtual FIntRect GetUsableBounds() const override
	{
		return m_UsableBounds;
	}

private:

	TArray<FVideoDisplayMode> m_DisplayModes;
	FString m_Name;
	FIntRect m_Bounds;
	FIntRect m_UsableBounds;
};