#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"
#include "Graphics/ShaderProgram.h"
#include "ShaderProgramGL.Generated.h"

/**
 * @brief Defines an OpenGL-backed shader program.
 */
UM_CLASS()
class UShaderProgramGL final : public UShaderProgram
{
	UM_GENERATED_BODY();

	static constexpr uint32 InvalidPipelineHandle = static_cast<uint32>(-1);
	static constexpr uint32 InvalidProgramHandle = static_cast<uint32>(-1);

	/**
	 * @brief An enumeration of shader program statuses.
	 */
	enum class EProgramState
	{
		NeedsShaders,
		NeedsLink,
		LinkFailed,
		LinkSuccess
	};

	/**
	 * @brief Defines information about a shader program attribute.
	 */
	struct FProgramAttributeGL
	{
	};

	/**
	 * @brief Defines information about a shader program uniform.
	 */
	struct FProgramUniformGL
	{
		/** @brief The uniform's name. */
		FString Name;

		/** @brief The uniform's location. */
		int32 Location = -1;
	};

public:

	/** @copydoc UShaderProgram::AttachShader */
	[[nodiscard]] virtual bool AttachShader(TObjectPtr<UShader> shader) override;

	/**
	 * @brief Gets this shader program's link log.
	 *
	 * @param log The output string for the log.
	 * @returns True if the link log was successfully retrieved, otherwise false.
	 */
	[[nodiscard]] TErrorOr<FString> GetLinkLog() const;

	/**
	 * @brief Gets the underlying pipeline handle.
	 *
	 * @return The underlying pipeline handle.
	 */
	[[nodiscard]] uint32 GetPipelineHandle() const;

	/**
	 * @brief Gets the underlying program handle.
	 *
	 * @return The underlying program handle.
	 */
	[[nodiscard]] uint32 GetProgramHandle() const;

	/** @copydoc UShaderProgram::AttachShader */
	[[nodiscard]] virtual TErrorOr<void> Link() override;

	/** @copydoc UShaderProgram::SetColor */
	[[nodiscard]] virtual bool SetColor(FStringView name, FColor value) override;

	/** @copydoc UShaderProgram::SetFloat */
	[[nodiscard]] virtual bool SetFloat(FStringView name, float value) override;

	/** @copydoc UShaderProgram::SetLinearColor */
	[[nodiscard]] virtual bool SetLinearColor(FStringView name, const FLinearColor& value) override;

	/** @copydoc UShaderProgram::SetMatrix3 */
	[[nodiscard]] virtual bool SetMatrix3(FStringView name, const FMatrix3& value) override;

	/** @copydoc UShaderProgram::SetMatrix4 */
	[[nodiscard]] virtual bool SetMatrix4(FStringView name, const FMatrix4& value) override;

	/** @copydoc UShaderProgram::SetTexture */
	[[nodiscard]] virtual bool SetTexture2D(FStringView name, TObjectPtr<const UTexture2D> value) override;

	/** @copydoc UShaderProgram::SetVector2 */
	[[nodiscard]] virtual bool SetVector2(FStringView name, const FVector2& value) override;

	/** @copydoc UShaderProgram::SetVector3 */
	[[nodiscard]] virtual bool SetVector3(FStringView name, const FVector3& value) override;

	/** @copydoc UShaderProgram::SetVector4 */
	[[nodiscard]] virtual bool SetVector4(FStringView name, const FVector4& value) override;

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UObject::Destroyed */
	virtual void Destroyed() override;

	/**
	 * @brief Finds and caches all of the program's attributes and uniforms.
	 */
	void FindAndCacheAttributesAndUniforms();

	/**
	 * @brief Finds the uniform with the given name.
	 *
	 * @param name The uniform name.
	 * @return Information about the uniform if it was found, otherwise nullptr.
	 */
	[[nodiscard]] const FProgramUniformGL* FindUniform(FStringView name) const;

private:

	TArray<FProgramUniformGL> m_Uniforms;
	uint32 m_PipelineHandle = InvalidPipelineHandle;
	uint32 m_ProgramHandle = InvalidProgramHandle;
	EProgramState m_State = EProgramState::NeedsShaders;
};