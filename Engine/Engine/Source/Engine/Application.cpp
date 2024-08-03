#include "Engine/Application.h"
#include "Engine/CommandLine.h"
#include "Engine/Engine.h"
#include "Engine/EngineLoop.h"
#include "Engine/EngineViewport.h"
#include "Engine/Logging.h"
#include "Engine/ModuleManager.h"
#include "HAL/FileSystem.h"
#include "Input/InputManager.h"
#include "Memory/Memory.h"
#include "Object/ObjectHeap.h"

TObjectPtr<UGraphicsDevice> UApplication::CreateGraphicsDevice(TObjectPtr<UEngineWindow> window)
{
	(void)window;

	UM_ASSERT_NOT_REACHED_MSG("CreateGraphicsDevice not implemented for this application");
}

TObjectPtr<UInputManager> UApplication::CreateInputManager(TObjectPtr<UEngineWindow> window)
{
	(void)window;
	UM_ASSERT_NOT_REACHED();
}

TObjectPtr<USwapChain> UApplication::CreateSwapChain(TObjectPtr<UGraphicsDevice> graphicsDevice)
{
	(void)graphicsDevice;

	UM_ASSERT_NOT_REACHED_MSG("CreateSwapChain not implemented for this application");
}

TObjectPtr<UEngineViewport> UApplication::CreateViewportForWindow(TObjectPtr<UEngineWindow> window)
{
	const TSubclassOf<UEngineViewport> viewportClass = GetEngine()->GetViewportClass();
	return CreateViewportForWindow(viewportClass, MoveTemp(window));
}

TObjectPtr<UEngineViewport> UApplication::CreateViewportForWindow(TSubclassOf<UEngineViewport> viewportClass, TObjectPtr<UEngineWindow> window)
{
	(void)viewportClass;
	(void)window;
	UM_ASSERT_NOT_REACHED();
}

TObjectPtr<UEngineWindow> UApplication::CreateWindow(const FString& title, const EGraphicsApi graphicsApi, const int32 width, const int32 height)
{
	(void)title;
	(void)graphicsApi;
	(void)width;
	(void)height;

	UM_ASSERT_NOT_REACHED_MSG("CreateWindow not implemented for this application");
}

TObjectPtr<UEngineViewport> UApplication::CreateWindowAndViewport(const FString& title, const EGraphicsApi graphicsApi, const int32 width, const int32 height, const TSubclassOf<UEngineViewport> viewportClass)
{
	TObjectPtr<UEngineWindow> window = CreateWindow(title, graphicsApi, width, height);
	UM_ASSERT(window.IsValid(), "Failed to create window for viewport");

	TObjectPtr<UGraphicsDevice> graphicsDevice = CreateGraphicsDevice(window);
	UM_ASSERT(graphicsDevice.IsValid(), "Failed to create graphics device for viewport");

	TObjectPtr<USwapChain> swapChain = CreateSwapChain(graphicsDevice);
	UM_ASSERT(swapChain.IsValid(), "Failed to create swap chain for viewport");

	TObjectPtr<UInputManager> inputManager = CreateInputManager(window);
	UM_ASSERT(inputManager.IsValid(), "Failed to create input manager for viewport");

#if WITH_IMGUI
	// TODO Create the ImGui renderer
#endif

	TObjectPtr<UEngineViewport> viewport = CreateViewportForWindow(viewportClass, window);
	UM_ASSERT(viewport.IsValid(), "Failed to create viewport");

	window->ShowWindow(); // Show window before returning because not everyone will remember to show it

	return viewport;
}

void UApplication::ForEachRenderingContext(TFunction<EIterationDecision(const IApplicationRenderingContext&)> callback) const
{
	const int32 numRenderingContexts = GetNumRenderingContexts();
	for (int32 idx = 0; idx < numRenderingContexts; ++idx)
	{
		const IApplicationRenderingContext* renderingContext = GetRenderingContext(idx);
		const EIterationDecision decision = callback(*renderingContext);

		if (decision == EIterationDecision::Break)
		{
			break;
		}
	}
}

TObjectPtr<UGraphicsDevice> UApplication::GetGraphicsDevice(TObjectPtr<const UEngineWindow> window) const
{
	const IApplicationRenderingContext* renderingContext = GetRenderingContextForWindow(MoveTemp(window));
	if (renderingContext == nullptr)
	{
		return nullptr;
	}

	return renderingContext->GetGraphicsDevice();
}

TObjectPtr<UInputManager> UApplication::GetInputManager(TObjectPtr<const UEngineWindow> window) const
{
	const IApplicationRenderingContext* renderingContext = GetRenderingContextForWindow(MoveTemp(window));
	if (renderingContext == nullptr)
	{
		return nullptr;
	}

	return renderingContext->GetInputManager();
}

int32 UApplication::GetNumRenderingContexts() const
{
	UM_ASSERT_NOT_REACHED();
}

int32 UApplication::GetNumVideoDisplays() const
{
	UM_ASSERT_NOT_REACHED();
}

const IApplicationRenderingContext* UApplication::GetRenderingContext(int32 index) const
{
	(void)index;
	UM_ASSERT_NOT_REACHED();
}

const IVideoDisplay* UApplication::GetVideoDisplay(const int32 index) const
{
	(void)index;
	UM_ASSERT_NOT_REACHED();
}

TObjectPtr<UEngineViewport> UApplication::GetViewport(TObjectPtr<const UEngineWindow> window) const
{
	const IApplicationRenderingContext* renderingContext = GetRenderingContextForWindow(MoveTemp(window));
	if (renderingContext == nullptr)
	{
		return nullptr;
	}

	return renderingContext->GetViewport();
}

TErrorOr<void> UApplication::Run()
{
	// TODO We need a better way to support running some kind of loop without a window for servers
	if (m_Engine->IsHeadless())
	{
		return {};
	}

	m_EngineLoop = CreateEngineLoop();
	if (m_EngineLoop.IsNull())
	{
		return MAKE_ERROR("Failed to create engine loop");
	}

#if WITH_IMGUI
	// The ImGui system usually depends on the main window being created, which the engine loop will do
	m_ImGuiSystem = CreateImGuiSystem();
	if (m_ImGuiSystem.IsNull())
	{
		return MAKE_ERROR("Failed to create ImGui system");
	}
#endif

	BeginRun();
	m_EngineLoop->Run();
	EndRun();

	m_ExitCode = m_EngineLoop->GetExitCode();

	return {};
}

void UApplication::BeginRun()
{
}

void UApplication::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	// Ensure that our current module is an engine module
	const IEngineModule* currentEngineModule = Cast<IEngineModule>(FModuleManager::GetCurrentModule());
	UM_ASSERT(currentEngineModule != nullptr, "Attempting to run a non-engine module");

	// Games have restricted file access
	const bool isGameModule = currentEngineModule->GetModuleType() == EModuleType::Game ||
	                          currentEngineModule->GetModuleType() == EModuleType::GameLibrary;
	FFileSystem::SetCanAccessFilesAnywhere(isGameModule == false);

	const TSubclassOf<UEngine> engineClass = currentEngineModule->GetEngineClass();
	UM_ASSERT(engineClass.IsValid(), "Current module has not specified an engine class");

	m_Engine = MakeObject<UEngine>(engineClass, this);
	UM_ASSERT(m_Engine.IsValid(), "Failed to create engine from current module");
}

TObjectPtr<UEngineLoop> UApplication::CreateEngineLoop()
{
	UM_ASSERT_NOT_REACHED_MSG("CreateEngineLoop not implemented for this application");
}

#if WITH_IMGUI
TObjectPtr<UImGuiSystem> UApplication::CreateImGuiSystem()
{
	UM_ASSERT_NOT_REACHED_MSG("CreateImGuiSystem not implemented for this application");
}
#endif

void UApplication::EndRun()
{
}

const IApplicationRenderingContext* UApplication::GetRenderingContextForWindow(TObjectPtr<const UEngineWindow> window) const
{
	if (window.IsNull())
	{
		return nullptr;
	}

	const IApplicationRenderingContext* result = nullptr;
	ForEachRenderingContext([window, &result](const IApplicationRenderingContext& renderingContext)
	{
		if (renderingContext.GetWindow() == window)
		{
			result = &renderingContext;
			return EIterationDecision::Break;
		}

		return EIterationDecision::Continue;
	});

	return result;
}