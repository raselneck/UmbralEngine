#include "Engine/ContentManager.h"
#include "Engine/Logging.h"
#include "Graphics/StaticMesh.h"
#include "HAL/File.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

TObjectPtr<UContentManager> UStaticMesh::GetContentManager() const
{
	return FindAncestorOfType<UContentManager>();
}

TErrorOr<void> UStaticMesh::LoadFromFile(const FString& filePath)
{
	TErrorOr<TArray<uint8>> fileLoadResult = FFile::ReadAllBytes(filePath);
	if (fileLoadResult.IsError())
	{
		return fileLoadResult.ReleaseError();
	}

	const TArray<uint8> fileBytes = fileLoadResult.ReleaseValue();
	constexpr uint32 importFlags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;

	Assimp::Importer importer;
	const aiScene* importedScene = importer.ReadFileFromMemory(fileBytes.GetData(), static_cast<size_t>(fileBytes.Num()), importFlags, filePath.GetChars());
	if (importedScene == nullptr)
	{
		return MAKE_ERROR("{}", importer.GetErrorString());
	}

	if (importedScene->mNumMeshes == 0)
	{
		return MAKE_ERROR("Model contains no meshes");
	}
	if (importedScene->mNumMeshes > 1)
	{
		UM_LOG(Warning, "Found {} meshes in file \"{}\"; importing first as static mesh", importedScene->mNumMeshes, filePath);
	}
	if (importedScene->mNumAnimations > 0)
	{
		UM_LOG(Warning, "Found {} animations in file \"{}\" being imported as static mesh", importedScene->mNumAnimations, filePath);
	}

	return LoadFromScene(importedScene);
}

TErrorOr<void> UStaticMesh::LoadFromScene(const struct aiScene* scene)
{
	return MAKE_ERROR("Parsing assimp scenes is not yet implemented");
}