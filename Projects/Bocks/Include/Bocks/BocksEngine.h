#pragma once

#include "Engine/GameEngine.h"
#include "BocksEngine.Generated.h"

class UEngineViewport;

UM_CLASS()
class UBocksEngine : public UGameEngine
{
	UM_GENERATED_BODY();

public:

	/** @copydoc UEngine::GetViewportClass */
	[[nodiscard]] virtual TSubclassOf<UEngineViewport> GetViewportClass() const override;
};