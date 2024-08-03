#pragma once

#include "Engine/Engine.h"
#include "EditorEngine.Generated.h"

/**
 * @brief Defines the base and default implementation of editor engines.
 */
UM_CLASS()
class UEditorEngine : public UEngine
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Destroys this editor engine.
	 */
	virtual ~UEditorEngine() override;
};