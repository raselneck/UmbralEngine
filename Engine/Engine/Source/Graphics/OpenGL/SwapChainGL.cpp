#include "Engine/Logging.h"
#include "OpenGL/GraphicsDeviceGL.h"
#include "OpenGL/SwapChainGL.h"
#include "OpenGL/UmbralToGL.h"
#include <SDL2/SDL.h>

/**
 * @brief Defines a way to save and restore the current OpenGL context in a scope.
 */
class FSaveCurrentContextScope final
{
public:

	/**
	 * @brief Saves the current OpenGL context and sets the given graphics device as the active one.
	 *
	 * @param graphicsDevice The graphics device to activate.
	 */
	explicit FSaveCurrentContextScope(const TObjectPtr<UGraphicsDeviceGL>& graphicsDevice)
		: m_WindowToRestore { SDL_GL_GetCurrentWindow() }
		, m_ContextToRestore { SDL_GL_GetCurrentContext() }
	{
		SDL_Window* window = graphicsDevice->GetWindowHandle();
		SDL_GLContext context = graphicsDevice->GetContext();
		SDL_GL_MakeCurrent(window, context);
	}

	/**
	 * @brief Restores the previous OpenGL context.
	 */
	~FSaveCurrentContextScope()
	{
		SDL_GL_MakeCurrent(m_WindowToRestore, m_ContextToRestore);
	}

private:

	SDL_Window* m_WindowToRestore = nullptr;
	SDL_GLContext m_ContextToRestore = nullptr;
};

TObjectPtr<UGraphicsDeviceGL> USwapChainGL::GetGraphicsDevice() const
{
	return FindAncestorOfType<UGraphicsDeviceGL>();
}

TErrorOr<void> USwapChainGL::SetSwapInterval(const ESwapInterval swapInterval)
{
	const FSaveCurrentContextScope contextScope { GetGraphicsDevice() };

	switch (swapInterval)
	{
	case ESwapInterval::Adaptive:
	case ESwapInterval::Immediate:
	case ESwapInterval::Synchronized:
		if (SDL_GL_SetSwapInterval(static_cast<int32>(swapInterval)) < 0)
		{
			return MAKE_ERROR("{}", SDL_GetError());
		}
		return {};

	default:
		UM_ASSERT_NOT_REACHED_MSG("Unhandled swap interval value");
	}
}

void USwapChainGL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	ESwapInterval desiredSwapInterval = ESwapInterval::Adaptive;
	if (const ESwapInterval* specifiedSwapInterval = context.GetParameter<ESwapInterval>("swapInterval"_sv))
	{
		desiredSwapInterval = *specifiedSwapInterval;
	}

	if (TErrorOr<void> result = SetSwapInterval(desiredSwapInterval);
	    result.IsError())
	{
		UM_LOG(Warning, "Failed to set {} swap interval; falling back to regular. Reason: {}", ToString(desiredSwapInterval), result.GetError().GetMessage());

		result = SetSwapInterval(ESwapInterval::Synchronized);
		UM_ENSURE(result.IsError() == false);
	}
}

void USwapChainGL::SwapBuffers()
{
	const TObjectPtr<UGraphicsDeviceGL> graphicsDevice = GetGraphicsDevice();
	const FSaveCurrentContextScope contextScope { graphicsDevice };

	SDL_GL_SwapWindow(graphicsDevice->GetWindowHandle());
}