#pragma once

#include "Containers/Span.h"
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Engine/Error.h"
#include "Graphics/GraphicsResource.h"
#include "Graphics/ShaderType.h"
#include "Shader.Generated.h"

/**
 * @brief An enumeration of file types for shaders.
 */
UM_ENUM()
enum class EShaderFileType : uint8
{
	Binary,
	Text
};

/**
 * @brief Defines a shader.
 */
UM_CLASS(Abstract)
class UShader : public UGraphicsResource
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets this shader's type.
	 *
	 * @returns This shader's type.
	 */
	[[nodiscard]] EShaderType GetShaderType() const
	{
		return m_ShaderType;
	}

	/**
	 * @brief Loads shader data from a binary source.
	 *
	 * @param bytes The shader's bytes.
	 * @param byteCount The number of shader bytes.
	 * @return The error encountered while loading the shader binary, otherwise nothing on success.
	 */
	[[nodiscard]] virtual TErrorOr<void> LoadFromBinary(const void* bytes, int32 byteCount);

	/**
	 * @brief Loads shader data from a binary source.
	 *
	 * @param bytes The shader's byte span.
	 * @return The error encountered while loading the shader binary, otherwise nothing on success.
	 */
	[[nodiscard]] TErrorOr<void> LoadFromBinary(TSpan<const uint8> bytes);

	/**
	 * @brief Loads shader data from a file.
	 *
	 * @param filePath The file's path.
	 * @param fileType The type of file.
	 * @return The error encountered while loading the shader file, otherwise nothing on success.
	 */
	[[nodiscard]] TErrorOr<void> LoadFromFile(FStringView filePath, EShaderFileType fileType);

	/**
	 * @brief Loads shader data from a text source.
	 *
	 * @param text The shader's text.
	 * @return The error encountered while loading the shader text, otherwise nothing on success.
	 */
	[[nodiscard]] virtual TErrorOr<void> LoadFromText(FStringView text);

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

private:

	EShaderType m_ShaderType = EShaderType::None;
};