#include "Graphics/Image.h"
#include "Engine/Logging.h"
#include "HAL/File.h"
#include "HAL/FileStream.h"
#include "HAL/FileSystem.h"
#include "HAL/Path.h"
#include "Memory/Memory.h"
#include "Templates/NumericLimits.h"

//#define STBI_ASSERT(x) UM_ASSERT(x, #x)
#define STBI_MALLOC(count) FMemory::Allocate(static_cast<FMemory::SizeType>(count))
#define STBI_REALLOC(block, size) FMemory::Reallocate(block, static_cast<FMemory::SizeType>(size))
#define STBI_FREE(memory) FMemory::Free(memory)
#define STBI_MAX_DIMENSIONS 16384
#define STBI_NO_PSD
#define STBI_WRITE_NO_STDIO

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

/**
 * @brief Writes STB image data to a file stream.
 *
 * @param context The file stream pointer.
 * @param data The image data.
 * @param dataLength The length of the image data, in bytes.
 */
static void WriteStbImageDataToFileStream(void* context, void* data, const int32 dataLength)
{
	reinterpret_cast<IFileStream*>(context)->Write(data, static_cast<uint64>(dataLength));
}

FImage::FImage()
	: m_ResourceName { "<image>"_sv }
{
}

FColor FImage::GetPixel(int32 x, int32 y) const
{
	if (x < 0 || x >= m_Width || y < 0 || y >= m_Height)
	{
		return FColor {};
	}

	const int32 pixelIndex = y * m_Width + x;
	return m_Pixels[pixelIndex];
}

TErrorOr<void> FImage::LoadFromFile(const FStringView fileName)
{
	TRY_EVAL(TArray<uint8> fileBytes, FFile::ReadAllBytes(fileName));

	int32 width, height;
	stbi_uc* filePixels = stbi_load_from_memory(fileBytes.GetData(), fileBytes.Num(), &width, &height, nullptr, STBI_rgb_alpha);

	if (filePixels == nullptr)
	{
		return MAKE_ERROR("File \"{}\" does not contain valid image data", fileName);
	}

	m_Width = width;
	m_Height = height;
	m_Pixels.SetNum(width * height);
	FMemory::Copy(m_Pixels.GetData(), filePixels, m_Pixels.Num() * sizeof(FColor));

	STBI_FREE(filePixels);

	m_ResourceName.Reset();
	m_ResourceName.Append(FPath::GetBaseFileName(fileName));

	return {};
}

TErrorOr<void> FImage::LoadFromMemory(const FColor* pixels, const int32 width, const int32 height)
{
	if (width < 0)
	{
		return MAKE_ERROR("Attempting to set negative width for image");
	}

	if (height < 0)
	{
		return MAKE_ERROR("Attempting to set negative height for image");
	}

	if (static_cast<size_t>(width) * static_cast<size_t>(height) > static_cast<size_t>(TNumericLimits<int32>::MaxValue))
	{
		return MAKE_ERROR("The requested size ({}x{}) is too large for an image", width, height);
	}

	m_Pixels.Reset();
	if (pixels == nullptr)
	{
		m_Pixels.AddZeroed(width * height);
	}
	else
	{
		m_Pixels.Reserve(width * height);
		m_Pixels.Append(pixels, width * height);
	}

	m_Width = width;
	m_Height = height;

	return {};
}

TErrorOr<void> FImage::SaveToFile(const FStringView fileName)
{
	const EImageFileType imageFileType = [fileName]
	{
		if (fileName.EndsWith(".jpg"_sv, EIgnoreCase::Yes) || fileName.EndsWith(".jpeg"_sv, EIgnoreCase::Yes))
		{
			return EImageFileType::JPG;
		}
		if (fileName.EndsWith(".tga"_sv, EIgnoreCase::Yes))
		{
			return EImageFileType::TGA;
		}
		if (fileName.EndsWith(".bmp"_sv, EIgnoreCase::Yes))
		{
			return EImageFileType::BMP;
		}
		return EImageFileType::PNG;
	}();

	return SaveToFile(fileName, imageFileType);
}

TErrorOr<void> FImage::SaveToFile(const FStringView fileName, const EImageFileType imageFileType)
{
	TSharedPtr<IFileStream> fileStream = FFileSystem::OpenWrite(fileName);

	const int32 writeResult = [this, imageFileType, fileStream]
	{
		constexpr int32 jpegQuality = 95;
		constexpr int32 composition = 4; // RGBA
		const int32 strideInBytes = m_Width * static_cast<int>(sizeof(FColor));

		switch (imageFileType)
		{
		case EImageFileType::PNG:
			return stbi_write_png_to_func(WriteStbImageDataToFileStream, fileStream.Get(),
			                              m_Width, m_Height, composition, m_Pixels.GetData(), strideInBytes);
		case EImageFileType::BMP:
			return stbi_write_bmp_to_func(WriteStbImageDataToFileStream, fileStream.Get(),
			                              m_Width, m_Height, composition, m_Pixels.GetData());
		case EImageFileType::TGA:
			return stbi_write_tga_to_func(WriteStbImageDataToFileStream, fileStream.Get(),
			                              m_Width, m_Height, composition, m_Pixels.GetData());
		case EImageFileType::JPG:
			return stbi_write_jpg_to_func(WriteStbImageDataToFileStream, fileStream.Get(),
			                              m_Width, m_Height, composition, m_Pixels.GetData(), jpegQuality);
		default:
			UM_ASSERT_NOT_REACHED_MSG("Unsupported image file type given");
		}
	}();

	m_ResourceName.Reset();
	m_ResourceName.Append(FPath::GetBaseFileName(fileName));

	if (writeResult == 0)
	{
		return MAKE_ERROR("Failed to save image to \"{}\", fileName");
	}

	return {};
}

void FImage::SetPixel(int32 x, int32 y, FColor color)
{
	if (x < 0 || x >= m_Width || y < 0 || y >= m_Height)
	{
		return;
	}

	const int32 pixelIndex = y * m_Width + x;
	m_Pixels[pixelIndex] = color;
}

void FImage::SetResourceName(const FStringView resourceName)
{
	m_ResourceName.Reset();
	m_ResourceName.Append(resourceName);
}

void FImage::SetResourceName(FString&& resourceName)
{
	m_ResourceName = MoveTemp(resourceName);
}

void FImage::SetResourceName(const FString& resourceName)
{
	m_ResourceName = resourceName;
}

TErrorOr<void> FImage::SetSize(const int32 width, const int32 height)
{
	if (width < 0)
	{
		return MAKE_ERROR("Attempting to set negative width for image");
	}

	if (height < 0)
	{
		return MAKE_ERROR("Attempting to set negative height for image");
	}

	if (static_cast<size_t>(width) * static_cast<size_t>(height) > static_cast<size_t>(TNumericLimits<int32>::MaxValue))
	{
		return MAKE_ERROR("The requested size ({}x{}) is too large for an image", width, height);
	}

	// FIXME This could overflow
	const int32 pixelCount = width * height;
	const int32 pixelDataSize = pixelCount * static_cast<int32>(sizeof(FColor));

	m_Pixels.SetNum(pixelCount);
	FMemory::ZeroOut(m_Pixels.GetData(), pixelDataSize);
	m_Width = width;
	m_Height = height;

	return {};
}