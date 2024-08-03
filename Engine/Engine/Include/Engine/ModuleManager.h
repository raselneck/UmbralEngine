#pragma once

#include "Containers/StringView.h"
#include "Engine/Module.h"
#include "Misc/Badge.h"

/**
 * @brief Defines a way to load, get, and query modules.
 */
class FModuleManager final
{
public:

	/**
	 * @brief Gets the module with the given name.
	 *
	 * @param moduleName The name of the module.
	 * @return The module if it was already loaded, otherwise nullptr.
	 */
	[[nodiscard]] static IModule* Get(FStringView moduleName);

	/**
	 * @brief Gets the currently executing module.
	 *
	 * @return The currently executing module.
	 */
	[[nodiscard]] static IModule* GetCurrentModule();

	/**
	 * @brief Gets the name of the currently executing module.
	 *
	 * @return The name of the currently executing module.
	 */
	[[nodiscard]] static FStringView GetCurrentModuleName();

	/**
	 * @brief Initializes the module manager.
	 */
	static void Initialize(TBadge<class FEngineInitializer>);

	/**
	 * @brief Loads the module with the given name, or returns it if it is already loaded.
	 *
	 * @param moduleName The name of the module.
	 * @return The loaded module, or nullptr if the module could not be found.
	 */
	[[nodiscard]] static IModule* Load(FStringView moduleName);

	/**
	 * @brief Checks to see if a module with the given name is loaded.
	 *
	 * @param moduleName The name of the module.
	 * @return True if the module is loaded, otherwise false.
	 */
	[[nodiscard]] static bool IsLoaded(FStringView moduleName);

	/**
	 * @brief Attempts to reload a module with the given name. If the module was not already loaded, this will load it.
	 *
	 * @param moduleName The name of the module.
	 * @return The module if it was reloaded, otherwise nullptr.
	 */
	[[nodiscard]] static IModule* Reload(FStringView moduleName);

	/**
	 * @brief Shuts down the module manager.
	 */
	static void Shutdown(TBadge<class FEngineInitializer>);
};