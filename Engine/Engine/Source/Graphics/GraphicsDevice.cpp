#include "Engine/Logging.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Image.h"

void UGraphicsDevice::BindIndexBuffer(const TObjectPtr<UIndexBuffer> indexBuffer)
{
	(void)indexBuffer;
	UM_ASSERT_NOT_REACHED();
}

void UGraphicsDevice::BindVertexBuffer(const TObjectPtr<UVertexBuffer> vertexBuffer)
{
	(void)vertexBuffer;
	UM_ASSERT_NOT_REACHED();
}

void UGraphicsDevice::Clear(EClearOptions clearOptions, const FLinearColor& color, float depth, int32 stencil)
{
	(void)clearOptions;
	(void)color;
	(void)depth;
	(void)stencil;
}

TObjectPtr<UIndexBuffer> UGraphicsDevice::CreateIndexBuffer(const EIndexBufferUsage usage)
{
	(void)usage;

	return nullptr;
}

TObjectPtr<UShader> UGraphicsDevice::CreateShader(EShaderType shaderType)
{
	(void)shaderType;

	return nullptr;
}

TObjectPtr<UShaderProgram> UGraphicsDevice::CreateShaderProgram()
{
	return nullptr;
}

TObjectPtr<UTexture2D> UGraphicsDevice::CreateTexture2D()
{
	UM_ASSERT_NOT_REACHED();
}

TObjectPtr<UVertexBuffer> UGraphicsDevice::CreateVertexBuffer(const EVertexBufferUsage usage)
{
	(void)usage;

	return nullptr;
}

void UGraphicsDevice::DrawIndexedVertices(const EPrimitiveType primitiveType)
{
	(void)primitiveType;
	UM_ASSERT_NOT_REACHED();
}

void UGraphicsDevice::DrawVertices(const EPrimitiveType primitiveType)
{
	(void)primitiveType;
	UM_ASSERT_NOT_REACHED();
}

EGraphicsApi UGraphicsDevice::GetApi() const
{
	UM_ASSERT_NOT_REACHED();
}

EGraphicsContextState UGraphicsDevice::SetActiveContext() const
{
	UM_ASSERT_NOT_REACHED();
}

void UGraphicsDevice::UseShaderProgram(TObjectPtr<UShaderProgram> shaderProgram)
{
	(void)shaderProgram;

	UM_ASSERT_NOT_REACHED();
}