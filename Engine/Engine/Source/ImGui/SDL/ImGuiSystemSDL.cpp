#include "Engine/GameTime.h"
#include "Engine/Logging.h"
#include "Engine/SDL/ApplicationSDL.h"
#include "Engine/SDL/EngineWindowSDL.h"
#include "Graphics/OpenGL/GraphicsDeviceGL.h"
#include "ImGui/ImGuiRenderer.h"
#include "ImGui/ImGuiViewport.h"
#include "ImGui/SDL/ImGuiSystemSDL.h"
#include "HAL/Directory.h"
#include "HAL/File.h"
#include "HAL/Path.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#ifndef WITH_CUSTOM_IMGUI_FONT
#	define WITH_CUSTOM_IMGUI_FONT 1
#endif

#if !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !(defined(__APPLE__) && TARGET_OS_IOS) && !defined(__amigaos4__)
#	define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE 1
#else
#	define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE 0
#endif

#ifdef GetMessage
#	undef GetMessage
#endif

/**
 * @brief Gets the current ImGui system.
 *
 * @return The current ImGui system.
 */
static TObjectPtr<UImGuiSystemSDL> GetImGuiSystem()
{
	ImGuiIO& io = ImGui::GetIO();
	return { static_cast<UImGuiSystemSDL*>(io.BackendPlatformUserData) };
}

/**
 * @brief Gets the current application.
 *
 * @return The current application.
 */
static TObjectPtr<UApplicationSDL> GetApplication()
{
	TObjectPtr<UImGuiSystemSDL> imguiSystem = ::GetImGuiSystem();
	if (imguiSystem.IsNull())
	{
		return nullptr;
	}

	return imguiSystem->FindAncestorOfType<UApplicationSDL>();
}

/**
 * @brief Allocates memory for ImGui.
 *
 * @param numBytes The number of bytes to allocate.
 * @param userData The user data.
 * @return The allocated memory.
 */
static void* ImGuiAllocate(const size_t numBytes, void* userData)
{
	(void)userData;

	constexpr size_t maxNumBytes = static_cast<size_t>(TNumericLimits<FMemory::SizeType>::MaxValue);
	UM_ASSERT(numBytes <= maxNumBytes, "ImGui attempting to allocate too much memory");

	return FMemory::Allocate(static_cast<FMemory::SizeType>(numBytes));
}

/**
 * @brief Frees memory allocated by ImGui.
 *
 * @param memory The memory.
 * @param userData The user data.
 */
static void ImGuiFree(void* memory, void* userData)
{
	(void)userData;

	FMemory::Free(memory);
}

/**
 * @brief Gets the clipboard's text.
 *
 * @param userData The user data.
 * @return The clipboard text.
 */
[[nodiscard]] static const char* ImGuiGetClipboardText(void* userData)
{
	(void)userData;

	TObjectPtr<UImGuiSystemSDL> imguiSystem = GetImGuiSystem();
	return imguiSystem->GetClipboardText();
}

/**
 * @brief Sets the clipboard's text.
 *
 * @param userData The user data.
 * @param clipboardText The clipboard text.
 */
static void ImGuiSetClipboardText(void* userData, const char* clipboardText)
{
	(void)userData;

	TObjectPtr<UImGuiSystemSDL> imguiSystem = GetImGuiSystem();
	imguiSystem->SetClipboardText(clipboardText);
}

/**
 * @brief Sets platform IME data for a viewport.
 *
 * @param viewport The viewport.
 * @param data The IME data.
 */
static void ImGuiSetPlatformImeData(ImGuiViewport* viewport, ImGuiPlatformImeData* data)
{
	if (data->WantVisible == false)
	{
		return;
	}

	SDL_Rect rect {};
	rect.x = static_cast<int>(data->InputPos.x - viewport->Pos.x);
	rect.y = static_cast<int>(data->InputPos.y - viewport->Pos.y + data->InputLineHeight);
	rect.w = 1;
	rect.h = static_cast<int>(data->InputLineHeight);
	SDL_SetTextInputRect(&rect);
}

/**
 * @brief Gets an ImGui key from an SDL keycode.
 *
 * @param keycode The SDL keycode.
 * @return The ImGui key.
 */
[[nodiscard]] static ImGuiKey GetImGuiKeyFromKeycode(const int32 keycode)
{
	switch (keycode)
	{
	case SDLK_TAB:              return ImGuiKey_Tab;
	case SDLK_LEFT:             return ImGuiKey_LeftArrow;
	case SDLK_RIGHT:            return ImGuiKey_RightArrow;
	case SDLK_UP:               return ImGuiKey_UpArrow;
	case SDLK_DOWN:             return ImGuiKey_DownArrow;
	case SDLK_PAGEUP:           return ImGuiKey_PageUp;
	case SDLK_PAGEDOWN:         return ImGuiKey_PageDown;
	case SDLK_HOME:             return ImGuiKey_Home;
	case SDLK_END:              return ImGuiKey_End;
	case SDLK_INSERT:           return ImGuiKey_Insert;
	case SDLK_DELETE:           return ImGuiKey_Delete;
	case SDLK_BACKSPACE:        return ImGuiKey_Backspace;
	case SDLK_SPACE:            return ImGuiKey_Space;
	case SDLK_RETURN:           return ImGuiKey_Enter;
	case SDLK_ESCAPE:           return ImGuiKey_Escape;
	case SDLK_QUOTE:            return ImGuiKey_Apostrophe;
	case SDLK_COMMA:            return ImGuiKey_Comma;
	case SDLK_MINUS:            return ImGuiKey_Minus;
	case SDLK_PERIOD:           return ImGuiKey_Period;
	case SDLK_SLASH:            return ImGuiKey_Slash;
	case SDLK_SEMICOLON:        return ImGuiKey_Semicolon;
	case SDLK_EQUALS:           return ImGuiKey_Equal;
	case SDLK_LEFTBRACKET:      return ImGuiKey_LeftBracket;
	case SDLK_BACKSLASH:        return ImGuiKey_Backslash;
	case SDLK_RIGHTBRACKET:     return ImGuiKey_RightBracket;
	case SDLK_BACKQUOTE:        return ImGuiKey_GraveAccent;
	case SDLK_CAPSLOCK:         return ImGuiKey_CapsLock;
	case SDLK_SCROLLLOCK:       return ImGuiKey_ScrollLock;
	case SDLK_NUMLOCKCLEAR:     return ImGuiKey_NumLock;
	case SDLK_PRINTSCREEN:      return ImGuiKey_PrintScreen;
	case SDLK_PAUSE:            return ImGuiKey_Pause;
	case SDLK_KP_0:             return ImGuiKey_Keypad0;
	case SDLK_KP_1:             return ImGuiKey_Keypad1;
	case SDLK_KP_2:             return ImGuiKey_Keypad2;
	case SDLK_KP_3:             return ImGuiKey_Keypad3;
	case SDLK_KP_4:             return ImGuiKey_Keypad4;
	case SDLK_KP_5:             return ImGuiKey_Keypad5;
	case SDLK_KP_6:             return ImGuiKey_Keypad6;
	case SDLK_KP_7:             return ImGuiKey_Keypad7;
	case SDLK_KP_8:             return ImGuiKey_Keypad8;
	case SDLK_KP_9:             return ImGuiKey_Keypad9;
	case SDLK_KP_PERIOD:        return ImGuiKey_KeypadDecimal;
	case SDLK_KP_DIVIDE:        return ImGuiKey_KeypadDivide;
	case SDLK_KP_MULTIPLY:      return ImGuiKey_KeypadMultiply;
	case SDLK_KP_MINUS:         return ImGuiKey_KeypadSubtract;
	case SDLK_KP_PLUS:          return ImGuiKey_KeypadAdd;
	case SDLK_KP_ENTER:         return ImGuiKey_KeypadEnter;
	case SDLK_KP_EQUALS:        return ImGuiKey_KeypadEqual;
	case SDLK_LCTRL:            return ImGuiKey_LeftCtrl;
	case SDLK_LSHIFT:           return ImGuiKey_LeftShift;
	case SDLK_LALT:             return ImGuiKey_LeftAlt;
	case SDLK_LGUI:             return ImGuiKey_LeftSuper;
	case SDLK_RCTRL:            return ImGuiKey_RightCtrl;
	case SDLK_RSHIFT:           return ImGuiKey_RightShift;
	case SDLK_RALT:             return ImGuiKey_RightAlt;
	case SDLK_RGUI:             return ImGuiKey_RightSuper;
	case SDLK_APPLICATION:      return ImGuiKey_Menu;
	case SDLK_0:                return ImGuiKey_0;
	case SDLK_1:                return ImGuiKey_1;
	case SDLK_2:                return ImGuiKey_2;
	case SDLK_3:                return ImGuiKey_3;
	case SDLK_4:                return ImGuiKey_4;
	case SDLK_5:                return ImGuiKey_5;
	case SDLK_6:                return ImGuiKey_6;
	case SDLK_7:                return ImGuiKey_7;
	case SDLK_8:                return ImGuiKey_8;
	case SDLK_9:                return ImGuiKey_9;
	case SDLK_a:                return ImGuiKey_A;
	case SDLK_b:                return ImGuiKey_B;
	case SDLK_c:                return ImGuiKey_C;
	case SDLK_d:                return ImGuiKey_D;
	case SDLK_e:                return ImGuiKey_E;
	case SDLK_f:                return ImGuiKey_F;
	case SDLK_g:                return ImGuiKey_G;
	case SDLK_h:                return ImGuiKey_H;
	case SDLK_i:                return ImGuiKey_I;
	case SDLK_j:                return ImGuiKey_J;
	case SDLK_k:                return ImGuiKey_K;
	case SDLK_l:                return ImGuiKey_L;
	case SDLK_m:                return ImGuiKey_M;
	case SDLK_n:                return ImGuiKey_N;
	case SDLK_o:                return ImGuiKey_O;
	case SDLK_p:                return ImGuiKey_P;
	case SDLK_q:                return ImGuiKey_Q;
	case SDLK_r:                return ImGuiKey_R;
	case SDLK_s:                return ImGuiKey_S;
	case SDLK_t:                return ImGuiKey_T;
	case SDLK_u:                return ImGuiKey_U;
	case SDLK_v:                return ImGuiKey_V;
	case SDLK_w:                return ImGuiKey_W;
	case SDLK_x:                return ImGuiKey_X;
	case SDLK_y:                return ImGuiKey_Y;
	case SDLK_z:                return ImGuiKey_Z;
	case SDLK_F1:               return ImGuiKey_F1;
	case SDLK_F2:               return ImGuiKey_F2;
	case SDLK_F3:               return ImGuiKey_F3;
	case SDLK_F4:               return ImGuiKey_F4;
	case SDLK_F5:               return ImGuiKey_F5;
	case SDLK_F6:               return ImGuiKey_F6;
	case SDLK_F7:               return ImGuiKey_F7;
	case SDLK_F8:               return ImGuiKey_F8;
	case SDLK_F9:               return ImGuiKey_F9;
	case SDLK_F10:              return ImGuiKey_F10;
	case SDLK_F11:              return ImGuiKey_F11;
	case SDLK_F12:              return ImGuiKey_F12;
	case SDLK_F13:              return ImGuiKey_F13;
	case SDLK_F14:              return ImGuiKey_F14;
	case SDLK_F15:              return ImGuiKey_F15;
	case SDLK_F16:              return ImGuiKey_F16;
	case SDLK_F17:              return ImGuiKey_F17;
	case SDLK_F18:              return ImGuiKey_F18;
	case SDLK_F19:              return ImGuiKey_F19;
	case SDLK_F20:              return ImGuiKey_F20;
	case SDLK_F21:              return ImGuiKey_F21;
	case SDLK_F22:              return ImGuiKey_F22;
	case SDLK_F23:              return ImGuiKey_F23;
	case SDLK_F24:              return ImGuiKey_F24;
	case SDLK_AC_BACK:          return ImGuiKey_AppBack;
	case SDLK_AC_FORWARD:       return ImGuiKey_AppForward;
	}
	return ImGuiKey_None;
}

const char* UImGuiSystemSDL::GetClipboardText() const
{
	if (m_ClipboardText != nullptr)
	{
		SDL_free(m_ClipboardText);
	}

	m_ClipboardText = SDL_GetClipboardText();

	return m_ClipboardText;
}

void* UImGuiSystemSDL::GetMainOpenGLContext() const
{
	UM_ASSERT(m_Application.IsValid(), "Application is no longer valid");
	UM_ASSERT(m_Application->GetNumRenderingContexts() > 0, "Application has no rendering contexts");

	const IApplicationRenderingContext* renderingContext = m_Application->GetRenderingContext(0);
	const TObjectPtr<UGraphicsDeviceGL> graphicsDevice = renderingContext->GetGraphicsDevice<UGraphicsDeviceGL>();
	if (graphicsDevice.IsValid())
	{
		return graphicsDevice->GetContext();
	}

	return nullptr;
}

TObjectPtr<UEngineWindowSDL> UImGuiSystemSDL::GetMainWindow() const
{
	UM_ASSERT(m_Application.IsValid(), "Application is no longer valid");
	UM_ASSERT(m_Application->GetNumRenderingContexts() > 0, "Application has no rendering contexts");

	const IApplicationRenderingContext* renderingContext = m_Application->GetRenderingContext(0);
	return renderingContext->GetWindow<UEngineWindowSDL>();
}

void UImGuiSystemSDL::NewFrame(const FGameTime& gameTime)
{
	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	const TObjectPtr<UEngineWindowSDL> mainWindow = GetMainWindow();
	const FIntSize windowSize = mainWindow->GetSize();
	const FIntSize displaySize = mainWindow->GetDrawableSize();

	io.DisplaySize = windowSize.AsVector2();
	if (windowSize.IsEmpty() == false)
	{
		//io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);
		io.DisplayFramebufferScale = displaySize.AsVector2() / windowSize.AsVector2();
	}

	if (m_NeedToUpdateMonitors)
	{
		UpdateMonitors();
		m_NeedToUpdateMonitors = false;
	}

	// Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
	// (Accept SDL_GetPerformanceCounter() not returning a monotonically increasing value. Happens in VMs and Emscripten, see #6189, #6114, #3644)
	io.DeltaTime = gameTime.GetDeltaSeconds();

	if (m_MouseLastLeaveFrame > 0 && m_MouseLastLeaveFrame >= ImGui::GetFrameCount() && m_MouseButtonsDown == 0)
	{
		m_MouseWindowId = 0;
		m_MouseLastLeaveFrame = 0;
		io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
	}

	// Our io.AddMouseViewportEvent() calls will only be valid when not capturing.
	// Technically speaking testing for 'bd->MouseButtonsDown == 0' would be more rygorous, but testing for payload reduces noise and potential side-effects.
	if (m_MouseCanReportHoveredViewport && ImGui::GetDragDropPayload() == nullptr)
	{
		AddFlagTo(io.BackendFlags, ImGuiBackendFlags_HasMouseHoveredViewport);
	}
	else
	{
		RemoveFlagFrom(io.BackendFlags, ImGuiBackendFlags_HasMouseHoveredViewport);
	}

	UpdateMousePosition();
	UpdateMouseCursor();
	// TODO UpdateControllers();

	ImGui::NewFrame();
}

void UImGuiSystemSDL::ProcessKeyboardEvent(const SDL_KeyboardEvent& event)
{
	if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	const int32 keymod = event.keysym.mod;
	io.AddKeyEvent(ImGuiMod_Ctrl, HasFlag(keymod, KMOD_CTRL));
	io.AddKeyEvent(ImGuiMod_Shift, HasFlag(keymod, KMOD_SHIFT));
	io.AddKeyEvent(ImGuiMod_Alt, HasFlag(keymod, KMOD_ALT));
	io.AddKeyEvent(ImGuiMod_Super, HasFlag(keymod, KMOD_GUI));

	const ImGuiKey key = GetImGuiKeyFromKeycode(event.keysym.sym);
	io.AddKeyEvent(key, (event.type == SDL_KEYDOWN));
	io.SetKeyEventNativeData(key, event.keysym.sym, event.keysym.scancode, event.keysym.scancode);
}

void UImGuiSystemSDL::ProcessMouseButtonEvent(const SDL_MouseButtonEvent& event)
{
	if (event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_MOUSEBUTTONUP)
	{
		return;
	}

	int32 mouseButton = -1;
	if (event.button == SDL_BUTTON_LEFT) { mouseButton = ImGuiMouseButton_Left; }
	if (event.button == SDL_BUTTON_RIGHT) { mouseButton = ImGuiMouseButton_Right; }
	if (event.button == SDL_BUTTON_MIDDLE) { mouseButton = ImGuiMouseButton_Middle; }
	if (event.button == SDL_BUTTON_X1) { mouseButton = 3; }
	if (event.button == SDL_BUTTON_X2) { mouseButton = 4; }
	if (mouseButton == -1)
	{
		return;
	}

	const bool mouseButtonDown = event.type == SDL_MOUSEBUTTONDOWN;

	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseSourceEvent(event.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
	io.AddMouseButtonEvent(mouseButton, mouseButtonDown);

	if (mouseButtonDown)
	{
		AddFlagTo(m_MouseButtonsDown, 1 << mouseButton);
	}
	else
	{
		RemoveFlagFrom(m_MouseButtonsDown, 1 << mouseButton);
	}
}

void UImGuiSystemSDL::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& event)
{
	ImGuiIO& io = ImGui::GetIO();

	ImVec2 mousePosition { static_cast<float>(event.x), static_cast<float>(event.y) };
	if (HasFlag(io.ConfigFlags, ImGuiConfigFlags_ViewportsEnable))
	{
		int32 windowX = 0, windowY = 0;
		SDL_GetWindowPosition(SDL_GetWindowFromID(event.windowID), &windowX, &windowY);

		mousePosition.x += static_cast<float>(windowX);
		mousePosition.y += static_cast<float>(windowY);
	}

	io.AddMouseSourceEvent(event.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
	io.AddMousePosEvent(mousePosition.x, mousePosition.y);
}

void UImGuiSystemSDL::ProcessMouseWheelEvent(const SDL_MouseWheelEvent& event)
{
	const float wheel_x = -event.preciseX;
	const float wheel_y = event.preciseY;

	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseSourceEvent(event.which == SDL_TOUCH_MOUSEID ? ImGuiMouseSource_TouchScreen : ImGuiMouseSource_Mouse);
	io.AddMouseWheelEvent(wheel_x, wheel_y);
}

void UImGuiSystemSDL::ProcessTextInputEvent(const SDL_TextInputEvent& event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharactersUTF8(event.text);
}

void UImGuiSystemSDL::ProcessWindowEvent(const SDL_WindowEvent& event)
{
	// - When capturing mouse, SDL will send a bunch of conflicting LEAVE/ENTER event on every mouse move, but the final ENTER tends to be right.
	// - However we won't get a correct LEAVE event for a captured window.
	// - In some cases, when detaching a window from main viewport SDL may send SDL_WINDOWEVENT_ENTER one frame too late,
	//   causing SDL_WINDOWEVENT_LEAVE on previous frame to interrupt drag operation by clear mouse position. This is why
	//   we delay process the SDL_WINDOWEVENT_LEAVE events by one frame. See issue #5012 for details.

	ImGuiIO& io = ImGui::GetIO();
	SDL_Window* platformHandle = SDL_GetWindowFromID(event.windowID);

	switch (event.windowID)
	{
	case SDL_WINDOWEVENT_ENTER:
		m_MouseWindowId = event.windowID;
		m_MouseLastLeaveFrame = 0;
		break;

	case SDL_WINDOWEVENT_LEAVE:
		m_MouseLastLeaveFrame = ImGui::GetFrameCount() + 1;
		break;

	case SDL_WINDOWEVENT_FOCUS_GAINED:
		io.AddFocusEvent(true);
		break;

	case SDL_WINDOWEVENT_FOCUS_LOST:
		io.AddFocusEvent(false);
		break;

	case SDL_WINDOWEVENT_CLOSE:
		if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(platformHandle))
		{
			viewport->PlatformRequestClose = true;
		}
		break;

	case SDL_WINDOWEVENT_MOVED:
		if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(platformHandle))
		{
			viewport->PlatformRequestMove = true;
		}
		break;

	case SDL_WINDOWEVENT_RESIZED:
		if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(platformHandle))
		{
			viewport->PlatformRequestResize = true;
		}
		break;
	}
}

void UImGuiSystemSDL::Render(const FGameTime& gameTime)
{
	ImGui::Render();

	ImGuiIO& io = ImGui::GetIO();
	if (HasFlag(io.ConfigFlags, ImGuiConfigFlags_ViewportsEnable))
	{
		ImGui::UpdatePlatformWindows();

		ImGuiPlatformIO& platform = ImGui::GetPlatformIO();
		for (int32 idx = 0; idx < platform.Viewports.Size; ++idx)
		{
			ImGuiViewport* viewport = platform.Viewports.Data[idx];
			const IApplicationRenderingContext* renderingContext = m_Application->GetRenderingContext(idx);
			const TObjectPtr<UImGuiRenderer> imguiRenderer = renderingContext->GetImGuiRenderer();
			imguiRenderer->Draw(gameTime, viewport->DrawData);
		}
	}
	else
	{
		const IApplicationRenderingContext* mainRenderingContext = m_Application->GetMainRenderingContext();
		const TObjectPtr<UImGuiRenderer> mainImGuiRenderer = mainRenderingContext->GetImGuiRenderer();
		mainImGuiRenderer->Draw(gameTime, ImGui::GetDrawData());
	}
}

void UImGuiSystemSDL::SetClipboardText(const char* clipboardText)
{
	SDL_SetClipboardText(clipboardText);
}

void UImGuiSystemSDL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_Application = FindAncestorOfType<UApplicationSDL>();

	UM_ASSERT(ImGui::GetCurrentContext() == nullptr, "An existing ImGui system has already been initialized");

	IMGUI_CHECKVERSION();
	ImGui::SetAllocatorFunctions(ImGuiAllocate, ImGuiFree, nullptr);
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	UM_ASSERT(io.BackendPlatformUserData == nullptr, "An existing ImGui system has already been initialized");
	io.BackendPlatformUserData = this;

	io.IniFilename = nullptr;                                 // Disable saving window positions
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (HasFlag(io.ConfigFlags, ImGuiConfigFlags_ViewportsEnable))
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	LoadFonts();

	// Check and store if we are on a SDL backend that supports global mouse position
	// ("wayland" and "rpi" don't support it, but we chose to use an allow-list instead of a deny-list)
	m_MouseCanUseGlobalState = false;
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
	const FStringView currentBackend { SDL_GetCurrentVideoDriver() };
	const FStringView globalMouseAllowedBackends[] { "windows"_sv, "cocoa"_sv, "x11"_sv, "DIVE"_sv, "VMAN"_sv };
	for (const FStringView& backend : globalMouseAllowedBackends)
	{
		if (backend == currentBackend)
		{
			m_MouseCanUseGlobalState = true;
			break;
		}
	}
#endif

	// Setup backend capabilities flags
	io.BackendPlatformUserData = this;
	io.BackendPlatformName = "UmbralEngine";
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;  // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;   // We can honor io.WantSetMousePos requests (optional, rarely used)
	if (m_MouseCanUseGlobalState)
	{
		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;  // We can create multi-viewports on the Platform side (optional)
	}

	// SDL on Linux/OSX doesn't report events for unfocused windows (see https://github.com/ocornut/imgui/issues/4960)
	// We will use 'MouseCanReportHoveredViewport' to set 'ImGuiBackendFlags_HasMouseHoveredViewport' dynamically each frame.
#ifndef __APPLE__
	m_MouseCanReportHoveredViewport = m_MouseCanUseGlobalState;
#else
	m_MouseCanReportHoveredViewport = false;
#endif
	m_NeedToUpdateMonitors = true;

	io.GetClipboardTextFn = ImGuiGetClipboardText;
	io.SetClipboardTextFn = ImGuiSetClipboardText;
	io.SetPlatformImeDataFn = ImGuiSetPlatformImeData;

	// Gamepad handling
	//m_GamepadMode = ImGuiGamepadMode_AutoFirst;
	//m_NeedToUpdateGamepadList = true;

	// Load mouse cursors
	m_MouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	m_MouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	m_MouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	m_MouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	m_MouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	m_MouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
	m_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
	m_MouseCursors[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	m_MouseCursors[ImGuiMouseCursor_NotAllowed] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);

	if (IsPlatformInterfaceSupported())
	{
		InitializePlatformInterface();
	}
}

void UImGuiSystemSDL::Destroyed()
{
	if (IsPlatformInterfaceSupported())
	{
		ShutdownPlatformInterface();
	}

	if (m_ClipboardText != nullptr)
	{
		SDL_free(m_ClipboardText);
		m_ClipboardText = nullptr;
	}

	for (SDL_Cursor*& cursor : m_MouseCursors)
	{
		SDL_FreeCursor(cursor);
		cursor = nullptr;
	}

	ImGuiIO& io = ImGui::GetIO();
	io.BackendPlatformName = nullptr;
	io.BackendPlatformUserData = nullptr;
	RemoveFlagFrom(io.BackendFlags, ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad | ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_HasMouseHoveredViewport);

	Super::Destroyed();
}

static TObjectPtr<UEngineWindowSDL> GetWindowFromViewport(ImGuiViewport* viewport)
{
	return { static_cast<UEngineWindowSDL*>(viewport->PlatformUserData) };
}

static void ImGuiCreateWindow(ImGuiViewport* viewport)
{
	FEngineWindowParametersSDL windowParams;
	if (HasFlag(viewport->Flags, ImGuiViewportFlags_NoDecoration))
	{
		windowParams.WindowFlags |= SDL_WINDOW_BORDERLESS;
	}
	else
	{
		windowParams.WindowFlags |= SDL_WINDOW_RESIZABLE;
	}
#if UMBRAL_PLATFORM != UMBRAL_PLATFORM_WINDOWS
	if (HasFlag(viewport->Flags, ImGuiViewportFlags_NoTaskBarIcon))
	{
		windowParams.WindowFlags |= SDL_WINDOW_SKIP_TASKBAR;
	}
#endif
	if (HasFlag(viewport->Flags, ImGuiViewportFlags_TopMost))
	{
		windowParams.WindowFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
	}

	windowParams.WindowHeight = static_cast<int32>(viewport->Size.y);
	windowParams.WindowWidth = static_cast<int32>(viewport->Size.x);
	windowParams.WindowX = static_cast<int32>(viewport->Pos.x);
	windowParams.WindowY = static_cast<int32>(viewport->Pos.y);

	TObjectPtr<UApplicationSDL> application = GetApplication();
	TObjectPtr<UImGuiViewport> imguiViewport = application->CreateWindowAndViewport<UImGuiViewport>(windowParams);
	TObjectPtr<UEngineWindowSDL> imguiWindow = imguiViewport->GetWindow<UEngineWindowSDL>();
	imguiWindow->SetImGuiViewport(viewport);
}

static void ImGuiDestroyWindow(ImGuiViewport* viewport)
{
	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	TObjectPtr<UApplicationSDL> application = Cast<UApplicationSDL>(engineWindow->GetApplication());
	application->DestroyRenderingContext(engineWindow);

	viewport->PlatformUserData = nullptr;
	viewport->PlatformHandle = nullptr;
}

static void ImGuiShowWindow(ImGuiViewport* viewport)
{
#if defined(_WIN32)
	HWND hwnd = (HWND)viewport->PlatformHandleRaw;

	// SDL hack: Hide icon from task bar
	// Note: SDL 2.0.6+ has a SDL_WINDOW_SKIP_TASKBAR flag which is supported under Windows but the way it create the window breaks our seamless transition.
	if (HasFlag(viewport->Flags, ImGuiViewportFlags_NoTaskBarIcon))
	{
		LONG ex_style = ::GetWindowLong(hwnd, GWL_EXSTYLE);
		ex_style &= ~WS_EX_APPWINDOW;
		ex_style |= WS_EX_TOOLWINDOW;
		::SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
	}

	// SDL hack: SDL always activate/focus windows :/
	if (HasFlag(viewport->Flags, ImGuiViewportFlags_NoFocusOnAppearing))
	{
		::ShowWindow(hwnd, SW_SHOWNA);
		return;
	}
#endif

	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	engineWindow->ShowWindow();
}

static ImVec2 ImGuiGetWindowPos(ImGuiViewport* viewport)
{
	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	return engineWindow->GetPosition().Cast<float>();
}

static void ImGuiSetWindowPos(ImGuiViewport* viewport, const ImVec2 pos)
{
	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	engineWindow->SetPosition(FIntPoint { static_cast<int32>(pos.x), static_cast<int32>(pos.y) });
}

static ImVec2 ImGuiGetWindowSize(ImGuiViewport* viewport)
{
	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	return engineWindow->GetSize();
}

static void ImGuiSetWindowSize(ImGuiViewport* viewport, const ImVec2 size)
{
	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	engineWindow->SetSize(FIntSize { static_cast<int32>(size.x), static_cast<int32>(size.y) });
}

static void ImGuiSetWindowTitle(ImGuiViewport* viewport, const char* title)
{
	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	engineWindow->SetTitle(FStringView { title });
}

static void ImGuiSetWindowAlpha(ImGuiViewport* viewport, const float alpha)
{
	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	engineWindow->SetOpacity(alpha);
}

static void ImGuiSetWindowFocus(ImGuiViewport* viewport)
{
	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	engineWindow->Focus();
}

static bool ImGuiGetWindowFocus(ImGuiViewport* viewport)
{
	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	return engineWindow->IsFocused();
}

static bool ImGuiGetWindowMinimized(ImGuiViewport* viewport)
{
	TObjectPtr<UEngineWindowSDL> engineWindow = GetWindowFromViewport(viewport);
	return engineWindow->IsMinimized();
}

void UImGuiSystemSDL::InitializePlatformInterface()
{
	// Register platform interface (will be coupled with a renderer interface)
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.Platform_CreateWindow = ImGuiCreateWindow;
	platform_io.Platform_DestroyWindow = ImGuiDestroyWindow;
	platform_io.Platform_ShowWindow = ImGuiShowWindow;
	platform_io.Platform_SetWindowPos = ImGuiSetWindowPos;
	platform_io.Platform_GetWindowPos = ImGuiGetWindowPos;
	platform_io.Platform_SetWindowSize = ImGuiSetWindowSize;
	platform_io.Platform_GetWindowSize = ImGuiGetWindowSize;
	platform_io.Platform_SetWindowFocus = ImGuiSetWindowFocus;
	platform_io.Platform_GetWindowFocus = ImGuiGetWindowFocus;
	platform_io.Platform_GetWindowMinimized = ImGuiGetWindowMinimized;
	platform_io.Platform_SetWindowTitle = ImGuiSetWindowTitle;
	platform_io.Platform_SetWindowAlpha = ImGuiSetWindowAlpha;

	const IApplicationRenderingContext* mainRenderingContext = m_Application->GetMainRenderingContext();
	const TObjectPtr<UEngineWindowSDL> mainWindow = mainRenderingContext->GetWindow<UEngineWindowSDL>();
	mainWindow->SetImGuiViewport(ImGui::GetMainViewport());
}

bool UImGuiSystemSDL::IsPlatformInterfaceSupported() const
{
	ImGuiIO& io = ImGui::GetIO();
	return HasFlag(io.ConfigFlags, ImGuiConfigFlags_ViewportsEnable)
	    && HasFlag(io.BackendFlags, ImGuiBackendFlags_PlatformHasViewports);
}

void UImGuiSystemSDL::LoadFonts()
{
	struct FFontPathAndSize
	{
		FString Path;
		float Size = 13.0f;
	};

	const TArray<FFontPathAndSize> fontPaths
	{{
		{ FDirectory::GetContentFilePath("Fonts"_sv, "OSRS"_sv, "RuneScape.woff"_sv), 15.0f },
		{ FDirectory::GetContentFilePath("Fonts"_sv, "Cousine"_sv, "Cousine-Regular.ttf"_sv), 14.0f },
		{ FDirectory::GetContentFilePath("Fonts"_sv, "UbuntuMono"_sv, "UbuntuMono-Regular.ttf"_sv), 14.0f },
		{ FDirectory::GetContentFilePath("Fonts"_sv, "Roboto"_sv, "Roboto-Medium.ttf"_sv), 14.0f },
	}};

	ImGuiIO& io = ImGui::GetIO();
	for (const FFontPathAndSize& fontPathAndSize : fontPaths)
	{
		const FString& fontPath = fontPathAndSize.Path;
		const float fontSize = fontPathAndSize.Size;

		if (FFile::Exists(fontPath) == false)
		{
			UM_LOG(Error, "Failed to find ImGui font \"{}\"", fontPath);
			continue;
		}

		// FIXME Adding a font from memory using our load functions doesn't work for some reason.
		//       Once that is fixed, we should be able to disable ImGui file functions.
#if 0
		const TErrorOr<TArray<uint8>> fontLoadResult = FFile::ReadAllBytes(fontPath);
		if (fontLoadResult.IsError())
		{
			UM_LOG(Error, "Failed to load ImGui font \"{}\". Reason: {}", fontPath, fontLoadResult.GetError().GetMessage());
			UM_ASSERT_NOT_REACHED_MSG("Failed to load ImGui font");
		}

		const TArray<uint8>& fontBytes = fontLoadResult.GetValue();
		const ImFont* font = io.Fonts->AddFontFromMemoryTTF(const_cast<void*>(static_cast<const void*>(fontBytes.GetData())), fontBytes.Num(), fontSize);
#else
		const ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.GetChars(), fontSize);
#endif

		if (font == nullptr)
		{
			UM_LOG(Error, "Failed to add ImGui font \"{}\"", fontPath);
		}
	}

	if (io.Fonts->Fonts.Size == 0)
	{
		UM_LOG(Warning, "Using default ImGui font");
		io.Fonts->AddFontDefault();
	}
}

void UImGuiSystemSDL::ShutdownPlatformInterface()
{
	ImGui::DestroyPlatformWindows();
}

void UImGuiSystemSDL::UpdateMonitors()
{
	ImGuiPlatformIO& platform = ImGui::GetPlatformIO();
	platform.Monitors.resize(0);

	const int32 numVideoDisplays = SDL_GetNumVideoDisplays();
	for (int32 idx = 0; idx < numVideoDisplays; ++idx)
	{
		// Warning: the validity of monitor DPI information on Windows depends on the application DPI awareness settings,
		// which generally needs to be set in the manifest or at runtime.
		ImGuiPlatformMonitor monitor {};
		SDL_Rect displayBounds {};
		SDL_GetDisplayBounds(idx, &displayBounds);

		monitor.MainPos = { static_cast<float>(displayBounds.x), static_cast<float>(displayBounds.y) };
		monitor.MainSize = { static_cast<float>(displayBounds.w), static_cast<float>(displayBounds.h) };

		SDL_GetDisplayUsableBounds(idx, &displayBounds);
		monitor.WorkPos = { static_cast<float>(displayBounds.x), static_cast<float>(displayBounds.y) };
		monitor.WorkSize = { static_cast<float>(displayBounds.w), static_cast<float>(displayBounds.h) };

		// FIXME-VIEWPORT: On MacOS SDL reports actual monitor DPI scale, ignoring OS configuration.
		//                 We may want to set DpiScale to cocoa_window.backingScaleFactor here.
		float dpi = 0.0f;
		if (SDL_GetDisplayDPI(idx, &dpi, nullptr, nullptr) == 0)
		{
			monitor.DpiScale = dpi / 96.0f;
		}

		monitor.PlatformHandle = reinterpret_cast<void*>(static_cast<uintptr_t>(idx));
		platform.Monitors.push_back(monitor);
	}
}

void UImGuiSystemSDL::UpdateMouseCursor()
{
	ImGuiIO& io = ImGui::GetIO();

	if (HasFlag(io.ConfigFlags, ImGuiConfigFlags_NoMouseCursorChange))
	{
		return;
	}

	ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
	if (io.MouseDrawCursor || imguiCursor == ImGuiMouseCursor_None)
	{
		// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
		SDL_ShowCursor(SDL_FALSE);
	}
	else
	{
		// Show OS mouse cursor
		SDL_Cursor* expectedCursor = m_MouseCursors[imguiCursor] ? m_MouseCursors[imguiCursor] : m_MouseCursors[ImGuiMouseCursor_Arrow];
		if (m_MouseLastCursor != expectedCursor)
		{
			SDL_SetCursor(expectedCursor); // SDL function doesn't have an early out (see #6113)
			m_MouseLastCursor = expectedCursor;
		}
		SDL_ShowCursor(SDL_TRUE);
	}
}

void UImGuiSystemSDL::UpdateMousePosition()
{
	ImGuiIO& io = ImGui::GetIO();

	// We forward mouse input when hovered or captured (via SDL_MOUSEMOTION) or when focused (below)
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
	// SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger other operations outside
	SDL_CaptureMouse(m_MouseButtonsDown != 0 ? SDL_TRUE : SDL_FALSE);

	SDL_Window* mainWindow = GetMainWindow()->GetWindowHandle();
	SDL_Window* focusedWindow = SDL_GetKeyboardFocus();
	const bool isAppFocused = (focusedWindow && (mainWindow == focusedWindow || ImGui::FindViewportByPlatformHandle(focusedWindow)));
#else
	SDL_Window* focusedWindow = GetMainWindow()->GetWindowHandle();
	const bool isAppFocused = HasFlag(SDL_GetWindowFlags(focusedWindow), SDL_WINDOW_INPUT_FOCUS);
#endif

	if (isAppFocused)
	{
		// (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
		if (io.WantSetMousePos)
		{
#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				SDL_WarpMouseGlobal(static_cast<int32>(io.MousePos.x), static_cast<int32>(io.MousePos.y));
			}
			else
#endif
			{
				SDL_WarpMouseInWindow(mainWindow, static_cast<int32>(io.MousePos.x), static_cast<int32>(io.MousePos.y));
			}
		}

		// (Optional) Fallback to provide mouse position when focused (SDL_MOUSEMOTION already provides this when hovered or captured)
		if (m_MouseCanUseGlobalState && m_MouseButtonsDown == 0)
		{
			// Single-viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
			// Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
			int32 mouseX = 0, mouseY = 0, windowX = 0, windowY = 0;
			SDL_GetGlobalMouseState(&mouseX, &mouseY);

			if (HasFlag(io.ConfigFlags, ImGuiConfigFlags_ViewportsEnable) == false)
			{
				SDL_GetWindowPosition(focusedWindow, &windowX, &windowY);
				mouseX -= windowX;
				mouseY -= windowY;
			}
			io.AddMousePosEvent(static_cast<float>(mouseX), static_cast<float>(mouseY));
		}
	}

	// (Optional) When using multiple viewports: call io.AddMouseViewportEvent() with the viewport the OS mouse cursor is hovering.
	// If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the backend, Dear imGui will ignore this field and infer the information using its flawed heuristic.
	// - [!] SDL backend does NOT correctly ignore viewports with the _NoInputs flag.
	//       Some backend are not able to handle that correctly. If a backend report an hovered viewport that has the _NoInputs flag (e.g. when dragging a window
	//       for docking, the viewport has the _NoInputs flag in order to allow us to find the viewport under), then Dear ImGui is forced to ignore the value reported
	//       by the backend, and use its flawed heuristic to guess the viewport behind.
	// - [X] SDL backend correctly reports this regardless of another viewport behind focused and dragged from (we need this to find a useful drag and drop target).
	if (HasFlag(io.BackendFlags, ImGuiBackendFlags_HasMouseHoveredViewport))
	{
		ImGuiID mouseViewportId = 0;
		if (SDL_Window* mouseWindow = SDL_GetWindowFromID(m_MouseWindowId))
		{
			if (ImGuiViewport* mouseViewport = ImGui::FindViewportByPlatformHandle(mouseWindow))
			{
				mouseViewportId = mouseViewport->ID;
			}
		}
		io.AddMouseViewportEvent(mouseViewportId);
	}
}