#include "Engine/Application.h"
#include "Engine/Engine.h"
#include "Engine/EngineViewport.h"
#include "Engine/EngineWindow.h"
#include "Graphics/GraphicsDevice.h"

UEngine::~UEngine() = default;

void UEngine::BeginRun(TBadge<UEngineLoop>)
{
}

TObjectPtr<UEngineViewport> UEngine::CreateViewport(const FString& title, int32 width, int32 height, TSubclassOf<UEngineViewport> viewportClass)
{
	const TObjectPtr<UApplication> application = FindAncestorOfType<UApplication>();
	const IApplicationRenderingContext* primaryRenderingContext = application->GetRenderingContext(0);
	const TObjectPtr<UGraphicsDevice> primaryGraphicsDevice = primaryRenderingContext->GetGraphicsDevice();
	const EGraphicsApi graphicsApi = primaryGraphicsDevice->GetApi();

	return application->CreateWindowAndViewport(title, graphicsApi, width, height, viewportClass);
}

void UEngine::EndRun(TBadge<UEngineLoop>)
{
}

TSubclassOf<UEngineViewport> UEngine::GetViewportClass() const
{
	return UEngineViewport::StaticType();
}

bool UEngine::IsHeadless() const
{
	return false;
}