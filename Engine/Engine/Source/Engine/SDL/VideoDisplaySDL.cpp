#include "Engine/Logging.h"
#include "Engine/SDL/VideoDisplaySDL.h"
#include <SDL2/SDL.h>

constexpr ETextureFormat InvalidTextureFormat = static_cast<ETextureFormat>(-1);

/**
 * @brief Converts an SDL pixel format to a texture format.
 *
 * @param pixelFormat The SDL pixel format.
 * @return The texture format.
 */
static ETextureFormat ToTextureFormat(const uint32 pixelFormat)
{
	switch (pixelFormat)
	{
	case SDL_PIXELFORMAT_ARGB8888:  return ETextureFormat::R8G8B8A8_UNORM;
	case SDL_PIXELFORMAT_RGBA8888:  return ETextureFormat::R8G8B8A8_UNORM;
	case SDL_PIXELFORMAT_RGB888:    return ETextureFormat::R8G8B8A8_UNORM;
	default:                        return InvalidTextureFormat;
	}
}

/**
 * @brief Converts an SDL display mode to a video display mode.
 *
 * @param displayMode The SDL display mode.
 * @return The video display mode.
 */
static FVideoDisplayMode ToDisplayMode(const SDL_DisplayMode& displayMode)
{
	return
	{
		displayMode.w,
		displayMode.h,
		displayMode.refresh_rate,
		ToTextureFormat(displayMode.format),
	};
}

/**
 * @brief Converts an SDL rect to an int rect.
 *
 * @param rect The SDL rect.
 * @return The int rect.
 */
static constexpr FIntRect ToIntRect(const SDL_Rect& rect)
{
	return FIntRect { rect.x, rect.y, rect.w, rect.h };
}

FVideoDisplaySDL::FVideoDisplaySDL(const int32 displayIndex)
{
	const int32 numDisplayModes = SDL_GetNumDisplayModes(displayIndex);
	for (int32 idx = 0; idx < numDisplayModes; ++idx)
	{
		SDL_DisplayMode displayMode {};
		UM_ASSERT(SDL_GetDisplayMode(displayIndex, idx, &displayMode) == 0, "Failed to retrieve display mdoe");

		if (ToTextureFormat(displayMode.format) == InvalidTextureFormat)
		{
			// All pixel format names start with "SDL_PIXELFORMAT_"
			const FStringView pixelFormatName { SDL_GetPixelFormatName(displayMode.format) };
			UM_LOG(Warning, "Ignoring non-standard video display pixel format {}", pixelFormatName.RemoveLeft("SDL_PIXELFORMAT_"_sv.Length()));
			continue;
		}

		m_DisplayModes.Add(ToDisplayMode(displayMode));
	}

	const FStringView name { SDL_GetDisplayName(displayIndex) };
	m_Name.Append(name);

	SDL_Rect rect {};
	UM_ASSERT(SDL_GetDisplayBounds(displayIndex, &rect) == 0, "Failed to retrieve display bounds");
	m_Bounds = ToIntRect(rect);

	UM_ASSERT(SDL_GetDisplayUsableBounds(displayIndex, &rect) == 0, "Failed to retrieve display usable bounds");
	m_UsableBounds = ToIntRect(rect);
}