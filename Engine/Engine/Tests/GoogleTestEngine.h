#pragma once

#include "Engine/Engine.h"
#include "GoogleTestEngine.Generated.h"

/**
 * @brief Defines the engine used by the Google Test application.
 */
UM_CLASS()
class UGoogleTestEngine : public UEngine
{
	UM_GENERATED_BODY();

public:

	/** @inheritdoc */
	virtual bool IsHeadless() const override;
};