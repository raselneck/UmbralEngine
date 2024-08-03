#include "Engine/Logging.h"
#include "Misc/StringBuilder.h"
#include "OpenGL/ShaderGL.h"
#include "OpenGL/UmbralToGL.h"
#include "USL/Conversion.h"

TErrorOr<void> UShaderGL::Compile()
{
	if (m_State == EShaderState::CompileSuccess)
	{
		return {};
	}

	if (m_State != EShaderState::NeedsCompile)
	{
		return MAKE_ERROR("Attempting to compile invalid shader");
	}

	GL_CHECK(glCompileShader(m_Handle));

	GLint compileStatus = 0;
	GL_CHECK(glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &compileStatus));

	if (compileStatus == GL_FALSE)
	{
		m_State = EShaderState::CompileFailed;

		TRY_EVAL(FString compileLog, GetCompileLog());
		return MAKE_ERROR("Failed to compile shader. Compilation log:\n{}", compileLog);
	}

	m_State = EShaderState::CompileSuccess;

	return {};
}

TErrorOr<FString> UShaderGL::GetCompileLog() const
{
	GLint logLength = 0;
	glGetShaderiv(m_Handle, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength <= 0)
	{
		return MAKE_ERROR("Found invalid length for shader compile log");
	}

	FStringBuilder log;
	char* logStartChars = log.AddZeroed(logLength);

	glGetShaderInfoLog(m_Handle, logLength, &logLength, logStartChars);

	return log.ReleaseString();
}

uint32 UShaderGL::GetShaderHandle() const
{
	return m_Handle;
}

bool UShaderGL::IsCompiled() const
{
	return m_State == EShaderState::CompileSuccess;
}

TErrorOr<void> UShaderGL::LoadFromBinary(const void* bytes, const int32 byteCount)
{
	const TSpan<const uint8> blob { reinterpret_cast<const uint8*>(bytes), byteCount };

	TRY_EVAL(const FString source, USL::ConvertSpirvToGlsl(blob));
	TErrorOr<void> loadResult = LoadFromText(source);

	if (loadResult.IsError())
	{
		UM_LOG(Info, "{} shader source generated from binary:\n{}", ToString(GetShaderType()), source);
	}

	return loadResult;
}

TErrorOr<void> UShaderGL::LoadFromText(const FStringView source)
{
	if (source.IsEmpty())
	{
		return MAKE_ERROR("Given source is empty");
	}

	// TODO Use the USL to process the shader source so we can support #include

	const int32 sourceLength = source.Length();
	const char* sourceChars = source.GetChars();

	GL_CHECK(glShaderSource(m_Handle, 1, &sourceChars, &sourceLength));
	m_State = EShaderState::NeedsCompile;

	return Compile();
}

void UShaderGL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	const GLenum shaderType = GL::GetShaderType(GetShaderType());
	GL_CHECK(m_Handle = glCreateShader(shaderType));
}

void UShaderGL::Destroyed()
{
	Super::Destroyed();

	if (SetActiveContextIfPossible() == EContextState::Unavailable)
	{
		return;
	}

	if (m_Handle != InvalidShaderHandle)
	{
		GL_CHECK(glDeleteShader(m_Handle));
	}
}