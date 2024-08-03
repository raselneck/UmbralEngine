#include "Containers/Optional.h"
#include "Engine/Application.h"
#if WITH_SDL2
#include "Engine/SDL/ApplicationSDL.h"
#endif
#include "Engine/Logging.h"
#include "Engine/ModuleManager.h"
#include "Misc/AtExit.h"
#include <cstdlib>

static TObjectPtr<UApplication> GApplication;

TObjectPtr<UApplication> GetApplication()
{
	return GApplication;
}

/**
 * @brief Defines a helper for initializing the engine.
 */
class UMBRAL_EXPORT FEngineInitializer
{
	UM_DISABLE_COPY(FEngineInitializer);
	UM_DISABLE_MOVE(FEngineInitializer);

public:

	/**
	 * @brief Creates an instance of this engine initializer.
	 */
	FEngineInitializer();

	/**
	 * @brief Destroys this engine initializer.
	 */
	~FEngineInitializer();

	/**
	 * @brief Attempts to initialize the engine.
	 *
	 * @return Any error that was encountered while initializing the error.
	 */
	TErrorOr<void> Initialize();

	/**
	 * @brief Shuts down this engine initializer.
	 */
	void Shutdown();
};

static FEngineInitializer* GEngineInitializer = nullptr;

FEngineInitializer::FEngineInitializer()
{
	UM_ASSERT(GEngineInitializer == nullptr, "Attempting to create multiple engine initializers");
	GEngineInitializer = this;
}

FEngineInitializer::~FEngineInitializer()
{
	Shutdown();
	GEngineInitializer = nullptr;
}

TErrorOr<void> FEngineInitializer::Initialize()
{
	constexpr TBadge<FEngineInitializer> badge;

	// The object heap can fail to initialize because we may allocate a lot of memory for it
	TRY_DO(FObjectHeap::Initialize(badge));

	// The module manager cannot fail to initialize because it's basically just letting know the linked module we're starting up
	FModuleManager::Initialize(badge);

	return {};
}

void FEngineInitializer::Shutdown()
{
	constexpr TBadge<FEngineInitializer> badge;

	FModuleManager::Shutdown(badge);
	FObjectHeap::Shutdown(badge);
}

static int32 UmbralRunApplication()
{
#if WITH_SDL2
	GApplication = MakeObject<UApplicationSDL>();
	GApplication->SetShouldKeepAlive(true);
#else
	TObjectPtr<UApplication> application;
#endif

	ON_EXIT_SCOPE()
	{
		GApplication.Reset();
	};

	if (GApplication.IsNull())
	{
		UM_LOG(Error, "Failed to allocate application");
		return EXIT_FAILURE;
	}

	if (TErrorOr<void> result = GApplication->Run();
	    result.IsError())
	{
		UM_LOG(Error, "{}", result.ReleaseError());
		return EXIT_FAILURE;
	}

	return GApplication->GetExitCode();
}

extern "C" int32 UmbralMain()
{
	FEngineInitializer initializer;

	if (TErrorOr<void> result = initializer.Initialize();
	    result.IsError())
	{
		UM_LOG(Error, "{}", result.ReleaseError());
		return EXIT_FAILURE;
	}

	return UmbralRunApplication();
}