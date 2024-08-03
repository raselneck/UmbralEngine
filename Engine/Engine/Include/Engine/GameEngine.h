#pragma once

#include "Engine/Engine.h"
#include "Engine/GameTime.h"
#include "GameEngine.Generated.h"

/**
 * @brief Defines the base and default implementation of game engines.
 */
UM_CLASS()
class UGameEngine : public UEngine
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Destroys this game engine.
	 */
	virtual ~UGameEngine() override;
};