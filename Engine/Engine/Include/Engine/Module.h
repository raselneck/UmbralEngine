#pragma once

#include "Containers/Optional.h"
#include "Containers/StringView.h"
#include "Engine/Assert.h"
#include "Engine/Engine.h"
#include "Misc/Version.h"
#include "Object/ObjectPtr.h"
#include "Templates/IsConstructible.h"

/**
 * @brief An enumeration of module types.
 */
enum class EModuleType : int32
{
	App,
	AppLibrary,
	Game,
	GameLibrary,
	Editor,
	EditorLibrary
};

/**
 * @brief Defines the interface to be implemented by all modules.
 */
class IModule
{
	friend class FModuleManager;

public:

	/**
	 * @brief Destroys this module.
	 */
	virtual ~IModule() = default;

	/**
	 * @brief Gets this module's type.
	 *
	 * @return This module's type.
	 */
	[[nodiscard]] virtual EModuleType GetModuleType() const;

	/**
	 * @brief Gets this module's version.
	 *
	 * @return This module's version.
	 */
	[[nodiscard]] virtual FVersion GetModuleVersion() const;

protected:

	/**
	 * @brief Called when this module starts up.
	 */
	virtual void StartupModule() { }

	/**
	 * @brief Called when this module is shut down, allowing it to free any necessary resources.
	 */
	virtual void ShutdownModule() { }
};

#define UMBRAL_IMPLEMENT_COMMON_MODULE_FUNCTIONS(ModuleClass, ModuleName)               \
	static_assert(TIsDefaultConstructible<ModuleClass>::Value, "Module class must be default constructible"); \
	static TOptional<ModuleClass> Umbral_ModuleInstance;                                \
	static constexpr FStringView Umbral_ModuleName = UM_STRINGIFY_AS_VIEW(ModuleName);  \
	extern "C" void Umbral_OnModuleLoaded()                                             \
	{                                                                                   \
		(void)Umbral_ModuleInstance.EmplaceValue();                                     \
	}                                                                                   \
	extern "C" void Umbral_OnModuleUnloaded()                                           \
	{                                                                                   \
		Umbral_ModuleInstance.Reset();                                                  \
	}                                                                                   \
	extern "C" const char* Umbral_GetModuleName()                                       \
	{                                                                                   \
		return Umbral_ModuleName.GetChars();                                            \
	}                                                                                   \
	extern "C" IModule* Umbral_GetModule()                                              \
	{                                                                                   \
		return Umbral_ModuleInstance.GetValueAsPointer();                               \
	}

/**
 * @brief Defines the base for engine modules.
 */
class IEngineModule : public IModule
{
public:

	/**
	 * @brief Destroys this engine module.
	 */
	virtual ~IEngineModule() override = default;

	/**
	 * @brief Gets this module's desired engine class.
	 *
	 * @return This module's desired engine class.
	 */
	[[nodiscard]] virtual TSubclassOf<UEngine> GetEngineClass() const = 0;
};

/**
 * @brief Defines the base for engine modules.
 */
class IEditorModule : public IEngineModule
{
public:

	/**
	 * @brief Destroys this editor module.
	 */
	~IEditorModule() override = default;

	/** @copydoc IModule::GetEngineClass */
	[[nodiscard]] virtual TSubclassOf<UEngine> GetEngineClass() const override;

	/** @copydoc IModule::GetModuleType */
	[[nodiscard]] virtual EModuleType GetModuleType() const final;
};

#define IMPLEMENT_EDITOR_MODULE(ModuleClass, ModuleName)                        \
	UMBRAL_IMPLEMENT_COMMON_MODULE_FUNCTIONS(ModuleClass, ModuleName)           \
	extern "C" EModuleType Umbral_GetModuleType()                               \
	{                                                                           \
		return EModuleType::Editor;                                             \
	}

/**
 * @brief Defines the base for game modules.
 */
class IGameModule : public IEngineModule
{
public:

	/**
	 * @brief Destroys this game module.
	 */
	~IGameModule() override = default;

	/** @copydoc IModule::GetEngineClass */
	[[nodiscard]] virtual TSubclassOf<UEngine> GetEngineClass() const override;

	/** @copydoc IModule::GetModuleType */
	[[nodiscard]] virtual EModuleType GetModuleType() const final;
};

#define IMPLEMENT_GAME_MODULE(ModuleClass, ModuleName)                          \
	UMBRAL_IMPLEMENT_COMMON_MODULE_FUNCTIONS(ModuleClass, ModuleName)           \
	extern "C" EModuleType Umbral_GetModuleType()                               \
	{                                                                           \
		return EModuleType::Game;                                               \
	}

// NOTE This comment is necessary to appease the "backslash-newline at end of file" erroneous error with GCC