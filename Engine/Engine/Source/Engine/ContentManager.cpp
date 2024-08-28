#include "Engine/ContentManager.h"
#include "Engine/Logging.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/StaticMesh.h"
#include "HAL/Directory.h"
#include "HAL/Path.h"

TObjectPtr<UStaticMesh> UContentManager::LoadStaticMesh(const FStringView assetPath) const
{
	const FString contentDir = FDirectory::GetContentDir();
	const FString fullAssetPath = FPath::Join(contentDir, assetPath);

	TObjectPtr<UStaticMesh> staticMesh = MakeObject<UStaticMesh>(GetGraphicsDevice());
	if (TErrorOr<void> loadResult = staticMesh->LoadFromFile({}, fullAssetPath);
	    loadResult.IsError())
	{
		UM_LOG(Error, "Failed to load static mesh \"{}\". Reason: {}", fullAssetPath, loadResult.GetError().GetMessage());
		return nullptr;
	}

	return staticMesh;
}

TObjectPtr<UGraphicsDevice> UContentManager::GetGraphicsDevice() const
{
	return GetTypedParent<UGraphicsDevice>();
}