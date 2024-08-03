#pragma once

#include "Containers/Span.h"
#include "Graphics/GraphicsResource.h"
#include "Graphics/SamplerState.h"
#include "Graphics/TextureFormat.h"
#include "Texture.Generated.h"

class FImage;

/**
 * @brief An enumeration for specifying whether or not mip-maps should be generated for textures.
 */
enum class EGenerateMipMaps : bool
{
	No = false,
	Yes = true
};

/**
 * @brief Defines the base for all textures.
 */
UM_CLASS(Abstract)
class UTexture : public UGraphicsResource
{
	UM_GENERATED_BODY();

protected:

	/** @brief The maximum height of a texture. */
	static constexpr int32 MaxHeight = 16384;

	/** @brief The maximum width of a texture. */
	static constexpr int32 MaxWidth = 16384;
};

/**
 * @brief Defines a 2D texture address mode.
 */
class FTextureAddressMode2D final
{
public:

	/** The address mode for the U texture component. */
	ETextureAddressMode U = ETextureAddressMode::Clamp;

	/** The address mode for the V texture component. */
	ETextureAddressMode V = ETextureAddressMode::Clamp;
};

/**
 * @brief Defines the base for all 2D textures.
 */
UM_CLASS(Abstract)
class UTexture2D : public UTexture
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets this texture's height, in pixels.
	 *
	 * @return This texture's height, in pixels.
	 */
	[[nodiscard]] virtual int32 GetHeight() const;

	/**
	 * @brief Gets this texture's width, in pixels.
	 *
	 * @return This texture's width, in pixels.
	 */
	[[nodiscard]] virtual int32 GetWidth() const;

	/**
	 * @brief Sets this texture's data.
	 *
	 * @param width The image's width.
	 * @param height The image's height.
	 * @param pixels The image's pixels.
	 * @param format The texture format of the pixels.
	 * @param generateMipMaps Whether or not to automatically generate mip-maps.
	 */
	virtual void SetData(int32 width, int32 height, const void* pixels, ETextureFormat format, EGenerateMipMaps generateMipMaps);

	/**
	 * @brief Sets this texture's data from the given image.
	 *
	 * @param image The image.
	 * @param generateMipMaps Whether or not to automatically generate mip-maps.
	 */
	void SetDataFromImage(const FImage& image, EGenerateMipMaps generateMipMaps);

	/**
	 * @brief Sets this texture's sampler state.
	 *
	 * @param samplerState The new sampler state.
	 */
	virtual void SetSamplerState(const FSamplerState& samplerState);
};

/**
 * @brief Defines the base for all cube-map textures.
 */
UM_CLASS(Abstract)
class UTextureCube : public UTexture
{
	UM_GENERATED_BODY();
};