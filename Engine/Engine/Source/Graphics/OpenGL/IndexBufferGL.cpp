#include "Engine/Assert.h"
#include "Engine/Logging.h"
#include "OpenGL/IndexBufferGL.h"

void UIndexBufferGL::Destroyed()
{
	Super::Destroyed();

	if (SetActiveContextIfPossible() == EContextState::Unavailable)
	{
		return;
	}

	if (m_BufferHandle != InvalidBufferHandle)
	{
		GL_CHECK(glDeleteBuffers(1, &m_BufferHandle));
	}
}

void UIndexBufferGL::SetData(const void* data, const int32 dataLength, const EIndexElementType indexType, const int32 indexCount)
{
	(void)indexType;

	if (indexCount < 0)
	{
		UM_LOG(Error, "Attempting to set a negative number of indices");
		return;
	}

	if (m_BufferHandle == InvalidBufferHandle)
	{
		glGenBuffers(1, &m_BufferHandle);
	}

	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferHandle));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataLength, data, GL::GetIndexBufferUsage(GetUsage())));
	//GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}