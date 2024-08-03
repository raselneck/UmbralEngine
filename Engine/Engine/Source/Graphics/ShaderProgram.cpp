#include "Graphics/ShaderProgram.h"

bool UShaderProgram::AttachShader(TObjectPtr<UShader> shader)
{
	(void)shader;

	return false;
}

TErrorOr<void> UShaderProgram::Link()
{
	return MAKE_ERROR("Link has not been implemented by this shader program type ({})", GetType()->GetName());
}

bool UShaderProgram::SetColor(FStringView name, FColor value)
{
	(void)name;
	(void)value;

	return false;
}

bool UShaderProgram::SetFloat(FStringView name, float value)
{
	(void)name;
	(void)value;

	return false;
}

bool UShaderProgram::SetLinearColor(FStringView name, const FLinearColor& value)
{
	(void)name;
	(void)value;

	return false;
}

bool UShaderProgram::SetMatrix3(FStringView name, const FMatrix3& value)
{
	(void)name;
	(void)value;

	return false;
}

bool UShaderProgram::SetMatrix4(FStringView name, const FMatrix4& value)
{
	(void)name;
	(void)value;

	return false;
}

bool UShaderProgram::SetTexture2D(FStringView name, TObjectPtr<const UTexture2D> value)
{
	(void)name;
	(void)value;

	return false;
}

bool UShaderProgram::SetVector2(FStringView name, const FVector2& value)
{
	(void)name;
	(void)value;

	return false;
}

bool UShaderProgram::SetVector3(FStringView name, const FVector3& value)
{
	(void)name;
	(void)value;

	return false;
}

bool UShaderProgram::SetVector4(FStringView name, const FVector4& value)
{
	(void)name;
	(void)value;

	return false;
}