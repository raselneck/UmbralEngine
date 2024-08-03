#include "Engine/Logging.h"
#include "Graphics/Color.h"
#include "Graphics/LinearColor.h"
#include "Misc/StringBuilder.h"
#include "Object/WeakObjectPtr.h"
#include "OpenGL/ShaderGL.h"
#include "OpenGL/ShaderProgramGL.h"
#include "OpenGL/Texture2DGL.h"
#include "OpenGL/UmbralToGL.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

/**
 * @brief Gets the name of a shader type.
 *
 * @param type The shader type.
 * @return The name of the shader type.
 */
[[maybe_unused]] static constexpr FStringView GetShaderTypeName(const GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:							return "float"_sv;
	case GL_FLOAT_VEC2:						return "vec2"_sv;
	case GL_FLOAT_VEC3:						return "vec3"_sv;
	case GL_FLOAT_VEC4:						return "vec4"_sv;
	case GL_INT:							return "int"_sv;
	case GL_INT_VEC2:						return "ivec2"_sv;
	case GL_INT_VEC3:						return "ivec3"_sv;
	case GL_INT_VEC4:						return "ivec4"_sv;
	case GL_UNSIGNED_INT:					return "unsigned int"_sv;
	case GL_UNSIGNED_INT_VEC2:				return "uvec2"_sv;
	case GL_UNSIGNED_INT_VEC3:				return "uvec3"_sv;
	case GL_UNSIGNED_INT_VEC4:				return "uvec4"_sv;
	case GL_BOOL:							return "bool"_sv;
	case GL_BOOL_VEC2:						return "bvec2"_sv;
	case GL_BOOL_VEC3:						return "bvec3"_sv;
	case GL_BOOL_VEC4:						return "bvec4"_sv;
	case GL_FLOAT_MAT2:						return "mat2"_sv;
	case GL_FLOAT_MAT3:						return "mat3"_sv;
	case GL_FLOAT_MAT4:						return "mat4"_sv;
	case GL_FLOAT_MAT2x3:					return "mat2x3"_sv;
	case GL_FLOAT_MAT2x4:					return "mat2x4"_sv;
	case GL_FLOAT_MAT3x2:					return "mat3x2"_sv;
	case GL_FLOAT_MAT3x4:					return "mat3x4"_sv;
	case GL_FLOAT_MAT4x2:					return "mat4x2"_sv;
	case GL_FLOAT_MAT4x3:					return "mat4x3"_sv;
	case GL_SAMPLER_2D:						return "sampler2D"_sv;
	case GL_SAMPLER_3D:						return "sampler3D"_sv;
	case GL_SAMPLER_CUBE:					return "samplerCube"_sv;
	case GL_SAMPLER_2D_SHADOW:				return "sampler2DShadow"_sv;
	case GL_SAMPLER_2D_ARRAY:				return "sampler2DArray"_sv;
	case GL_SAMPLER_2D_ARRAY_SHADOW:		return "sampler2DArrayShadow"_sv;
	case GL_SAMPLER_CUBE_SHADOW:			return "samplerCubeShadow"_sv;
	case GL_INT_SAMPLER_2D:					return "isampler2D"_sv;
	case GL_INT_SAMPLER_3D:					return "isampler3D"_sv;
	case GL_INT_SAMPLER_CUBE:				return "isamplerCube"_sv;
	case GL_INT_SAMPLER_2D_ARRAY:			return "isampler2DArray"_sv;
	case GL_UNSIGNED_INT_SAMPLER_2D:		return "usampler2D"_sv;
	case GL_UNSIGNED_INT_SAMPLER_3D:		return "usampler3D"_sv;
	case GL_UNSIGNED_INT_SAMPLER_CUBE:		return "usamplerCube"_sv;
	case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:	return "usampler2DArray"_sv;
	default:                                return "<unknown>"_sv;
	}
}

bool UShaderProgramGL::AttachShader(const TObjectPtr<UShader> genericShader)
{
	if (genericShader.IsNull())
	{
		return false;
	}

	// TODO Ensure the shader was made with the same graphics device?

	const TObjectPtr<const UShaderGL> shader = CastChecked<UShaderGL>(genericShader);
	GL_CHECK(glAttachShader(m_ProgramHandle, shader->GetShaderHandle()));

	m_State = EProgramState::NeedsLink;

	return true;
}

TErrorOr<FString> UShaderProgramGL::GetLinkLog() const
{
	GLint logLength = 0;
	GL_CHECK(glGetProgramiv(m_ProgramHandle, GL_INFO_LOG_LENGTH, &logLength));

	if (logLength <= 0)
	{
		return MAKE_ERROR("Found invalid length for program link log");
	}

	FStringBuilder log;
	char* logStartChars = log.AddZeroed(logLength);

	GL_CHECK(glGetProgramInfoLog(m_ProgramHandle, logLength, &logLength, logStartChars));

	return log.ReleaseString();
}

uint32 UShaderProgramGL::GetPipelineHandle() const
{
	return m_PipelineHandle;
}

uint32 UShaderProgramGL::GetProgramHandle() const
{
	return m_ProgramHandle;
}

TErrorOr<void> UShaderProgramGL::Link()
{
	if (m_State == EProgramState::LinkSuccess)
	{
		return {};
	}

	if (m_State != EProgramState::NeedsLink)
	{
		return MAKE_ERROR("Attempting to link invalid shader program");
	}

	GL_CHECK(glLinkProgram(m_ProgramHandle));

	GLint linkStatus = 0;
	GL_CHECK(glGetProgramiv(m_ProgramHandle, GL_LINK_STATUS, &linkStatus));

	if (linkStatus == GL_FALSE)
	{
		m_State = EProgramState::LinkFailed;

		TRY_EVAL(FString linkLog, GetLinkLog());
		return MAKE_ERROR("Failed to link shader program:\n{}", linkLog);
	}

	FindAndCacheAttributesAndUniforms();

	m_State = EProgramState::LinkSuccess;

	return {};
}

bool UShaderProgramGL::SetColor(const FStringView name, const FColor value)
{
	return SetLinearColor(name, value.ToLinearColor());
}

bool UShaderProgramGL::SetFloat(const FStringView name, const float value)
{
	if (const FProgramUniformGL* uniform = FindUniform(name))
	{
		GL_CHECK(glProgramUniform1f(m_ProgramHandle, uniform->Location, value));
		return true;
	}

	return false;
}

bool UShaderProgramGL::SetLinearColor(const FStringView name, const FLinearColor& value)
{
	if (const FProgramUniformGL* uniform = FindUniform(name))
	{
		GL_CHECK(glProgramUniform4f(m_ProgramHandle, uniform->Location, value.R, value.G, value.B, value.A));
		return true;
	}

	return false;
}

bool UShaderProgramGL::SetMatrix3(const FStringView name, const FMatrix3& value)
{
	if (const FProgramUniformGL* uniform = FindUniform(name))
	{
		GL_CHECK(glProgramUniformMatrix3fv(m_ProgramHandle, uniform->Location, 1, GL_FALSE, value.GetValuePtr()));
		return true;
	}

	return false;
}

bool UShaderProgramGL::SetMatrix4(const FStringView name, const FMatrix4& value)
{
	if (const FProgramUniformGL* uniform = FindUniform(name))
	{
		GL_CHECK(glProgramUniformMatrix4fv(m_ProgramHandle, uniform->Location, 1, GL_FALSE, value.GetValuePtr()));
		return true;
	}

	return false;
}

bool UShaderProgramGL::SetTexture2D(const FStringView name, const TObjectPtr<const UTexture2D> value)
{
	int32 textureSlot = 0;

	if (const UTexture2DGL* texture2D = Cast<UTexture2DGL>(value.GetObject()))
	{
		textureSlot = texture2D->Bind();
	}

	if (const FProgramUniformGL* uniform = FindUniform(name))
	{
		GL_CHECK(glProgramUniform1i(m_ProgramHandle, uniform->Location, textureSlot));
		return true;
	}

	return false;
}

bool UShaderProgramGL::SetVector2(const FStringView name, const FVector2& value)
{
	if (const FProgramUniformGL* uniform = FindUniform(name))
	{
		GL_CHECK(glProgramUniform2f(m_ProgramHandle, uniform->Location, value.X, value.Y));
		return true;
	}

	return false;
}

bool UShaderProgramGL::SetVector3(const FStringView name, const FVector3& value)
{
	if (const FProgramUniformGL* uniform = FindUniform(name))
	{
		GL_CHECK(glProgramUniform3f(m_ProgramHandle, uniform->Location, value.X, value.Y, value.Z));
		return true;
	}

	return false;
}

bool UShaderProgramGL::SetVector4(const FStringView name, const FVector4& value)
{
	if (const FProgramUniformGL* uniform = FindUniform(name))
	{
		GL_CHECK(glProgramUniform4f(m_ProgramHandle, uniform->Location, value.X, value.Y, value.Z, value.W));
		return true;
	}

	return false;
}

void UShaderProgramGL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	GL_CHECK(m_ProgramHandle = glCreateProgram());
}

void UShaderProgramGL::Destroyed()
{
	Super::Destroyed();

	if (SetActiveContextIfPossible() == EContextState::Unavailable)
	{
		return;
	}

	if (m_ProgramHandle != InvalidProgramHandle)
	{
		GL_CHECK(glDeleteProgram(m_ProgramHandle));
	}

	if (m_PipelineHandle != InvalidPipelineHandle)
	{
		GL_CHECK(glDeleteProgramPipelines(1, &m_PipelineHandle));
	}
}

void UShaderProgramGL::FindAndCacheAttributesAndUniforms()
{
	int32 uniformCount = 0;
	GL_CHECK(glGetProgramiv(m_ProgramHandle, GL_ACTIVE_UNIFORMS, &uniformCount));

	int32 attribCount = 0;
	GL_CHECK(glGetProgramiv(m_ProgramHandle, GL_ACTIVE_ATTRIBUTES, &attribCount));

	m_Uniforms.Reset();
	m_Uniforms.Reserve(uniformCount);

	GLchar nameBuffer[128];
	FMemory::ZeroOutArray(nameBuffer);
	constexpr int32 maxNameLength = UM_ARRAY_SIZE(nameBuffer);

	for (int32 idx = 0; idx < attribCount; ++idx)
	{
		int32 attribSize = 0;
		GLenum attribType = GL_NONE;
		int32 nameLength = 0;
		GL_CHECK(glGetActiveAttrib(m_ProgramHandle, static_cast<GLuint>(idx), maxNameLength, &nameLength, &attribSize, &attribType, nameBuffer));

		const FStringView attribName { nameBuffer, nameLength };
		(void)attribName;

		//UM_LOG(Info, "Found {} attribute \"{}\" at index {}", GetShaderTypeName(attribType), attribName, idx);
	}

	for (int32 idx = 0; idx < uniformCount; ++idx)
	{
		int32 uniformSize = 0;
		GLenum uniformType = GL_NONE;
		int32 nameLength = 0;
		GL_CHECK(glGetActiveUniform(m_ProgramHandle, static_cast<GLuint>(idx), maxNameLength, &nameLength, &uniformSize, &uniformType, nameBuffer));

		FProgramUniformGL& uniformInfo = m_Uniforms.AddDefaultGetRef();
		uniformInfo.Name = FString { nameBuffer, nameLength };
		GL_CHECK(uniformInfo.Location = glGetUniformLocation(m_ProgramHandle, nameBuffer));

		//UM_LOG(Info, "Found {} uniform \"{}\" at location {} (index = {})", GetShaderTypeName(uniformType), uniformInfo.Name, uniformInfo.Location, idx);
	}
}

const UShaderProgramGL::FProgramUniformGL* UShaderProgramGL::FindUniform(const FStringView name) const
{
	return m_Uniforms.FindByPredicate([name](const FProgramUniformGL& uniformInfo)
	{
		return uniformInfo.Name == name;
	});
}