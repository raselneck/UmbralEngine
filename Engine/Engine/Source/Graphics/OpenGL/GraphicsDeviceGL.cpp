#include "Engine/Assert.h"
#include "Engine/Logging.h"
#include "Engine/SDL/EngineWindowSDL.h"
#include "Graphics/TextureFormat.h"
#include "Graphics/OpenGL/IndexBufferGL.h"
#include "Graphics/OpenGL/GraphicsDeviceGL.h"
#include "Graphics/OpenGL/ShaderGL.h"
#include "Graphics/OpenGL/ShaderProgramGL.h"
#include "Graphics/OpenGL/Texture2DGL.h"
#include "Graphics/OpenGL/TextureManagerGL.h"
#include "Graphics/OpenGL/VertexBufferGL.h"
#include "Graphics/OpenGL/UmbralToGL.h"
#include <SDL2/SDL_video.h>

// TODO To support wireframe with OpenGL ES, see this article https://www.polymonster.co.uk/blog/gles-wireframe

template<> struct TIsChar<GLubyte> : FTrueType { };

namespace GL
{
	[[nodiscard]] FStringView GetString(GLenum name)
	{
		const GLubyte* stringBytes = glGetString(name);
		const int32 stringByteCount = TStringTraits<GLubyte>::GetNullTerminatedCharCount(stringBytes);
		return FStringView { reinterpret_cast<const char*>(stringBytes), stringByteCount };
	}

	[[nodiscard]] TArray<FStringView> GetExtensions()
	{
		int32 numExtensions = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

		TArray<FStringView> extensions;
		extensions.Reserve(numExtensions);

		for (int32 idx = 0; idx < numExtensions; ++idx)
		{
			const GLubyte* extensionNameBytes = glGetStringi(GL_EXTENSIONS, idx);
			const int32 extensionNameByteCount = TStringTraits<GLubyte>::GetNullTerminatedCharCount(extensionNameBytes);
			(void)extensions.Emplace(reinterpret_cast<const char*>(extensionNameBytes), extensionNameByteCount);
		}

		return extensions;
	}
}

void UGraphicsDeviceGL::BindIndexBuffer(const TObjectPtr<const UIndexBuffer> indexBuffer)
{
	GLuint bufferHandle = 0;

	if (indexBuffer.IsValid())
	{
		m_BoundIndexBuffer = CastChecked<UIndexBufferGL>(indexBuffer);
		bufferHandle = m_BoundIndexBuffer->GetBufferHandle();
	}
	else
	{
		m_BoundIndexBuffer.Reset();
	}

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferHandle));
}

void UGraphicsDeviceGL::BindVertexBuffer(const TObjectPtr<const UVertexBuffer> vertexBuffer)
{
	GLuint arrayHandle = 0;
	GLuint bufferHandle = 0;

	if (vertexBuffer.IsValid())
	{
		m_BoundVertexBuffer = CastChecked<const UVertexBufferGL>(vertexBuffer);
		arrayHandle = m_BoundVertexBuffer->GetArrayHandle();
		bufferHandle = m_BoundVertexBuffer->GetBufferHandle();
	}
	else
	{
		m_BoundVertexBuffer.Reset();
	}

	GL_CHECK(glBindVertexArray(arrayHandle));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, bufferHandle));
}

void UGraphicsDeviceGL::Clear(const EClearOptions clearOptions, const FLinearColor& color, const float depth, const int32 stencil)
{
	GLbitfield clearFlags = 0;

	if (HasFlag(clearOptions, EClearOptions::Color))
	{
		if (m_ClearColor != color)
		{
			GL_CHECK(glClearColor(color.R, color.G, color.B, color.A));
			m_ClearColor = color;
		}

		clearFlags |= GL_COLOR_BUFFER_BIT;
	}

	if (HasFlag(clearOptions, EClearOptions::Depth))
	{
		if (FMath::IsNearlyEqual(m_ClearDepth, depth) == false)
		{
			GL_CHECK(glClearDepthf(depth));
			m_ClearDepth = depth;
		}

		clearFlags |= GL_DEPTH_BUFFER_BIT;
	}

	if (HasFlag(clearOptions, EClearOptions::Stencil))
	{
		if (m_ClearStencil != stencil)
		{
			GL_CHECK(glClearStencil(stencil));
			m_ClearStencil = stencil;
		}

		clearFlags |= GL_STENCIL_BUFFER_BIT;
	}

	if (clearFlags != 0)
	{
		GL_CHECK(glClear(clearFlags));
	}
}

TObjectPtr<UIndexBuffer> UGraphicsDeviceGL::CreateIndexBuffer(const EIndexBufferUsage usage)
{
	FObjectCreationContext context;
	context.SetParameter("usage"_sv, usage);

	return MakeObject<UIndexBufferGL>(this, nullptr, context);
}

TObjectPtr<UShader> UGraphicsDeviceGL::CreateShader(const EShaderType shaderType)
{
	FObjectCreationContext context;
	context.SetParameter("type"_sv, shaderType);

	return MakeObject<UShaderGL>(this, nullptr, context);
}

TObjectPtr<UShaderProgram> UGraphicsDeviceGL::CreateShaderProgram()
{
	return MakeObject<UShaderProgramGL>(this);
}

TObjectPtr<UTexture2D> UGraphicsDeviceGL::CreateTexture2D()
{
	return MakeObject<UTexture2DGL>(this);
}

TObjectPtr<UVertexBuffer> UGraphicsDeviceGL::CreateVertexBuffer(const EVertexBufferUsage usage)
{
	FObjectCreationContext context;
	context.SetParameter("usage"_sv, usage);

	return MakeObject<UVertexBufferGL>(this, nullptr, context);
}

void UGraphicsDeviceGL::DrawIndexedVertices(const EPrimitiveType primitiveType)
{
	UM_ASSERT(m_BoundVertexBuffer.IsValid(), "No vertex buffer is currently bound");
	UM_ASSERT(m_BoundIndexBuffer.IsValid(), "No index buffer is currently bound");

	const GLenum mode = GL::GetPrimitiveType(primitiveType);
	const GLsizei count = m_BoundIndexBuffer->GetElementCount();
	const GLenum type = GL::GetIndexElementType(m_BoundIndexBuffer->GetElementType());
	const GLvoid* indices = nullptr;
	GL_CHECK(glDrawElements(mode, count, type, indices));
}

void UGraphicsDeviceGL::DrawVertices(const EPrimitiveType primitiveType)
{
	UM_ASSERT(m_BoundVertexBuffer.IsValid(), "No vertex buffer is currently bound");

	const GLenum mode = GL::GetPrimitiveType(primitiveType);
	const GLint first = 0;
	const GLsizei count = m_BoundVertexBuffer->GetVertexCount();
	GL_CHECK(glDrawArrays(mode, first, count));
}

EGraphicsApi UGraphicsDeviceGL::GetApi() const
{
	return EGraphicsApi::OpenGL;
}

void* UGraphicsDeviceGL::GetContext() const
{
	return m_Context;
}

TObjectPtr<UEngineWindow> UGraphicsDeviceGL::GetWindow() const
{
	return m_Window;
}

SDL_Window* UGraphicsDeviceGL::GetWindowHandle() const
{
	return m_Window->GetWindowHandle();
}

EGraphicsContextState UGraphicsDeviceGL::SetActiveContext() const
{
	if (m_Window.IsNull())
	{
		return EGraphicsContextState::Unavailable;
	}

	if (m_Context == SDL_GL_GetCurrentContext())
	{
		return EGraphicsContextState::Available;
	}

	if (SDL_GL_MakeCurrent(GetWindowHandle(), m_Context) < 0)
	{
		UM_LOG(Error, "Failed to set current OpenGL context. Reason: {}", SDL_GetError());
		UM_ASSERT_NOT_REACHED();
	}

	return EGraphicsContextState::Available;
}

void UGraphicsDeviceGL::UseShaderProgram(TObjectPtr<UShaderProgram> shaderProgram)
{
	GLuint program = 0;

	if (shaderProgram.IsValid())
	{
		program = CastChecked<UShaderProgramGL>(shaderProgram)->GetProgramHandle();
	}

	if (program != m_CurrentProgram)
	{
		GL_CHECK(glUseProgram(program));
		m_CurrentProgram = program;
	}
}

void UGraphicsDeviceGL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_Window = FindAncestorOfType<UEngineWindowSDL>();
	UM_ENSURE(m_Window.IsValid());

	m_Context = SDL_GL_CreateContext(m_Window->GetWindowHandle());
	if (m_Context == nullptr)
	{
		UM_LOG(Error, "Failed to create OpenGL context. Reason: {}", SDL_GetError());
		UM_ASSERT_NOT_REACHED();
	}

	UM_ASSERT(SetActiveContext() == EGraphicsContextState::Available, "Graphics context is unavailable");

#if WITH_ANGLE == 0
	if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
	{
		UM_LOG(Error, "Failed to load OpenGL functions. Last error: {}", SDL_GetError());
		UM_ASSERT_NOT_REACHED();
	}
#endif

#ifdef glDebugMessageCallback
	glDebugMessageCallback([](GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam)
	{
		(void)source;
		(void)type;
		(void)id;
		(void)length;
		(void)userParam;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH: UM_LOG(Error, "[OpenGL] {}", message); break;
		case GL_DEBUG_SEVERITY_MEDIUM: UM_LOG(Warning, "[OpenGL] {}", message); break;
		case GL_DEBUG_SEVERITY_LOW: break;
		default: break;
		}
	}, this);
#endif

	m_TextureManager = MakeObject<UTextureManagerGL>(this);

#if 0
	const bool isComputeSupported = []()
	{
		GLuint shaderHandle = glCreateShader(GL_COMPUTE_SHADER);
		const bool isShaderValid = shaderHandle != 0;
		glDeleteShader(shaderHandle);
		return isShaderValid;
	}();

	UM_LOG(Info, "Compute Supported: {}", isComputeSupported);
	UM_LOG(Info, "OpenGL Vendor:     {}", GL::GetString(GL_VENDOR));
	UM_LOG(Info, "OpenGL Renderer:   {}", GL::GetString(GL_RENDERER));
	UM_LOG(Info, "OpenGL Version:    {}", GL::GetString(GL_VERSION));
	UM_LOG(Info, "GLSL Version:      {}", GL::GetString(GL_SHADING_LANGUAGE_VERSION));

	const TArray<FStringView> extensions = GL::GetExtensions();
	UM_LOG(Info, "Extension Count:   {}", extensions.Num());
	for (FStringView extension : extensions)
	{
		UM_LOG(Info, "-- {}", extension);
	}
#endif

	// TODO(TEMP) disable depth test by default
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_SCISSOR_TEST);

	glClearColor(m_ClearColor.R, m_ClearColor.G, m_ClearColor.B, m_ClearColor.A);
	glClearDepthf(m_ClearDepth);
	glClearStencil(m_ClearStencil);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	const FIntSize viewportSize = m_Window->GetDrawableSize();
	GL_CHECK(glViewport(0, 0, viewportSize.Width, viewportSize.Height));





	const FEnumInfo* formatEnum = ::GetType<ETextureFormat>();
	for (const FEnumEntryInfo& formatEntry : formatEnum->GetEntries())
	{
		const ETextureFormat textureFormat = static_cast<ETextureFormat>(formatEntry.GetValue());
		const GLenum internalFormat = ::GL::GetTextureInternalFormat(textureFormat);
		const GLenum format = ::GL::GetTextureFormat(textureFormat);
		const GLenum type = ::GL::GetTextureDataType(textureFormat);

		GLuint textureHandle = 0;
		GL_CHECK(glGenTextures(1, &textureHandle));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureHandle));
		GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 1, 1, 0, format, type, nullptr));
		GL_CHECK(glDeleteTextures(1, &textureHandle));
	}
}

void UGraphicsDeviceGL::Destroyed()
{
	SDL_GL_DeleteContext(m_Context);
	m_Context = nullptr;

	Super::Destroyed();
}