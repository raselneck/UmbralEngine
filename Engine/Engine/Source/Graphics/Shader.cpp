#include "Engine/Logging.h"
#include "Graphics/Shader.h"
#include "HAL/File.h"

TErrorOr<void> UShader::LoadFromBinary(const void* bytes, const int32 byteCount)
{
	(void)bytes;
	(void)byteCount;

	UM_LOG(Fatal, "LoadFromBinary not implemented for shader type {}", GetType()->GetName());
	return {};
}

TErrorOr<void> UShader::LoadFromBinary(TSpan<const uint8> bytes)
{
	return LoadFromBinary(bytes.GetData(), bytes.Num());
}

TErrorOr<void> UShader::LoadFromFile(const FStringView filePath, const EShaderFileType fileType)
{
	switch (fileType)
	{
	case EShaderFileType::Binary:
	{
		TRY_EVAL(const TArray<uint8> fileBytes, FFile::ReadAllBytes(filePath));
		return LoadFromBinary(fileBytes.AsSpan());
	}

	case EShaderFileType::Text:
	{
		TRY_EVAL(const FString fileText, FFile::ReadAllText(filePath));
		return LoadFromText(fileText);
	}

	default:
		UM_ASSERT_NOT_REACHED_MSG("Unknown shader file type");
	}
}

TErrorOr<void> UShader::LoadFromText(const FStringView text)
{
	(void)text;

	UM_LOG(Fatal, "LoadFromText not implemented for shader type {}", GetType()->GetName());
	return {};
}

void UShader::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_ShaderType = context.GetParameterChecked<EShaderType>("type"_sv);
}