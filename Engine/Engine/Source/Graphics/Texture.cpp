#include "Graphics/Image.h"
#include "Graphics/Texture.h"

int32 UTexture2D::GetHeight() const
{
	UM_ASSERT_NOT_REACHED();
}

int32 UTexture2D::GetWidth() const
{
	UM_ASSERT_NOT_REACHED();
}

void UTexture2D::SetData(const int32 width, const int32 height, const void* pixels, const ETextureFormat format, const EGenerateMipMaps generateMipMaps)
{
	(void)width;
	(void)height;
	(void)pixels;
	(void)format;
	(void)generateMipMaps;
	UM_ASSERT_NOT_REACHED();
}

void UTexture2D::SetDataFromImage(const FImage& image, const EGenerateMipMaps generateMipMaps)
{
	SetData(image.GetWidth(), image.GetHeight(), image.GetPixels(), ETextureFormat::R8G8B8A8_UNORM, generateMipMaps);
}

void UTexture2D::SetSamplerState(const FSamplerState& samplerState)
{
	(void)samplerState;
	UM_ASSERT_NOT_REACHED();
}