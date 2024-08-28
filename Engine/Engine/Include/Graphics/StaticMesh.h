#pragma once

#include "Graphics/GraphicsResource.h"
#include "StaticMesh.Generated.h"

class UContentManager;

/**
 * @brief Defines a static mesh.
 */
UM_CLASS()
class UStaticMesh : public UGraphicsResource
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Attempts to load static mesh data from a file.
	 *
	 * @param filePath The path to the file.
	 * @return The error that was encountered while loading the file, otherwise nothing on success.
	 */
	[[nodiscard]] TErrorOr<void> LoadFromFile(TBadge<UContentManager>, const FString& filePath);
};