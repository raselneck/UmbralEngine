#pragma once

#include "Graphics/GraphicsResource.h"
#include "StaticMesh.Generated.h"

class UContentManager;
class UIndexBuffer;
class UVertexBuffer;

/**
 * @brief Defines a static mesh.
 */
UM_CLASS(ChildOf=UContentManager)
class UStaticMesh final : public UGraphicsResource
{
	friend class UContentManager;

	UM_GENERATED_BODY();

public:

	// TODO Make both of these getters return TObjectPtr<const ...>

	/**
	 * @brief Gets this mesh's index buffer.
	 *
	 * @return This mesh's index buffer.
	 */
	[[nodiscard]] TObjectPtr<UIndexBuffer> GetIndexBuffer() const
	{
		return m_IndexBuffer;
	}

	/**
	 * @brief Gets this mesh's vertex buffer.
	 *
	 * @return This mesh's vertex buffer.
	 */
	[[nodiscard]] TObjectPtr<UVertexBuffer> GetVertexBuffer() const
	{
		return m_VertexBuffer;
	}

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
	 * @brief Loads static mesh data from memory.
	 *
	 * @param bytes The mesh bytes.
	 * @param numBytes The number of mesh bytes.
	 * @param fileName The name of the mesh file, used as a hint to determine the type of importer to use.
	 * @return The error encountered while loading the static mesh, otherwise nothing.
	 */
	[[nodiscard]] TErrorOr<void> LoadFromMemory(const void* bytes, int32 numBytes, FStringView fileName);

	/**
	 * @brief Loads static mesh data from an assimp scene.
	 *
	 * @param scene The scene.
	 * @param fileName The name of the file that the scene was loaded from.
	 * @return The error encountered while loading static mesh data from the scene, otherwise nothing.
	 */
	[[nodiscard]] TErrorOr<void> LoadFromScene(const struct aiScene* scene, FStringView fileName);

	UM_PROPERTY()
	TObjectPtr<UVertexBuffer> m_VertexBuffer;

	UM_PROPERTY()
	TObjectPtr<UIndexBuffer> m_IndexBuffer;
};