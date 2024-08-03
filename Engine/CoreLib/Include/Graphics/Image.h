#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Engine/Error.h"
#include "Graphics/Color.h"

/**
 * @brief An enumeration of image file types.
 */
enum class EImageFileType
{
	PNG,
	BMP,
	TGA,
	JPG
};

/**
 * @brief Defines a 2D image.
 */
class FImage final
{
	UM_DISABLE_COPY(FImage);

public:

	/**
	 * @brief Sets default values for this image's properties.
	 */
	FImage();

	// TODO ContainsPoint

	/**
	 * @brief Gets this image's height.
	 *
	 * @returns This image's height.
	 */
	[[nodiscard]] int32 GetHeight() const
	{
		return m_Height;
	}

	/**
	 * @brief Gets the color of the pixel at the given coordinates.
	 *
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @return The pixel at (\p x, \p y). If the coordinates are out of bounds, this will return an empty color.
	 */
	[[nodiscard]] FColor GetPixel(int32 x, int32 y) const;

	/**
	 * @brief Gets the pointer to this image's pixels.
	 *
	 * @returns The pointer to this image's pixels.
	 */
	[[nodiscard]] const FColor* GetPixels() const
	{
		return m_Pixels.GetData();
	}

	/**
	 * @brief Gets the pointer to this image's pixels.
	 *
	 * @returns The pointer to this image's pixels.
	 */
	[[nodiscard]] FColor* GetPixels()
	{
		return m_Pixels.GetData();
	}

	// TODO GetRect

	// TODO GetSize

	/**
	 * @brief Gets this image's width.
	 *
	 * @returns This image's width.
	 */
	[[nodiscard]] int32 GetWidth() const
	{
		return m_Width;
	}

	/**
	 * @brief Attempts to load image data from the given file.
	 *
	 * @param fileName The name of the file to load.
	 * @returns True if the image was loaded, otherwise false.
	 */
	[[nodiscard]] TErrorOr<void> LoadFromFile(FStringView fileName);

	/**
	 * @brief Attempts to load image data from memory.
	 *
	 * @param pixels The image pixels.
	 * @param width The image width.
	 * @param height The image height.
	 * @return True if the image was loaded, otherwise false.
	 */
	[[nodiscard]] TErrorOr<void> LoadFromMemory(const FColor* pixels, int32 width, int32 height);

	/**
	 * @brief Gets this image's resource name.
	 *
	 * @return This image's resource name.
	 */
	[[nodiscard]] FStringView GetResourceName() const
	{
		return m_ResourceName.AsStringView();
	}

	/**
	 * @brief Saves this image to a file.
	 *
	 * The file type will be guessed from the file name. If none could be guessed,
	 * then the image will be saved as a PNG.
	 *
	 * @param fileName The name of the file to save to.
	 * @return True if saving was successful, otherwise false.
	 */
	[[nodiscard]] TErrorOr<void> SaveToFile(FStringView fileName);

	/**
	 * @brief Saves this image to a file.
	 *
	 * @param fileName The name of the file to save to.
	 * @param imageFileType The image file type.
	 * @return True if saving was successful, otherwise false.
	 */
	[[nodiscard]] TErrorOr<void> SaveToFile(FStringView fileName, EImageFileType imageFileType);

	/**
	 * @brief Sets the color of the pixel at the given coordinates.
	 *
	 * @param x The X coordinate.
	 * @param y The Y coordinate.
	 * @param color The new color.
	 */
	void SetPixel(int32 x, int32 y, FColor color);

	/**
	 * @brief Sets this image's resource name.
	 *
	 * @param resourceName The new resource name.
	 */
	void SetResourceName(FStringView resourceName);

	/**
	 * @brief Sets this image's resource name.
	 *
	 * @param resourceName The new resource name.
	 */
	void SetResourceName(FString&& resourceName);

	/**
	 * @brief Sets this image's resource name.
	 *
	 * @param resourceName The new resource name.
	 */
	void SetResourceName(const FString& resourceName);

	/**
	 * @brief Sets this image's size. This will clear out any existing pixel data.
	 *
	 * @param width The new width.
	 * @param height The new height.
	 * @returns True if this image was resized, otherwise false.
	 */
	[[nodiscard]] TErrorOr<void> SetSize(int32 width, int32 height);

private:

	FString m_ResourceName;
	TArray<FColor> m_Pixels;
	int32 m_Width = 0;
	int32 m_Height = 0;
};