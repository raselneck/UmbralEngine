#pragma once

#include "Containers/Array.h"
#include "Object/Object.h"
#include "Scene.Generated.h"

class AActor;

UM_CLASS()
class UScene : public UObject
{
	UM_GENERATED_BODY();

public:

private:

	UM_PROPERTY()
	TArray<TObjectPtr<AActor>> m_Actors;
};