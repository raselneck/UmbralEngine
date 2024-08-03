#include "Engine/Application.h"
#include "Engine/Engine.h"
#include "Engine/EngineLoop.h"
#include "Engine/Logging.h"
#include "Engine/ModuleManager.h"
#if WITH_IMGUI
#	include "ImGui/ImGui.h"
#	include "ImGui/ImGuiRenderer.h"
#	include "ImGui/ImGuiSystem.h"
#endif
#include "Object/ObjectHeap.h"

void UEngineLoop::Exit(int32 exitCode)
{
	m_ExitCode = exitCode;
	m_IsRunning = false;
}

TObjectPtr<UEngine> UEngineLoop::GetEngine() const
{
	return m_Application->GetEngine();
}

void UEngineLoop::Run()
{
	constexpr TBadge<UEngineLoop> badge;

	m_GameTime.Initialize(badge);

	TObjectPtr<UApplication> application = GetApplication();

	TObjectPtr<UEngine> engine = GetEngine();
	engine->BeginRun(badge);
	BeginRun();

	const IApplicationRenderingContext* primaryRenderingContext = application->GetRenderingContext(0);
	const TObjectPtr<UEngineWindow> primaryWindow = primaryRenderingContext->GetWindow();

	bool primaryWindowIsValidAndOpen = primaryWindow.IsValid() && primaryWindow->IsOpen();
	while (primaryWindowIsValidAndOpen)
	{
		m_GameTime.Update(badge);

		PollEvents();

		// Need to update this flag after polling events because the window may have closed
		primaryWindowIsValidAndOpen = primaryWindow.IsValid() && primaryWindow->IsOpen();

		if (primaryWindowIsValidAndOpen)
		{
			RunFrame(application);
		}
	}

	EndRun();
	engine->EndRun(badge);
}

void UEngineLoop::BeginFrame()
{
#if WITH_IMGUI
	// Tell each ImGui renderer that a new frame has begun
	m_Application->ForEachRenderingContext([this](const IApplicationRenderingContext& renderingContext)
	{
		renderingContext.GetImGuiRenderer()->NewFrame(m_GameTime);
		return EIterationDecision::Continue;
	});

	// Tell the ImGui system that a new frame has begun
	m_ImGuiSystem->NewFrame(m_GameTime);
#endif
}

void UEngineLoop::BeginRun()
{
#if WITH_IMGUI
	m_ImGuiSystem = GetApplication()->GetImGuiSystem();
#endif
}

void UEngineLoop::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_Application = FindAncestorOfType<UApplication>();
	const TObjectPtr<UEngine> engine = m_Application->GetEngine();

	const FIntSize windowSize = [this]()
	{
		// Sizes from https://www.studio1productions.com/Articles/16x9-Resolution.htm
		const TStaticArray<FIntSize, 8> appropriateSizes
		{{
			{ 768, 432 },
			{ 1280, 720 },
			{ 1600, 900 },
			{ 1920, 1080 },
			{ 2048, 1152 },
			{ 2560, 1440 },
			{ 3072, 1728 },
			{ 3840, 2160 },
		}};

		// This just uses the first monitor, but that might not always be the primary display
		const FIntRect displaySize = m_Application->GetVideoDisplayBounds(0);
		const FIntSize maximumSize
		{
			static_cast<int32>(displaySize.Width * 0.75f),
			static_cast<int32>(displaySize.Height * 0.75f),
		};

		for (int32 idx = appropriateSizes.Num() - 1; idx >= 0; --idx)
		{
			const FIntSize currentSize = appropriateSizes[idx];
			if (currentSize.Width <= maximumSize.Width && currentSize.Height <= maximumSize.Height)
			{
				return currentSize;
			}
		}

		return appropriateSizes[0];
	}();

	constexpr EGraphicsApi graphicsApi = EGraphicsApi::OpenGL;
	const FString windowTitle { FModuleManager::GetCurrentModuleName() };
	const TSubclassOf<UEngineViewport> viewportClass = engine->GetViewportClass();

	const TObjectPtr<UEngineViewport> mainViewport = m_Application->CreateWindowAndViewport(windowTitle, graphicsApi, windowSize.Width, windowSize.Height, viewportClass);
	if (mainViewport.IsNull())
	{
		UM_LOG(Error, "Failed to create primary viewport; title=\"{}\", api={}, size=({}x{}), viewport=\"{}\"", windowTitle, ToString(graphicsApi), windowSize.Width, windowSize.Height, viewportClass.GetClassName());
		UM_ASSERT_NOT_REACHED_MSG("Failed to create main viewport");
	}
}

void UEngineLoop::EndFrame()
{
	// Collect garbage before swapping buffers to allow us to sneak
	// into the end of a frame before any vertical sync can happen
	FObjectHeap::CollectGarbage();

	// Allow each swap chain to swap its back buffers
	m_Application->ForEachRenderingContext([](const IApplicationRenderingContext& renderingContext)
	{
		renderingContext.GetSwapChain()->SwapBuffers();
		return EIterationDecision::Continue;
	});
}

void UEngineLoop::EndRun()
{
}

void UEngineLoop::PollEvents()
{
	UM_ASSERT_NOT_REACHED_MSG("PollEvents not implemented for current engine loop type");
}

static double g_TickCount = 0.0;
static uint64 g_FrameCount = 0;

void UEngineLoop::RunFrame(const TObjectPtr<UApplication>& application)
{
	constexpr TBadge<UEngineLoop> badge;

	g_TickCount += m_GameTime.GetDeltaTime().GetTotalMilliseconds();
	g_FrameCount++;
	if (g_TickCount >= 1000.0)
	{
		UM_LOG(Info, "{} FPS", g_FrameCount);
		g_TickCount -= 1000.0;
		g_FrameCount = 0;
	}

	BeginFrame();

	// Update all viewports before drawing them in case they interact with each other
	application->ForEachRenderingContext([this](const IApplicationRenderingContext& renderingContext)
	{
		renderingContext.GetViewport()->Update(m_GameTime);
		return EIterationDecision::Continue;
	});

	// Draw all viewports
	application->ForEachRenderingContext([this](const IApplicationRenderingContext& renderingContext)
	{
#if WITH_IMGUI
		// TODO Viewports with UImGuiViewport need to be grouped with and childed to regular viewports, but other user-defined
		//      viewports need to be created as root rendering contexts... Guess that whole system needs to be revamped
//		const TObjectPtr<UImGuiRenderer> imguiRenderer = renderingContext.GetImGuiRenderer();
//		const ImGuiViewport* imguiViewport = imguiRenderer->GetImGuiViewport();
//		ImGui::SetNextWindowViewport(imguiViewport->ID);
#endif

		renderingContext.GetViewport()->Draw(m_GameTime);
		return EIterationDecision::Continue;
	});

#if WITH_IMGUI
	m_ImGuiSystem->Render(m_GameTime);
#endif

	EndFrame();
}