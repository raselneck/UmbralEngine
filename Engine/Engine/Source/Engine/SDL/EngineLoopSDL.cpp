#include "Engine/SDL/ApplicationSDL.h"
#include "Engine/SDL/EngineLoopSDL.h"
#include "Engine/SDL/EngineWindowSDL.h"
#include "Input/SDL/InputManagerSDL.h"
#include <SDL2/SDL.h>

#if WITH_IMGUI
#	include "ImGui/SDL/ImGuiSystemSDL.h"
#endif

void UEngineLoopSDL::BeginRun()
{
	Super::BeginRun();

#if WITH_IMGUI
	m_ImGuiSystem = m_Application->GetImGuiSystem<UImGuiSystemSDL>();

	// HACK Create the main rendering context's ImGui renderer now that the ImGui system exists
	FApplicationRenderingContextSDL* mainRenderingContext = m_Application->GetMainRenderingContext();
	UM_ASSERT(mainRenderingContext->ConditionalCreateImGuiRenderer(), "Failed to create ImGui renderer for main viewport");
#endif
}

void UEngineLoopSDL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_Application = Cast<UApplicationSDL>(GetApplication());
}

void UEngineLoopSDL::EndFrame()
{
	// Call this before super class's EndFrame so that the graphics stacks can be cleaned up by the garbage collector
	m_Application->PurgeRenderingContextsPendingDeletion();

	Super::EndFrame();
}

void UEngineLoopSDL::PollEvents()
{
	m_Application->ForEachRenderingContext([](const IApplicationRenderingContext& renderingContext)
	{
		TObjectPtr<UInputManagerSDL> inputManager = CastChecked<UInputManagerSDL>(renderingContext.GetInputManager());
		inputManager->UpdateBeforePollingEvents();

		return EIterationDecision::Continue;
	});

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_WINDOWEVENT:
		{
#if WITH_IMGUI
			m_ImGuiSystem->ProcessWindowEvent(event.window);
#endif
			m_Application->DispatchWindowEvent(event.window);
			break;
		}

#if 0
		case SDL_CONTROLLERAXISMOTION:
		{
			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.caxis.windowID);
			inputManager->ProcessGamePadAxisEvent(event.caxis);
			break;
		}

		case SDL_CONTROLLERBUTTONDOWN:
		{
			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.cbutton.windowID);
			inputManager->ProcessGamePadButtonDownEvent(event.cbutton);
			break;
		}

		case SDL_CONTROLLERBUTTONUP:
		{
			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.cbutton.windowID);
			inputManager->ProcessGamePadButtonUpEvent(event.cbutton);
			break;
		}

		case SDL_CONTROLLERDEVICEADDED:
		{
			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.cdevice.windowID);
			inputManager->ProcessGamePadAddedEvent(event.cdevice);
			break;
		}

		case SDL_CONTROLLERDEVICEREMOVED:
		{
			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.cdevice.windowID);
			inputManager->ProcessGamePadRemovedEvent(event.cdevice);
			break;
		}

		case SDL_CONTROLLERDEVICEREMAPPED:
		{
			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.cdevice.windowID);
			inputManager->ProcessGamePadRemappedEvent(event.cdevice);
			break;
		}
#endif

		case SDL_KEYDOWN:
		{
#if WITH_IMGUI
			m_ImGuiSystem->ProcessKeyboardEvent(event.key);
#endif

			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.key.windowID);
			inputManager->ProcessKeyDownEvent(event.key);

			break;
		}

		case SDL_KEYUP:
		{
#if WITH_IMGUI
			m_ImGuiSystem->ProcessKeyboardEvent(event.key);
#endif

			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.key.windowID);
			[[likely]] if (UM_ENSURE(inputManager.IsValid())) // TODO(HACK) Fix the cause of this issue :^) (repro by pressing Escape to close game)
			{
				inputManager->ProcessKeyUpEvent(event.key);
			}

			break;
		}

		case SDL_MOUSEMOTION:
		{
#if WITH_IMGUI
			m_ImGuiSystem->ProcessMouseMotionEvent(event.motion);
#endif

			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.motion.windowID);
			inputManager->ProcessMouseMoveEvent(event.motion);

			break;
		}

		case SDL_MOUSEBUTTONDOWN:
		{
#if WITH_IMGUI
			m_ImGuiSystem->ProcessMouseButtonEvent(event.button);
#endif

			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.button.windowID);
			inputManager->ProcessMouseButtonDownEvent(event.button);

			break;
		}

		case SDL_MOUSEBUTTONUP:
		{
#if WITH_IMGUI
			m_ImGuiSystem->ProcessMouseButtonEvent(event.button);
#endif

			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.button.windowID);
			inputManager->ProcessMouseButtonUpEvent(event.button);

			break;
		}

		case SDL_MOUSEWHEEL:
		{
#if WITH_IMGUI
			m_ImGuiSystem->ProcessMouseWheelEvent(event.wheel);
#endif

			const TObjectPtr<UInputManagerSDL> inputManager = m_Application->GetInputManagerFromWindowId(event.wheel.windowID);
			inputManager->ProcessMouseWheelEvent(event.wheel);

			break;
		}

#if WITH_IMGUI
		case SDL_TEXTINPUT:
		{
			m_ImGuiSystem->ProcessTextInputEvent(event.text);
			break;
		}
#endif
		}
	}
}