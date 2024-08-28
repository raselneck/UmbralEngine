#pragma once

#include "Object/Object.h"
#include "ContentManager.Generated.h"

class UGraphicsDevice;
class UStaticMesh;

namespace Private
{
	template<typename AssetType>
	struct FContentManagerLoadDispatcher;
}

/**
 * @brief Defines a manager for loading content.
 */
UM_CLASS(ChildOf=UGraphicsDevice)
class UContentManager : public UObject
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Loads an asset from a path relative to the content folder.
	 *
	 * @tparam AssetType The asset type.
	 * @param assetPath The path to the asset.
	 * @return The loaded asset.
	 */
	template<typename AssetType>
	[[nodiscard]] TObjectPtr<AssetType> Load(FStringView assetPath) const
	{
		return Private::FContentManagerLoadDispatcher<AssetType>::Load(this, assetPath);
	}

	/**
	 * @brief Loads a static mesh from a file.
	 *
	 * @param assetPath The path to the static mesh relative to the content directory.
	 * @return The loaded static mesh.
	 */
	[[nodiscard]] TObjectPtr<UStaticMesh> LoadStaticMesh(FStringView assetPath) const;

private:

	/**
	 * @brief Gets the graphics device associated with this content manager.
	 *
	 * @return The graphics device associated with this content manager.
	 */
	[[nodiscard]] TObjectPtr<UGraphicsDevice> GetGraphicsDevice() const;
};

namespace Private
{
#define IMPLEMENT_CONTENT_LOAD_DISPATCH(TypeName, FunctionName) \
	template<> \
	struct FContentManagerLoadDispatcher<TypeName> \
	{ \
		[[nodiscard]] static TObjectPtr<TypeName> Load(UContentManager* contentManager, const FStringView assetPath) \
		{ \
			return contentManager->Load##FunctionName(assetPath); \
		} \
	}

	IMPLEMENT_CONTENT_LOAD_DISPATCH(UStaticMesh, StaticMesh);

#undef IMPLEMENT_CONTENT_LOAD_DISPATCH
}