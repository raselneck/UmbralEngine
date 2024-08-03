#include "Engine/Assert.h"
#include "Engine/EngineViewport.h"
#include "Engine/Logging.h"
#include "Engine/SDL/ApplicationSDL.h"
#include "Engine/SDL/EngineLoopSDL.h"
#include "Engine/SDL/EngineWindowSDL.h"
#include "Graphics/OpenGL/GraphicsDeviceGL.h"
#include "Graphics/OpenGL/SwapChainGL.h"
#include "Graphics/Vulkan/GraphicsDeviceVK.h"
#include "Graphics/Vulkan/SwapChainVK.h"
#include "Input/SDL/InputManagerSDL.h"
#include "Templates/CanVisitReferencedObjects.h"
#include <SDL2/SDL.h>

#if WITH_IMGUI
#	include "ImGui/OpenGL/ImGuiRendererGL.h"
#	include "ImGui/SDL/ImGuiSystemSDL.h"
#endif

namespace SDL
{
	void Free(void* memory)
	{
		FMemory::Free(memory);
	}

	void* Malloc(const size_t size)
	{
		return FMemory::Allocate(static_cast<FMemory::SizeType>(size));
	}

	void* Calloc(const size_t numElements, const size_t elementSize)
	{
		return FMemory::AllocateArray(static_cast<FMemory::SizeType>(numElements),
		                              static_cast<FMemory::SizeType>(elementSize));
	}

	void* Realloc(void* memory, const size_t size)
	{
		return FMemory::Reallocate(memory, static_cast<FMemory::SizeType>(size));
	}
}

FApplicationRenderingContextSDL::FApplicationRenderingContextSDL(TObjectPtr<UEngineWindowSDL> window)
	: m_Window { MoveTemp(window) }
{
	UM_ASSERT(m_Window.IsValid(), "Attempting to create rendering context with null window");
}

bool FApplicationRenderingContextSDL::ConditionalCreateGraphicsDevice()
{
	if (m_Window.IsNull())
	{
		return false;
	}

	if (m_GraphicsDevice.IsValid())
	{
		return true;
	}

	switch (m_Window->GetGraphicsApi())
	{
	case EGraphicsApi::OpenGL:
		m_GraphicsDevice = MakeObject<UGraphicsDeviceGL>(m_Window);
		break;

	case EGraphicsApi::Vulkan:
		m_GraphicsDevice = MakeObject<UGraphicsDeviceVK>(m_Window);
		break;

	default:
		UM_LOG(Error, "Cannot create {} graphics device", ToString(m_Window->GetGraphicsApi()));
		break;
	}

	return m_GraphicsDevice.IsValid();
}

#if WITH_IMGUI
bool FApplicationRenderingContextSDL::ConditionalCreateImGuiRenderer()
{
	if (m_GraphicsDevice.IsNull())
	{
		return false;
	}

	if (m_ImGuiRenderer.IsValid())
	{
		return true;
	}

	switch (m_Window->GetGraphicsApi())
	{
	case EGraphicsApi::OpenGL:
		m_ImGuiRenderer = MakeObject<UImGuiRendererGL>(m_GraphicsDevice);
		break;

	default:
		UM_LOG(Error, "Cannot create {} ImGui renderer", ToString(m_Window->GetGraphicsApi()));
		break;
	}

	return m_GraphicsDevice.IsValid();
}
#endif

bool FApplicationRenderingContextSDL::ConditionalCreateInputManager()
{
	if (m_InputManager.IsValid())
	{
		return true;
	}

	m_InputManager = MakeObject<UInputManagerSDL>(m_Window);

	return m_GraphicsDevice.IsValid();
}

bool FApplicationRenderingContextSDL::ConditionalCreateViewport(TSubclassOf<UEngineViewport> viewportClass)
{
	if (viewportClass.IsNull())
	{
		UM_LOG(Error, "Cannot create viewport when given invalid viewport class");
		return false;
	}

	if (m_Viewport.IsValid())
	{
		UM_LOG(Error, "Attempting to create \"{}\" viewport for window \"{}\" when one already exists", viewportClass->GetName(), m_Window->GetTitle());
		return true;
	}

	m_Viewport = MakeObject<UEngineViewport>(viewportClass, m_Window);

	return m_Viewport.IsValid();
}

bool FApplicationRenderingContextSDL::ConditionalCreateSwapChain()
{
	if (m_GraphicsDevice.IsNull())
	{
		return false;
	}

	if (m_SwapChain.IsValid())
	{
		return true;
	}

	switch (m_Window->GetGraphicsApi())
	{
	case EGraphicsApi::OpenGL:
		m_SwapChain = MakeObject<USwapChainGL>(m_GraphicsDevice);
		break;

	case EGraphicsApi::Vulkan:
		m_SwapChain = MakeObject<USwapChainVK>(m_GraphicsDevice);
		break;

	default:
		UM_LOG(Error, "Cannot create {} swap chain", ToString(m_Window->GetGraphicsApi()));
		break;
	}

	return m_SwapChain.IsValid();
}

TObjectPtr<UInputManager> FApplicationRenderingContextSDL::GetInputManager() const
{
	return m_InputManager;
}

TObjectPtr<UEngineWindow> FApplicationRenderingContextSDL::GetWindow() const
{
	return m_Window;
}

uint32 FApplicationRenderingContextSDL::GetWindowId() const
{
	if (m_Window.IsNull())
	{
		return static_cast<uint32>(-1);
	}

	SDL_Window* window = m_Window->GetWindowHandle();
	return SDL_GetWindowID(window);
}

TObjectPtr<UGraphicsDevice> UApplicationSDL::CreateGraphicsDevice(TObjectPtr<UEngineWindow> window)
{
	if (window.IsNull())
	{
		return nullptr;
	}

	FApplicationRenderingContextSDL* renderingContext = m_RenderingContexts.FindByPredicate([window](const FApplicationRenderingContextSDL& context)
	{
		return context.GetWindow() == window;
	});
	if (UM_ENSURE(renderingContext != nullptr) == false)
	{
		UM_LOG(Error, "Cannot create graphics device; failed to find rendering context for given window");
		return nullptr;
	}

	UM_ASSERT(renderingContext->ConditionalCreateGraphicsDevice(), "Failed to create graphics device for window");

	return renderingContext->GetGraphicsDevice();
}

TObjectPtr<UInputManager> UApplicationSDL::CreateInputManager(TObjectPtr<UEngineWindow> window)
{
	if (window.IsNull())
	{
		return nullptr;
	}

	FApplicationRenderingContextSDL* renderingContext = m_RenderingContexts.FindByPredicate([window](const FApplicationRenderingContextSDL& context)
	{
		return context.GetWindow() == window;
	});
	if (UM_ENSURE(renderingContext != nullptr) == false)
	{
		UM_LOG(Error, "Cannot create input manager; failed to find rendering context for given window");
		return nullptr;
	}

	UM_ASSERT(renderingContext->ConditionalCreateInputManager(), "Failed to create input manager for window");

	return renderingContext->GetInputManager();
}

TObjectPtr<USwapChain> UApplicationSDL::CreateSwapChain(TObjectPtr<UGraphicsDevice> graphicsDevice)
{
	if (graphicsDevice.IsNull())
	{
		return nullptr;
	}

	FApplicationRenderingContextSDL* renderingContext = m_RenderingContexts.FindByPredicate([graphicsDevice](const FApplicationRenderingContextSDL& context)
	{
		return context.GetGraphicsDevice() == graphicsDevice;
	});
	if (UM_ENSURE(renderingContext != nullptr) == false)
	{
		UM_LOG(Error, "Cannot create swap chain; failed to find rendering context for given graphics device");
		return nullptr;
	}

	UM_ASSERT(renderingContext->ConditionalCreateSwapChain(), "Failed to create swap chain for graphics device");

	return renderingContext->GetSwapChain();
}

TObjectPtr<UEngineViewport> UApplicationSDL::CreateViewportForWindow(TSubclassOf<UEngineViewport> viewportClass, TObjectPtr<UEngineWindow> window)
{
	if (window.IsNull())
	{
		return nullptr;
	}

	FApplicationRenderingContextSDL* renderingContext = m_RenderingContexts.FindByPredicate([window](const FApplicationRenderingContextSDL& context)
	{
		return context.GetWindow() == window;
	});
	if (UM_ENSURE(renderingContext != nullptr) == false)
	{
		UM_LOG(Error, "Cannot create viewport; failed to find rendering context for given window");
		return nullptr;
	}

	UM_ASSERT(renderingContext->ConditionalCreateViewport(viewportClass), "Failed to create viewport for window");

	return renderingContext->GetViewport();
}

TObjectPtr<UEngineWindow> UApplicationSDL::CreateWindow(const FString& title, const EGraphicsApi graphicsApi, const int32 width, const int32 height)
{
	FEngineWindowParametersSDL params;
	params.Title = title;
	params.GraphicsApi = graphicsApi;
	params.WindowWidth = width;
	params.WindowHeight = height;

	return CreateWindow(params);
}

TObjectPtr<UEngineWindowSDL> UApplicationSDL::CreateWindow(const FEngineWindowParametersSDL& params)
{
	FObjectCreationContext context;
	params.ApplyToContext(context);

	const TObjectPtr<UEngineWindowSDL> window = MakeObject<UEngineWindowSDL>(this, nullptr, context);
	(void)m_RenderingContexts.Emplace(window);

	return window;
}

TObjectPtr<UEngineViewport> UApplicationSDL::CreateWindowAndViewport(const FString& title, const EGraphicsApi graphicsApi, const int32 width, const int32 height, const TSubclassOf<UEngineViewport> viewportClass)
{
	FEngineWindowParametersSDL params;
	params.Title = title;
	params.GraphicsApi = graphicsApi;
	params.WindowWidth = width;
	params.WindowHeight = height;

	return CreateWindowAndViewport(params, viewportClass);
}

TObjectPtr<UEngineViewport> UApplicationSDL::CreateWindowAndViewport(const FEngineWindowParametersSDL& params, TSubclassOf<UEngineViewport> viewportClass)
{
	const int32 renderingContextIndex = m_RenderingContexts.Num();

	TObjectPtr<UEngineWindowSDL> window = CreateWindow(params);
	if (UM_ENSURE(window.IsValid()) == false)
	{
		return nullptr;
	}

	FApplicationRenderingContextSDL& renderingContext = m_RenderingContexts[renderingContextIndex];
	UM_ASSERT(renderingContext.GetWindow() == window, "Ruh roh, Raggy! Re rindow is wrong!");
	UM_ASSERT(renderingContext.ConditionalCreateGraphicsDevice(), "Failed to create graphics device for viewport");
	UM_ASSERT(renderingContext.ConditionalCreateSwapChain(), "Failed to create swap chain for viewport");
	UM_ASSERT(renderingContext.ConditionalCreateInputManager(), "Failed to create input manager for viewport");
#if WITH_IMGUI
	// HACK Don't create the main rendering context's ImGui renderer just yet because the ImGui system won't
	//      have been created by the time the window is created
	if (renderingContextIndex > 0)
	{
		UM_ASSERT(renderingContext.ConditionalCreateImGuiRenderer(), "Failed to create ImGui renderer for viewport");
	}
#endif
	UM_ASSERT(renderingContext.ConditionalCreateViewport(viewportClass), "Failed to create  viewport");

	window->ShowWindow();

	return renderingContext.GetViewport();
}

void UApplicationSDL::DestroyRenderingContext(const TObjectPtr<UEngineWindowSDL> window)
{
	const int32 renderingContextIndex = m_RenderingContexts.IndexOfByPredicate([window](const FApplicationRenderingContextSDL& renderingContext)
	{
		return renderingContext.GetWindow() == window;
	});

	if (UM_ENSURE(renderingContextIndex != INDEX_NONE))
	{
		m_RenderingContexts[renderingContextIndex].MarkForDeletion();
	}
}

void UApplicationSDL::DispatchWindowEvent(const SDL_WindowEvent& event)
{
	const TObjectPtr<UEngineWindowSDL> window = GetWindowFromWindowId(event.windowID);
	window->ProcessEvent(event);
}

TObjectPtr<UInputManagerSDL> UApplicationSDL::GetInputManagerFromWindowId(const uint32 id) const
{
	const FApplicationRenderingContextSDL* renderingContext = m_RenderingContexts.FindByPredicate([id](const FApplicationRenderingContextSDL& context)
	{
		return context.GetWindowId() == id;
	});

	if (renderingContext == nullptr)
	{
		return nullptr;
	}

	return renderingContext->GetInputManagerSDL();
}

const IApplicationRenderingContext* UApplicationSDL::GetRenderingContext(const int32 index) const
{
	if (m_RenderingContexts.IsValidIndex(index))
	{
		return m_RenderingContexts.GetData() + index;
	}
	return nullptr;
}

const FApplicationRenderingContextSDL* UApplicationSDL::GetRenderingContextForWindow(TObjectPtr<const UEngineWindow> window) const
{
	return m_RenderingContexts.FindByPredicate([window](const FApplicationRenderingContextSDL& context)
	{
		return context.GetWindow() == window;
	});
}

const IVideoDisplay* UApplicationSDL::GetVideoDisplay(const int32 index) const
{
	if (m_VideoDisplays.IsValidIndex(index))
	{
		return m_VideoDisplays.GetData() + index;
	}
	return nullptr;
}

TObjectPtr<UEngineWindowSDL> UApplicationSDL::GetWindowFromWindowId(const uint32 id) const
{
	const FApplicationRenderingContextSDL* renderingContext = m_RenderingContexts.FindByPredicate([id](const FApplicationRenderingContextSDL& context)
	{
		return context.GetWindowId() == id;
	});

	if (renderingContext == nullptr)
	{
		return nullptr;
	}

	return renderingContext->GetWindowSDL();
}

void UApplicationSDL::PurgeRenderingContextsPendingDeletion()
{
	for (int32 idx = m_RenderingContexts.Num() - 1; idx >= 0; --idx)
	{
		if (m_RenderingContexts[idx].IsMarkedForDeletion())
		{
			m_RenderingContexts.RemoveAt(idx);
		}
	}
}

void UApplicationSDL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	// TODO We need a better way to support running some kind of loop without a window for servers
	if (GetEngine()->IsHeadless())
	{
		return;
	}

	SDL_SetMemoryFunctions(SDL::Malloc, SDL::Calloc, SDL::Realloc, SDL::Free);
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		UM_LOG(Error, "Failed to initialize SDL; reason: {}", SDL_GetError());
		UM_ASSERT_NOT_REACHED_MSG("Failed to initialize SDL");
	}

	// Retrieve all video displays
	const int32 numVideoDisplays = SDL_GetNumVideoDisplays();
	for (int32 idx = 0; idx < numVideoDisplays; ++idx)
	{
		(void)m_VideoDisplays.Emplace(idx);
	}
}

TObjectPtr<UEngineLoop> UApplicationSDL::CreateEngineLoop()
{
	return MakeObject<UEngineLoopSDL>(this);
}

#if WITH_IMGUI
TObjectPtr<UImGuiSystem> UApplicationSDL::CreateImGuiSystem()
{
	return MakeObject<UImGuiSystemSDL>(this);
}
#endif

void UApplicationSDL::Destroyed()
{
	SDL_Quit();

	Super::Destroyed();
}

void UApplicationSDL::ManuallyVisitReferencedObjects(FObjectHeapVisitor& visitor)
{
	Super::ManuallyVisitReferencedObjects(visitor);

	for (FApplicationRenderingContextSDL& renderingContext : m_RenderingContexts)
	{
		renderingContext.VisitReferencedObjects(visitor);
	}
}