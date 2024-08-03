#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsResource.h"

TObjectPtr<UGraphicsDevice> UGraphicsResource::GetGraphicsDevice() const
{
	return FindAncestorOfType<UGraphicsDevice>();
}

void UGraphicsResource::SetActiveContext() const
{
	const TObjectPtr<UGraphicsDevice> graphicsDevice = GetGraphicsDevice();
	if (UM_ENSURE(graphicsDevice.IsValid()))
	{
		UM_ENSURE(graphicsDevice->SetActiveContext() == EContextState::Available);
	}
}

UGraphicsResource::EContextState UGraphicsResource::SetActiveContextIfPossible() const
{
	const TObjectPtr<UGraphicsDevice> graphicsDevice = GetGraphicsDevice();
	if (graphicsDevice.IsValid())
	{
		return graphicsDevice->SetActiveContext();
	}

	return EContextState::Unavailable;
}