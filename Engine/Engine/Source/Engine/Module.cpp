#include "Engine/Module.h"
#include "Engine/GameEngine.h"
#include "Engine/EditorEngine.h"

#if WITH_MODULE_EMITTED_TYPES
/**
 * @brief Gets the pointer to the module's types array.
 *
 * @return The pointer to the module's types array.
 */
extern "C" const FTypeInfo** Umbral_GetModuleTypes();

/**
 * @brief Gets the number of module types.
 *
 * @return The number of module types.
 */
extern "C" int32 Umbral_GetNumModuleTypes();
#endif

EModuleType IModule::GetModuleType() const
{
	UM_ASSERT_NOT_REACHED_MSG("GetModuleType not implemented for this module");
}

TSpan<const FTypeInfo*> IModule::GetModuleTypes() const
{
#if WITH_MODULE_EMITTED_TYPES
	return TSpan<const FTypeInfo*> { Umbral_GetModuleTypes(), Umbral_GetNumModuleTypes() };
#else
	return { };
#endif
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