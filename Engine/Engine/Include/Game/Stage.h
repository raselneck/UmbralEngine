#pragma once

#include "Containers/Array.h"
#include "Object/Object.h"
#include "Stage.Generated.h"

class UScene;

UM_CLASS()
class UStage : public UObject
{
	UM_GENERATED_BODY();

public:

private:

	UM_PROPERTY()
	TArray<TObjectPtr<UScene>> m_Scenes;
};