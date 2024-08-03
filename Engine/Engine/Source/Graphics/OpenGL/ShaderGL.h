#pragma once

#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Engine/Error.h"
#include "Graphics/Shader.h"
#include "ShaderGL.Generated.h"

/**
 * @brief Defines an OpenGL-backed shader.
 */
UM_CLASS()
class UShaderGL : public UShader
{
	UM_GENERATED_BODY();

	static constexpr uint32 InvalidShaderHandle = static_cast<uint32>(-1);

	/**
	 * @brief An enumeration of shader states.
	 */
	enum class EShaderState
	{
		NeedsSource,
		NeedsCompile,
		CompileFailed,
		CompileSuccess,
	};

public:

	/**
	 * @brief Attempts to compile this shader.
	 *
	 * @return Nothing if this shader compile's successfully, otherwise an error.
	 */
	[[nodiscard]] TErrorOr<void> Compile();

	/**
	 * @brief Gets this shader's compile log.
	 *
	 * @return This shader's compile log, or an error if one was encountered.
	 */
	[[nodiscard]] TErrorOr<FString> GetCompileLog() const;

	/**
	 * @brief Gets this shader's handle.
	 *
	 * @returns This shader's handle.
	 */
	[[nodiscard]] uint32 GetShaderHandle() const;

	/**
	 * @brief Checks to see if this shader is compiled.
	 *
	 * @returns True if this shader is compiled, otherwise false.
	 */
	[[nodiscard]] bool IsCompiled() const;

	/** @copydoc UShader::LoadFromBinary */
	virtual TErrorOr<void> LoadFromBinary(const void* bytes, int32 byteCount) override;

	/** @copydoc UShader::LoadFromText */
	virtual TErrorOr<void> LoadFromText(FStringView text) override;

protected:

	/** @copydoc UShaderGL::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UShaderGL::Destroyed */
	virtual void Destroyed() override;

private:

	uint32 m_Handle = InvalidShaderHandle;
	EShaderState m_State = EShaderState::NeedsSource;
};