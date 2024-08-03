#include "Engine/Module.h"
#include "Engine/GameEngine.h"
#include "Engine/EditorEngine.h"

EModuleType IModule::GetModuleType() const
{
	UM_ASSERT_NOT_REACHED_MSG("GetModuleType not implemented for this module");
}

FVersion IModule::GetModuleVersion() const
{
	return FVersion { 1, 0, 0 };
}

TSubclassOf<UEngine> IEditorModule::GetEngineClass() const
{
	return UEditorEngine::StaticType();
}

EModuleType IEditorModule::GetModuleType() const
{
	return EModuleType::Editor;
}

TSubclassOf<UEngine> IGameModule::GetEngineClass() const
{
	return UGameEngine::StaticType();
}

EModuleType IGameModule::GetModuleType() const
{
	return EModuleType::Game;
}