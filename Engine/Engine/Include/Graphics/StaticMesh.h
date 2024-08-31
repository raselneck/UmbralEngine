#pragma once

#include "Graphics/GraphicsResource.h"
#include "StaticMesh.Generated.h"

class UContentManager;

/**
 * @brief Defines a static mesh.
 */
UM_CLASS(ChildOf=UContentManager)
class UStaticMesh final : public UGraphicsResource
{
	friend class UContentManager;

	UM_GENERATED_BODY();

private:

	/**
	 * @brief Gets the content manager that was used to load this static mesh.
	 *
	 * @return The content manager used to load this static mesh.
	 */
	[[nodiscard]] TObjectPtr<UContentManager> GetContentManager() const;

	/**
	 * @brief Attempts to load static mesh data from a file.
	 *
	 * @param filePath The path to the file.
	 * @return The error encountered while loading the static mesh, otherwise nothing.
	 */
	[[nodiscard]] TErrorOr<void> LoadFromFile(const FString& filePath);

	/**
	 * @brief Loads static mesh data from an assimp scene.
	 *
	 * @param scene The scene.
	 * @return The error encountered while loading static mesh data from the scene, otherwise nothing.
	 */
	[[nodiscard]] TErrorOr<void> LoadFromScene(const struct aiScene* scene);
};