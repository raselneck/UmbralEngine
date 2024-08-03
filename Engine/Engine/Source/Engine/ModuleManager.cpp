#include "Containers/Array.h"
#include "Engine/Assert.h"
#include "Engine/ModuleManager.h"
#include "Engine/Logging.h"
#include "Engine/Platform.h"
#include <SDL.h>

/**
 * @brief Notifies the current module that it has been loaded.
 */
extern "C" void Umbral_OnModuleLoaded();

/**
 * @brief Notifies the current module that it has been unloaded.
 */
extern "C" void Umbral_OnModuleUnloaded();

/**
 * @brief Gets the current module.
 */
extern "C" IModule* Umbral_GetModule();

/**
 * @brief Gets the current module's name.
 *
 * @return The current module's name.
 */
extern "C" const char* Umbral_GetModuleName();

/**
 * @brief Gets the current module's type.
 *
 * @return The current module's type.
 */
extern "C" EModuleType Umbral_GetModuleType();

using FOnModuleLoadedCallback = void(*)();
using FOnModuleUnloadedCallback = void(*)();
using FGetModuleCallback = IModule*(*)();
using FGetModuleNameCallback = const char*(*)();
using FGetModuleTypeCallback = EModuleType(*)();

/**
 * @brief Gets the path to a module based on its name.
 *
 * @param moduleName The module name.
 * @return The path to the module.
 */
static FString GetModulePathFromName(const FStringView moduleName)
{
#if UMBRAL_PLATFORM_IS_WINDOWS
	constexpr FStringView moduleExtension = "dll"_sv;
#elif UMBRAL_PLATFORM_IS_MAC
	constexpr FStringView moduleExtension = "so"_sv;
#else
	constexpr FStringView moduleExtension = "a"_sv;
#endif

	return FString::Format("{}.{}"_sv, moduleName, moduleExtension);
}

/**
 * @brief Defines a handle for a loaded module.
 */
class FModuleHandle final
{
public:

	/**
	 * @brief Checks to see if this module handle is valid.
	 *
	 * @return True if this module handle is valid, otherwise false.
	 */
	[[nodiscard]] bool IsValid() const
	{
		return m_ObjectHandle != nullptr &&
		       m_OnModuleLoaded != nullptr &&
		       m_OnModuleUnloaded != nullptr &&
		       m_GetModule != nullptr &&
		       m_GetModuleName != nullptr &&
		       m_GetModuleType != nullptr &&
		       m_ModuleName.Length() > 0 &&
		       m_ModuleType != static_cast<EModuleType>(-1);
	}

	/**
	 * @brief Attempts to load a module handle.
	 *
	 * @param moduleName The name of the module.
	 * @return The module handle.
	 */
	[[nodiscard]] bool Load(const FStringView moduleName)
	{
		if (IsValid())
		{
			return true;
		}

		const FString modulePath = GetModulePathFromName(moduleName);
		// TODO Verify that the file exists?

		m_ObjectHandle = SDL_LoadObject(modulePath.GetChars());
		if (m_ObjectHandle == nullptr)
		{
			UM_LOG(Error, "Failed to load module named {} (reason: {})", moduleName, SDL_GetError());
			return false;
		}

		LoadModuleFunctions();
		NotifyModuleLoaded();

		return IsValid();
	}

	/**
	 * @brief Gets the module.
	 *
	 * @return The module.
	 */
	[[nodiscard]] IModule* GetModule() const
	{
		if (m_GetModule == nullptr)
		{
			return nullptr;
		}

		return m_GetModule();
	}

	/**
	 * @brief Gets the module's name.
	 *
	 * @return The module's name.
	 */
	[[nodiscard]] FStringView GetModuleName() const
	{
		return m_ModuleName;
	}

	/**
	 * @brief Gets the module's type.
	 *
	 * @return The module's type.
	 */
	[[nodiscard]] EModuleType GetModuleType() const
	{
		return m_ModuleType;
	}

	/**
	 * @brief Unloads this module.
	 */
	void Unload()
	{
		NotifyModuleUnloaded();

		m_OnModuleLoaded = nullptr;
		m_OnModuleUnloaded = nullptr;
		m_GetModuleName = nullptr;
		m_GetModuleType = nullptr;
		m_ModuleName = {};
		m_ModuleType = static_cast<EModuleType>(-1);

		if (m_ObjectHandle != nullptr)
		{
			SDL_UnloadObject(m_ObjectHandle);
			m_ObjectHandle = nullptr;
		}
	}

private:

	/**
	 * @brief Loads a function from the opened module.
	 *
	 * @tparam FunctionType The type of the function.
	 * @param functionName The function name.
	 * @return The function in the module, or nullptr if the function was not found.
	 */
	template<typename FunctionType>
	[[nodiscard]] FunctionType LoadFunction(const FStringView functionName) const
	{
		return reinterpret_cast<FunctionType>(SDL_LoadFunction(m_ObjectHandle, functionName.GetChars()));
	}

	/**
	 * @brief Loads all module functions.
	 */
	void LoadModuleFunctions()
	{
		m_OnModuleLoaded = LoadFunction<FOnModuleLoadedCallback>(nameof(Umbral_OnModuleLoaded));
		m_OnModuleUnloaded = LoadFunction<FOnModuleUnloadedCallback>(nameof(Umbral_OnModuleUnloaded));
		m_GetModule = LoadFunction<FGetModuleCallback>(nameof(Umbral_GetModule));
		m_GetModuleName = LoadFunction<FGetModuleNameCallback>(nameof(Umbral_GetModuleName));
		m_GetModuleType = LoadFunction<FGetModuleTypeCallback>(nameof(Umbral_GetModuleType));

		// Cache the module name
		if (m_GetModuleName != nullptr)
		{
			const char* moduleName = m_GetModuleName();
			const int32 moduleNameLength = TCharTraits<char>::GetNullTerminatedLength(moduleName);
			m_ModuleName = FStringView { moduleName, moduleNameLength };
		}

		// Cache the module type
		if (m_GetModuleType != nullptr)
		{
			m_ModuleType = m_GetModuleType();
		}
	}

	/**
	 * @brief Notified the module that it was loaded.
	 */
	void NotifyModuleLoaded()
	{
		if (m_OnModuleLoaded == nullptr)
		{
			return;
		}

		m_OnModuleLoaded();
	}

	/**
	 * @brief Notified the module that it was unloaded.
	 */
	void NotifyModuleUnloaded()
	{
		if (m_OnModuleUnloaded == nullptr)
		{
			return;
		}

		m_OnModuleUnloaded();
	}

	void* m_ObjectHandle = nullptr;
	FOnModuleLoadedCallback m_OnModuleLoaded = nullptr;
	FOnModuleUnloadedCallback m_OnModuleUnloaded = nullptr;
	FGetModuleCallback m_GetModule = nullptr;
	FGetModuleNameCallback m_GetModuleName = nullptr;
	FGetModuleTypeCallback m_GetModuleType = nullptr;
	FStringView m_ModuleName;
	EModuleType m_ModuleType = static_cast<EModuleType>(-1);
};

static TArray<FModuleHandle> GModules;

/**
 * @brief Finds a module handle and its index in the modules array by its name.
 *
 * @param moduleName The name of the module.
 * @param outHandle The module handle.
 * @param outIndex The module's index in the modules array.
 * @return True if a module with the name \p moduleName was found, otherwise false.
 */
static bool FindModuleHandleAndIndex(const FStringView moduleName, FModuleHandle*& outHandle, int32& outIndex)
{
	outHandle = nullptr;
	outIndex = INDEX_NONE;

	for (int32 idx = 0; idx < GModules.Num(); ++idx)
	{
		if (GModules[idx].GetModuleName() == moduleName)
		{
			outHandle = &GModules[idx];
			outIndex = idx;
			return true;
		}
	}

	return false;
}

IModule* FModuleManager::Get(const FStringView moduleName)
{
	FModuleHandle* moduleHandle = nullptr;
	int32 moduleIndex = INDEX_NONE;
	if (FindModuleHandleAndIndex(moduleName, moduleHandle, moduleIndex) == false)
	{
		return nullptr;
	}

	return moduleHandle->GetModule();
}

IModule* FModuleManager::GetCurrentModule()
{
	return Umbral_GetModule();
}

FStringView FModuleManager::GetCurrentModuleName()
{
	return FStringView { Umbral_GetModuleName() };
}

IModule* FModuleManager::Load(const FStringView moduleName)
{
	if (IModule* existingModule = Get(moduleName))
	{
		return existingModule;
	}

	FModuleHandle moduleHandle;
	if (moduleHandle.Load(moduleName) == false)
	{
		moduleHandle.Unload(); // Ensure the module is cleaned up

		UM_LOG(Error, "Failed to load module {}", moduleName);
		return nullptr;
	}

	IModule* module = moduleHandle.GetModule();
	GModules.Add(MoveTemp(moduleHandle));

	return module;
}

void FModuleManager::Initialize(TBadge<class FEngineInitializer>)
{
	UM_ASSERT(GetCurrentModule() == nullptr, "Attempting to initialize the module manager more than once");

	Umbral_OnModuleLoaded();

	UM_ASSERT(GetCurrentModule() != nullptr, "Current module has not been created");

	GetCurrentModule()->StartupModule();
}

bool FModuleManager::IsLoaded(const FStringView moduleName)
{
	FModuleHandle* moduleHandle = nullptr;
	int32 moduleIndex = INDEX_NONE;
	return FindModuleHandleAndIndex(moduleName, moduleHandle, moduleIndex);
}

IModule* FModuleManager::Reload(const FStringView moduleName)
{
	// If we can't find the module at all, then we need to load it
	FModuleHandle* moduleHandle = nullptr;
	int32 moduleIndex = INDEX_NONE;
	if (FindModuleHandleAndIndex(moduleName, moduleHandle, moduleIndex) == false)
	{
		return Load(moduleName);
	}

	// Unload the module, and then attempt to re-load it
	moduleHandle->Unload();
	if (moduleHandle->Load(moduleName))
	{
		return moduleHandle->GetModule();
	}

	moduleHandle->Unload();
	UM_LOG(Error, "Failed to reload module {}", moduleName);

	return nullptr;
}

void FModuleManager::Shutdown(TBadge<class FEngineInitializer>)
{
	if (GetCurrentModule() == nullptr)
	{
		return;
	}

	for (FModuleHandle& module : GModules)
	{
		module.Unload();
	}
	GModules.Clear();

	GetCurrentModule()->ShutdownModule();
	Umbral_OnModuleUnloaded();

	UM_ASSERT(GetCurrentModule() == nullptr, "Current module has not been destroyed");
}