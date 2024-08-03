#include "Engine/SDL/ApplicationSDL.h"
#include "Engine/SDL/EngineWindowSDL.h"
#include "Engine/Logging.h"
#include "Memory/Memory.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>

#ifdef GetObject
#	undef GetObject
#endif

#if WITH_IMGUI
#	include "ImGui/ImGui.h"
#endif

#ifndef WITH_SHARED_OPENGL_CONTEXTS
#	define WITH_SHARED_OPENGL_CONTEXTS 1
#endif

FEngineWindowParametersSDL::FEngineWindowParametersSDL()
	: Title { "Umbral"_sv }
	, WindowX { SDL_WINDOWPOS_CENTERED }
	, WindowY { SDL_WINDOWPOS_CENTERED }
{
}

void FEngineWindowParametersSDL::ApplyToContext(FObjectCreationContext& context) const
{
	context.SetParameter<FStringView>("title"_sv, Title);
	context.SetParameter<EGraphicsApi>("graphicsApi"_sv, GraphicsApi);
	context.SetParameter<int32>("windowX"_sv, WindowX);
	context.SetParameter<int32>("windowY"_sv, WindowY);
	context.SetParameter<int32>("windowWidth"_sv, WindowWidth);
	context.SetParameter<int32>("windowHeight"_sv, WindowHeight);
	context.SetParameter<uint32>("windowFlags"_sv, WindowFlags);
#if WITH_IMGUI
	context.SetParameter<ImGuiViewport*>("viewport"_sv, Viewport);
#endif
}

void FEngineWindowParametersSDL::RetrieveFromContext(const FObjectCreationContext& context)
{
	if (const FStringView* titleParam = context.GetParameter<FStringView>("title"_sv))
	{
		Title.Reset();
		Title.Append(*titleParam);
	}

	if (const EGraphicsApi* graphicsApiParam = context.GetParameter<EGraphicsApi>("graphicsApi"_sv))
	{
		GraphicsApi = *graphicsApiParam;
	}

	if (const int32* windowXParam = context.GetParameter<int32>("windowX"_sv))
	{
		WindowX = *windowXParam;
	}

	if (const int32* windowYParam = context.GetParameter<int32>("windowY"_sv))
	{
		WindowY = *windowYParam;
	}

	if (const int32* widthParam = context.GetParameter<int32>("windowWidth"_sv))
	{
		WindowWidth = *widthParam;
	}

	if (const int32* heightParam = context.GetParameter<int32>("windowHeight"_sv))
	{
		WindowHeight = *heightParam;
	}

	if (const uint32* windowFlags = context.GetParameter<uint32>("windowFlags"_sv))
	{
		WindowFlags = *windowFlags;
	}

#if WITH_IMGUI
	if (ImGuiViewport** viewport = context.GetParameter<ImGuiViewport*>("viewport"_sv))
	{
		Viewport = *viewport;
	}
#endif
}

void UEngineWindowSDL::Close()
{
	m_IsOpen = false;

	const TObjectPtr<UApplicationSDL> application = FindAncestorOfType<UApplicationSDL>();
	application->DestroyRenderingContext(this);
}

void UEngineWindowSDL::Focus()
{
	SDL_RaiseWindow(m_WindowHandle);
}

FIntSize UEngineWindowSDL::GetDrawableSize() const
{
	if (m_WindowHandle == nullptr)
	{
		return {};
	}

	FIntSize drawableSize;
	switch (m_GraphicsApi)
	{
	case EGraphicsApi::OpenGL:
		SDL_GL_GetDrawableSize(m_WindowHandle, &drawableSize.Width, &drawableSize.Height);
		break;

	case EGraphicsApi::Vulkan:
		SDL_Vulkan_GetDrawableSize(m_WindowHandle, &drawableSize.Width, &drawableSize.Height);
		break;

	default:
		drawableSize = GetSize();
		break;
	}

	return drawableSize;
}

FIntPoint UEngineWindowSDL::GetPosition() const
{
	FIntPoint position;
	SDL_GetWindowPosition(m_WindowHandle, &position.X, &position.Y);
	return position;
}

FIntSize UEngineWindowSDL::GetSize() const
{
	if (m_WindowHandle == nullptr)
	{
		return {};
	}

	FIntSize size;
	if (IsMinimized() == false)
	{
		SDL_GetWindowSize(m_WindowHandle, &size.Width, &size.Height);
	}
	return size;
}

FStringView UEngineWindowSDL::GetTitle() const
{
	return m_Title;
}

void UEngineWindowSDL::HideWindow()
{
	if (m_WindowHandle != nullptr)
	{
		SDL_HideWindow(m_WindowHandle);
	}
}

bool UEngineWindowSDL::IsFocused() const
{
	const uint32 windowFlags = SDL_GetWindowFlags(m_WindowHandle);
	return HasFlag(windowFlags, SDL_WINDOW_INPUT_FOCUS);
}

bool UEngineWindowSDL::IsMinimized() const
{
	const uint32 windowFlags = SDL_GetWindowFlags(m_WindowHandle);
	return HasFlag(windowFlags, SDL_WINDOW_MINIMIZED);
}

bool UEngineWindowSDL::IsOpen() const
{
	return m_IsOpen;
}

void UEngineWindowSDL::ProcessEvent(const SDL_WindowEvent& event)
{
	if (event.event == SDL_WINDOWEVENT_CLOSE)
	{
		Close();
	}
}

#if WITH_IMGUI
void UEngineWindowSDL::SetImGuiViewport(ImGuiViewport* viewport)
{
	UM_ASSERT(m_ImGuiViewport == nullptr, "Attempting to register multiple viewports with one window");
	UM_ASSERT(viewport != nullptr, "Cannot register null viewport with window");

	m_ImGuiViewport = viewport;
	m_ImGuiViewport->PlatformHandle = m_WindowHandle;
	m_ImGuiViewport->PlatformUserData = this;

	SDL_SysWMinfo info {};
	SDL_VERSION(&info.version);
	if (SDL_GetWindowWMInfo(m_WindowHandle, &info))
	{
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
		viewport->PlatformHandleRaw = static_cast<void*>(info.info.win.window);
#elif defined(__APPLE__) && defined(SDL_VIDEO_DRIVER_COCOA)
		viewport->PlatformHandleRaw = static_cast<void*>(info.info.cocoa.window);
#endif
	}
}
#endif

void UEngineWindowSDL::SetOpacity(const float opacity)
{
	SDL_SetWindowOpacity(m_WindowHandle, FMath::Saturate(opacity));
}

void UEngineWindowSDL::SetPosition(const FIntPoint& position)
{
	SDL_SetWindowPosition(m_WindowHandle, position.X, position.Y);
}

void UEngineWindowSDL::SetSize(const FIntSize& size)
{
	SDL_SetWindowSize(m_WindowHandle, size.Width, size.Height);
}

void UEngineWindowSDL::SetTitle(const FStringView title)
{
	m_Title.Reset();
	m_Title.Append(title);

	SDL_SetWindowTitle(m_WindowHandle, m_Title.GetChars());
}

void UEngineWindowSDL::ShowWindow()
{
	SDL_ShowWindow(m_WindowHandle);
}

void UEngineWindowSDL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	FEngineWindowParametersSDL params;
	params.RetrieveFromContext(context);

	UM_LOG(Verbose, "Attempting to create a {}x{} {} window with title \"{}\"",
	       params.WindowWidth, params.WindowHeight,
	       ToString(params.GraphicsApi), params.Title);

	SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "0");
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#if WITH_IMGUI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, params.Viewport ? "1" : "0");
#else
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "0");
#endif

#if WITH_SHARED_OPENGL_CONTEXTS
	const TObjectPtr<UApplicationSDL> application = FindAncestorOfType<UApplicationSDL>();
	const bool isSecondaryWindow = application->GetNumRenderingContexts() > 0;

	if (isSecondaryWindow)
	{
		const IApplicationRenderingContext* primaryRenderingContext = application->GetRenderingContext(0);
		const TObjectPtr<UGraphicsDevice> primaryGraphicsDevice = primaryRenderingContext->GetGraphicsDevice();
		(void)primaryGraphicsDevice->SetActiveContext();
	}
#endif

	// Build the window flags
	uint32 windowFlags = SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI | params.WindowFlags;
	switch (params.GraphicsApi)
	{
	case EGraphicsApi::OpenGL:
		SDL_GL_ResetAttributes();
#if WITH_SHARED_OPENGL_CONTEXTS
		SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, isSecondaryWindow ? 1 : 0);
#endif
#if WITH_ANGLE
		SDL_SetHint(SDL_HINT_OPENGL_ES_DRIVER, "1");
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_EGL, true);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
#if UMBRAL_DEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		windowFlags |= SDL_WINDOW_OPENGL;
		break;

	case EGraphicsApi::Vulkan:
		windowFlags |= SDL_WINDOW_VULKAN;
		break;

	default:
		break;
	}

	m_Title = MoveTemp(params.Title);
	SDL_Window* windowHandle = SDL_CreateWindow(m_Title.GetChars(),
	                                            params.WindowX, params.WindowY,
	                                            params.WindowWidth, params.WindowHeight,
	                                            windowFlags);
	if (windowHandle == nullptr)
	{
		UM_LOG(Error, "Failed to allocate window. Reason: {}", SDL_GetError());
		UM_ASSERT_NOT_REACHED();
	}

	m_WindowHandle = windowHandle;
	m_GraphicsApi = params.GraphicsApi;
	m_IsOpen = true;

#if WITH_IMGUI
	if (params.Viewport != nullptr)
	{
		SetImGuiViewport(params.Viewport);
	}
#endif
}

void UEngineWindowSDL::Destroyed()
{
#if WITH_IMGUI
	if (m_ImGuiViewport != nullptr)
	{
		m_ImGuiViewport->PlatformHandle = nullptr;
		m_ImGuiViewport->PlatformUserData = nullptr;
		m_ImGuiViewport->PlatformHandleRaw = nullptr;
		m_ImGuiViewport = nullptr;
	}
#endif

	if (m_WindowHandle != nullptr)
	{
		SDL_DestroyWindow(m_WindowHandle);
		m_WindowHandle = nullptr;
	}

	Super::Destroyed();
}