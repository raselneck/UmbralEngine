#include "Graphics/VertexBuffer.h"

void UVertexBuffer::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_Usage = context.GetParameterChecked<EVertexBufferUsage>("usage"_sv);
}

void UVertexBuffer::SetData(const void* data, const int32 dataLength, const FVertexDeclaration& declaration, const int32 vertexCount)
{
	(void)data;
	(void)dataLength;
	(void)declaration;
	(void)vertexCount;
}

void UVertexBuffer::DispatchSetData(const void* data, const int32 dataLength, const FVertexDeclaration& declaration, const int32 vertexCount)
{
	UM_ASSERT(declaration.IsEmpty() == false, "Attempting to set vertex data with empty vertex declaration");

	// Call virtual first so child classes can check if anything's changed
	SetData(data, dataLength, declaration, vertexCount);

	m_VertexDeclaration = declaration;
	m_VertexCount = vertexCount;
}