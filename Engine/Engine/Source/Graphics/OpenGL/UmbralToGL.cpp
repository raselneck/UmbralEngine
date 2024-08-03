#include "Containers/StringView.h"
#include "Engine/Logging.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/GraphicsResource.h"
#include "Graphics/OpenGL/GraphicsDeviceGL.h"
#include "Graphics/OpenGL/UmbralToGL.h"
#include "HAL/Path.h"
#if WITH_SDL2
#	include <SDL2/SDL.h>
#endif

namespace GL
{
	static FStringView GetOpenGLErrorName(const GLenum error)
	{
		switch (error)
		{
			case GL_INVALID_ENUM: return "GL_INVALID_ENUM"_sv;
			case GL_INVALID_VALUE: return "GL_INVALID_VALUE"_sv;
			case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION"_sv;
			case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION"_sv;
			case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY"_sv;
			default: return "<unknown>"_sv;
		}
	}

	static FStringView GetOpenGLErrorMessage(const GLenum error)
	{
		switch (error)
		{
			case GL_INVALID_ENUM: return "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag."_sv;
			case GL_INVALID_VALUE: return "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag."_sv;
			case GL_INVALID_OPERATION: return "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag."_sv;
			case GL_INVALID_FRAMEBUFFER_OPERATION: return "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag."_sv;
			case GL_OUT_OF_MEMORY: return "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded."_sv;
			default: return "Unknown error."_sv;
		}
	}

	bool CheckForActiveContext(const UGraphicsDevice* genericGraphicsDevice)
	{
		(void)genericGraphicsDevice;

#if WITH_SDL2
		const UGraphicsDeviceGL* graphicsDevice = CastChecked<UGraphicsDeviceGL>(genericGraphicsDevice);
		const SDL_GLContext graphicsDeviceContext = graphicsDevice->GetContext();
		const SDL_GLContext currentContext = SDL_GL_GetCurrentContext();

		if (graphicsDeviceContext != currentContext)
		{
			UM_LOG(Error, "Desired OpenGL context ({}) and active context ({}) do not match", graphicsDeviceContext, currentContext);
			return true;
		}
#endif

		return false;
	}

	bool CheckForActiveContext(const UGraphicsResource* resource)
	{
		if (resource == nullptr)
		{
			return false;
		}

		return CheckForActiveContext(resource->GetGraphicsDevice().GetObject());
	}

	bool CheckForError(const FStringView call, const FSourceLocation sourceLocation)
	{
		const GLenum error = glGetError();
		if (error == GL_NO_ERROR)
		{
			return false;
		}

		const FStringView errorName = GetOpenGLErrorName(error);
		const FStringView errorMessage = GetOpenGLErrorMessage(error);
		UM_LOG(Error, "OpenGL: {} error calling \"{}\"\n\tMessage: {}\n\tAt: {}", errorName, call, errorMessage, sourceLocation);

		return true;
	}
}