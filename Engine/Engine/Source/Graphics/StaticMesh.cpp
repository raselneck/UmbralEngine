#include "Containers/Function.h"
#include "Containers/HashMap.h"
#include "Engine/ContentManager.h"
#include "Engine/Logging.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/Vertex.h"
#include "Graphics/VertexBuffer.h"
#include "HAL/File.h"
#include "HAL/Path.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

// TODO Incorporate the mesh optimizer library https://github.com/zeux/meshoptimizer
//      See here for how to do so https://github.com/emeiri/ogldev/blob/master/Common/ogldev_basic_mesh.cpp#L248

/**
 * @brief Converts an assimp vector to a packed color.
 *
 * @tparam T The assimp color's component type.
 * @param value The assimp color.
 * @return The packed color.
 */
template<typename T>
inline FColor AssimpColorToColor(const aiColor4t<T>& value)
{
	return FColor
	{
		Private::NormalizedFloatToByte(value.r),
		Private::NormalizedFloatToByte(value.g),
		Private::NormalizedFloatToByte(value.b),
		Private::NormalizedFloatToByte(value.a)
	};
}

/**
 * @brief Converts an assimp vector to a 2-dimensional vector.
 *
 * @tparam T The assimp vector's component type.
 * @param value The assimp vector.
 * @return The 2-dimensional vector.
 */
template<typename T>
inline FVector2 AssimpVectorToVector2(const aiVector3t<T>& value)
{
	return FVector2
	{
		static_cast<float>(value.x),
		static_cast<float>(value.y)
	};
}

/**
 * @brief Converts an assimp vector to a 3-dimensional vector.
 *
 * @tparam T The assimp vector's component type.
 * @param value The assimp vector.
 * @return The 3-dimensional vector.
 */
template<typename T>
inline FVector3 AssimpVectorToVector3(const aiVector3t<T>& value)
{
	return FVector3
	{
		static_cast<float>(value.x),
		static_cast<float>(value.y),
		static_cast<float>(value.z),
	};
}

template<bool HasColors, bool HasNormals, bool HasTexCoords>
struct TVertexDataParser;

template<>
struct TVertexDataParser<true, true, true>
{
	// TODO FVertexPositionColorNormalTexture or similar name

	static void PopulateBufferFromMesh(UVertexBuffer* vertexBuffer, const aiMesh* mesh)
	{
		(void)vertexBuffer;
		(void)mesh;
	}
};

template<>
struct TVertexDataParser<true, true, false>
{
	using VertexType = FVertexPositionColorNormal;

	static void PopulateBufferFromMesh(UVertexBuffer* vertexBuffer, const aiMesh* mesh)
	{
		TArray<VertexType> vertices;
		vertices.Reserve(static_cast<int32>(mesh->mNumVertices));

		for (uint32 idx = 0; idx < mesh->mNumVertices; ++idx)
		{
			VertexType& vertex = vertices.AddDefaultGetRef();
			vertex.Position = AssimpVectorToVector3(mesh->mVertices[idx]);
			vertex.Normal = AssimpVectorToVector3(mesh->mNormals[idx]);
			vertex.Color = AssimpColorToColor(mesh->mColors[0][idx]);
		}

		vertexBuffer->SetData(vertices);
	}
};

template<>
struct TVertexDataParser<true, false, true>
{
	using VertexType = FVertexPositionColorTexture;

	static void PopulateBufferFromMesh(UVertexBuffer* vertexBuffer, const aiMesh* mesh)
	{
		TArray<VertexType> vertices;
		vertices.Reserve(static_cast<int32>(mesh->mNumVertices));

		for (uint32 idx = 0; idx < mesh->mNumVertices; ++idx)
		{
			VertexType& vertex = vertices.AddDefaultGetRef();
			vertex.Position = AssimpVectorToVector3(mesh->mVertices[idx]);
			vertex.UV = AssimpVectorToVector2(mesh->mTextureCoords[0][idx]);
			vertex.Color = AssimpColorToColor(mesh->mColors[0][idx]);
		}

		vertexBuffer->SetData(vertices);
	}
};

template<>
struct TVertexDataParser<true, false, false>
{
	using VertexType = FVertexPositionColor;

	static void PopulateBufferFromMesh(UVertexBuffer* vertexBuffer, const aiMesh* mesh)
	{
		TArray<VertexType> vertices;
		vertices.Reserve(static_cast<int32>(mesh->mNumVertices));

		for (uint32 idx = 0; idx < mesh->mNumVertices; ++idx)
		{
			VertexType& vertex = vertices.AddDefaultGetRef();
			vertex.Position = AssimpVectorToVector3(mesh->mVertices[idx]);
			vertex.Color = AssimpColorToColor(mesh->mColors[0][idx]);
		}

		vertexBuffer->SetData(vertices);
	}
};

template<>
struct TVertexDataParser<false, true, true>
{
	using VertexType = FVertexPositionNormalTexture;

	static void PopulateBufferFromMesh(UVertexBuffer* vertexBuffer, const aiMesh* mesh)
	{
		TArray<VertexType> vertices;
		vertices.Reserve(static_cast<int32>(mesh->mNumVertices));

		for (uint32 idx = 0; idx < mesh->mNumVertices; ++idx)
		{
			VertexType& vertex = vertices.AddDefaultGetRef();
			vertex.Position = AssimpVectorToVector3(mesh->mVertices[idx]);
			vertex.Normal = AssimpVectorToVector3(mesh->mNormals[idx]);
			vertex.UV = AssimpVectorToVector2(mesh->mTextureCoords[0][idx]);
		}

		vertexBuffer->SetData(vertices);
	}
};

template<>
struct TVertexDataParser<false, true, false>
{
	using VertexType = FVertexPositionNormal;

	static void PopulateBufferFromMesh(UVertexBuffer* vertexBuffer, const aiMesh* mesh)
	{
		TArray<VertexType> vertices;
		vertices.Reserve(static_cast<int32>(mesh->mNumVertices));

		for (uint32 idx = 0; idx < mesh->mNumVertices; ++idx)
		{
			VertexType& vertex = vertices.AddDefaultGetRef();
			vertex.Position = AssimpVectorToVector3(mesh->mVertices[idx]);
			vertex.Normal = AssimpVectorToVector3(mesh->mNormals[idx]);
		}

		vertexBuffer->SetData(vertices);
	}
};

template<>
struct TVertexDataParser<false, false, true>
{
	using VertexType = FVertexPositionTexture;

	static void PopulateBufferFromMesh(UVertexBuffer* vertexBuffer, const aiMesh* mesh)
	{
		TArray<VertexType> vertices;
		vertices.Reserve(static_cast<int32>(mesh->mNumVertices));

		for (uint32 idx = 0; idx < mesh->mNumVertices; ++idx)
		{
			VertexType& vertex = vertices.AddDefaultGetRef();
			vertex.Position = AssimpVectorToVector3(mesh->mVertices[idx]);
			vertex.UV = AssimpVectorToVector2(mesh->mTextureCoords[0][idx]);
		}

		vertexBuffer->SetData(vertices);
	}
};

template<>
struct TVertexDataParser<false, false, false>
{
	using VertexType = FVertexPosition;

	static void PopulateBufferFromMesh(UVertexBuffer* vertexBuffer, const aiMesh* mesh)
	{
		TArray<VertexType> vertices;
		vertices.Reserve(static_cast<int32>(mesh->mNumVertices));

		for (uint32 idx = 0; idx < mesh->mNumVertices; ++idx)
		{
			VertexType& vertex = vertices.AddDefaultGetRef();
			vertex.Position = AssimpVectorToVector3(mesh->mVertices[idx]);
		}

		vertexBuffer->SetData(vertices);
	}
};

struct FVertexStats
{
	bool HasColors = false;
	bool HasNormals = false;
	bool HasTextureCoords = false;

	auto operator<=>(const FVertexStats&) const = default;

	friend uint64 GetHashCode(const FVertexStats& stats)
	{
		return HashItems(stats.HasColors, stats.HasNormals, stats.HasTextureCoords);
	}
};

using FPopulateVertexBufferFunction = void(*)(UVertexBuffer*, const aiMesh*);

/**
 * @brief Populates an index buffer with face data from an assimp mesh.
 *
 * @tparam IndexType The index type.
 * @param indexBuffer The index buffer.
 * @param mesh The mesh.
 */
template<typename IndexType>
static void PopulateIndexBufferWithFaceData(UIndexBuffer* indexBuffer, const aiMesh* mesh)
{
	TArray<IndexType> indices;
	indices.Reserve(mesh->mNumFaces * 3);

	for (uint32 idx = 0; idx < mesh->mNumFaces; ++idx)
	{
		const aiFace& face = mesh->mFaces[idx];
		indices.Add(face.mIndices[0]);
		indices.Add(face.mIndices[1]);
		indices.Add(face.mIndices[2]);
	}

	indexBuffer->SetData(indices);
}

/**
 * @brief Populates a vertex and index buffer with the necessary data from an assimp mesh.
 *
 * @param vertexBuffer The vertex buffer.
 * @param indexBuffer The index buffer.
 * @param mesh The mesh.
 * @return Any error encountered while populating the buffers.
 */
static TErrorOr<void> PopulateBuffersWithMeshData(UVertexBuffer* vertexBuffer, UIndexBuffer* indexBuffer, const aiMesh* mesh)
{
	if (mesh->mNumVertices > TNumericLimits<int32>::MaxValue)
	{
		return MAKE_ERROR("Mesh contains too many vertices (max is {})", TNumericLimits<int32>::MaxValue);
	}

	// TODO There's GOT to be a more sane way to support all of the different combinations of mesh data...

#define REGISTER_MESH_POPULATE_FUNCTION(HasColors, HasNormals, HasTextureCoords) \
	{ FVertexStats {HasColors, HasNormals, HasTextureCoords}, TVertexDataParser<HasColors, HasNormals, HasTextureCoords>::PopulateBufferFromMesh }

	static THashMap<FVertexStats, FPopulateVertexBufferFunction> populateVertexBufferFunctionMap
	{
		REGISTER_MESH_POPULATE_FUNCTION(true,  true,  true),
		REGISTER_MESH_POPULATE_FUNCTION(true,  true,  false),
		REGISTER_MESH_POPULATE_FUNCTION(true,  false, true),
		REGISTER_MESH_POPULATE_FUNCTION(true,  false, false),
		REGISTER_MESH_POPULATE_FUNCTION(false, true,  true),
		REGISTER_MESH_POPULATE_FUNCTION(false, true,  false),
		REGISTER_MESH_POPULATE_FUNCTION(false, false, true),
		REGISTER_MESH_POPULATE_FUNCTION(false, false, false)
	};

#undef REGISTER_MESH_POPULATE_FUNCTION

	FVertexStats meshStats;
	meshStats.HasColors = mesh->HasVertexColors(0);
	meshStats.HasNormals = mesh->mNormals != nullptr;
	meshStats.HasTextureCoords = mesh->HasTextureCoords(0); // TODO Support meshes that use multiple texture coordinates

	const FPopulateVertexBufferFunction populateFunction = populateVertexBufferFunctionMap[meshStats];
	populateFunction(vertexBuffer, mesh);

	// Populate the index buffer, but attempt to use as little memory as possible
	if (mesh->mNumVertices <= TNumericLimits<uint8>::MaxValue)
	{
		PopulateIndexBufferWithFaceData<uint8>(indexBuffer, mesh);
	}
	else if (mesh->mNumVertices <= TNumericLimits<uint16>::MaxValue)
	{
		PopulateIndexBufferWithFaceData<uint16>(indexBuffer, mesh);
	}
	else
	{
		PopulateIndexBufferWithFaceData<uint32>(indexBuffer, mesh);
	}

	return {};
}

TObjectPtr<UContentManager> UStaticMesh::GetContentManager() const
{
	return FindAncestorOfType<UContentManager>();
}

TErrorOr<void> UStaticMesh::LoadFromFile(const FString& filePath)
{
	TErrorOr<TArray<uint8>> fileLoadResult = FFile::ReadBytes(filePath);
	if (fileLoadResult.IsError())
	{
		return fileLoadResult.ReleaseError();
	}

	const TArray<uint8> fileBytes = fileLoadResult.ReleaseValue();
	return LoadFromMemory(fileBytes.GetData(), fileBytes.Num(), FPath::GetFileNameAsView(filePath));
}

TErrorOr<void> UStaticMesh::LoadFromMemory(const void* bytes, const int32 numBytes, const FStringView fileName)
{
	if (bytes == nullptr || numBytes <= 0)
	{
		return MAKE_ERROR("Invalid mesh memory given (bytes={}, numBytes={})", bytes, numBytes);
	}
	if (fileName.IsEmpty())
	{
		return MAKE_ERROR("No file name given as hint for mesh loader");
	}

	// TODO Make some of the import flags configurable
	Assimp::Importer importer;
	constexpr uint32 importFlags = aiProcess_Triangulate | aiProcess_GenSmoothNormals /*| aiProcess_FlipUVs*/ | aiProcess_JoinIdenticalVertices;
	const FStringView fileHint = FPath::GetTemporaryNullTerminatedStringView(fileName);
	const aiScene* importedScene = importer.ReadFileFromMemory(bytes, static_cast<size_t>(numBytes), importFlags, fileHint.GetChars());
	if (importedScene == nullptr)
	{
		return MAKE_ERROR("{}", importer.GetErrorString());
	}

	return LoadFromScene(importedScene, fileHint);
}

TErrorOr<void> UStaticMesh::LoadFromScene(const struct aiScene* scene, const FStringView fileName)
{
	if (scene->mNumMeshes == 0)
	{
		return MAKE_ERROR("Found no meshes in file \"{}\"", fileName);
	}
	if (scene->mNumMeshes > 1)
	{
		UM_LOG(Warning, "Found {} meshes in file \"{}\"; importing first as static mesh", scene->mNumMeshes, fileName);
	}
	if (scene->mNumAnimations > 0)
	{
		UM_LOG(Warning, "Found {} animations in file \"{}\" being imported as static mesh", scene->mNumAnimations, fileName);
	}

	const aiMesh* mesh = scene->mMeshes[0];
	if (mesh->mNumVertices == 0 || mesh->mVertices == nullptr)
	{
		return MAKE_ERROR("Found no vertices in file \"{}\"", fileName);
	}

	TObjectPtr<UGraphicsDevice> graphicsDevice = GetGraphicsDevice();
	m_VertexBuffer = graphicsDevice->CreateVertexBuffer(EVertexBufferUsage::Static);
	m_IndexBuffer = graphicsDevice->CreateIndexBuffer(EIndexBufferUsage::Static);

	return PopulateBuffersWithMeshData(m_VertexBuffer.GetObject(), m_IndexBuffer.GetObject(), mesh);
}