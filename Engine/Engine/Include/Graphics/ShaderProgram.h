#pragma once

#include "Engine/Error.h"
#include "Graphics/Color.h"
#include "Graphics/GraphicsResource.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "ShaderProgram.Generated.h"

class FLinearColor;
class FMatrix3;
class FMatrix4;
class FVector2;
class FVector3;
class FVector4;

/**
 * @brief Defines a shader program.
 */
UM_CLASS(Abstract)
class UShaderProgram : public UGraphicsResource
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Attaches a shader to this shader program.
	 *
	 * @param shader The shader to attach.
	 */
	[[nodiscard]] virtual bool AttachShader(TObjectPtr<UShader> shader);

	/**
	 * @brief Attempts to link all shaders that have been attached to this shader program.
	 *
	 * @return Nothing if linking was successful, otherwise the error that was encountered.
	 */
	[[nodiscard]] virtual TErrorOr<void> Link();

	/**
	 * @brief Attempts to set a color shader value.
	 *
	 * @param name The name of the property.
	 * @param value The value to set.
	 * @returns True if the value was set, otherwise false.
	 */
	[[nodiscard]] virtual bool SetColor(FStringView name, FColor value);

	/**
	 * @brief Attempts to set a float shader value.
	 *
	 * @param name The name of the property.
	 * @param value The value to set.
	 * @returns True if the value was set, otherwise false.
	 */
	[[nodiscard]] virtual bool SetFloat(FStringView name, float value);

	/**
	 * @brief Attempts to set a linear color shader value.
	 *
	 * @param name The name of the property.
	 * @param value The value to set.
	 * @returns True if the value was set, otherwise false.
	 */
	[[nodiscard]] virtual bool SetLinearColor(FStringView name, const FLinearColor& value);

	/**
	 * @brief Attempts to set a 3x3 matrix shader value.
	 *
	 * @param name The name of the property.
	 * @param value The value to set.
	 * @returns True if the value was set, otherwise false.
	 */
	[[nodiscard]] virtual bool SetMatrix3(FStringView name, const FMatrix3& value);

	/**
	 * @brief Attempts to set a 4x4 matrix shader value.
	 *
	 * @param name The name of the property.
	 * @param value The value to set.
	 * @returns True if the value was set, otherwise false.
	 */
	[[nodiscard]] virtual bool SetMatrix4(FStringView name, const FMatrix4& value);

	/**
	 * @brief Attempts to set a 2D texture shader value.
	 *
	 * @param name The name of the property.
	 * @param value The value to set.
	 * @returns True if the value was set, otherwise false.
	 */
	[[nodiscard]] virtual bool SetTexture2D(FStringView name, TObjectPtr<const UTexture2D> value);

	/**
	 * @brief Attempts to set a two component shader value.
	 *
	 * @param name The name of the property.
	 * @param value The value to set.
	 * @returns True if the value was set, otherwise false.
	 */
	[[nodiscard]] virtual bool SetVector2(FStringView name, const FVector2& value);

	/**
	 * @brief Attempts to set a three component shader value.
	 *
	 * @param name The name of the property.
	 * @param value The value to set.
	 * @returns True if the value was set, otherwise false.
	 */
	[[nodiscard]] virtual bool SetVector3(FStringView name, const FVector3& value);

	/**
	 * @brief Attempts to set a four component vector shader value.
	 *
	 * @param name The name of the property.
	 * @param value The value to set.
	 * @returns True if the value was set, otherwise false.
	 */
	[[nodiscard]] virtual bool SetVector4(FStringView name, const FVector4& value);
};