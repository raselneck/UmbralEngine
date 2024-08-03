#include "Graphics/IndexBuffer.h"

void UIndexBuffer::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_Usage = context.GetParameterChecked<EIndexBufferUsage>("usage"_sv);
}

void UIndexBuffer::SetData(const void* data, const int32 dataLength, const EIndexElementType elementType, const int32 elementCount)
{
	(void)data;
	(void)dataLength;
	(void)elementType;
	(void)elementCount;
}

void UIndexBuffer::DispatchSetData(const void* data, int32 dataLength, const EIndexElementType elementType, const int32 elementCount)
{
	// Call virtual first so child classes can check if anything's changed
	SetData(data, dataLength, elementType, elementCount);

	m_ElementType = elementType;
	m_ElementCount = elementCount;
}