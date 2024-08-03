#include "Engine/Logging.h"
#include "OpenGL/UmbralToGL.h"
#include "OpenGL/VertexBufferGL.h"

void UVertexBufferGL::Destroyed()
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

	if (m_ArrayHandle != InvalidArrayHandle)
	{
		GL_CHECK(glDeleteVertexArrays(1, &m_ArrayHandle));
	}
}

void UVertexBufferGL::SetData(const void* data, const int32 dataLength, const FVertexDeclaration& declaration, const int32 vertexCount)
{
	if (vertexCount < 0)
	{
		UM_LOG(Error, "Attempting to set a negative number of vertices");
		return;
	}

	if (declaration.GetElementCount() == 0)
	{
		UM_LOG(Error, "Attempting to set vertex data with invalid vertex declaration");
		return;
	}

	// Update the vertex declaration if necessary (and create the array handle)
	if (m_VertexDeclaration != declaration)
	{
		m_VertexDeclaration = declaration;

		if (m_BufferHandle != InvalidBufferHandle)
		{
			GL_CHECK(glDeleteBuffers(1, &m_BufferHandle));
		}
		if (m_ArrayHandle != InvalidArrayHandle)
		{
			GL_CHECK(glDeleteVertexArrays(1, &m_ArrayHandle));
		}

		GL_CHECK(glGenVertexArrays(1, &m_ArrayHandle));
	}

	if (m_BufferHandle == InvalidBufferHandle)
	{
		GL_CHECK(glGenBuffers(1, &m_BufferHandle));
	}

	GL_CHECK(glBindVertexArray(m_ArrayHandle));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_BufferHandle));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, dataLength, data, GL::GetVertexBufferUsage(GetUsage())));

	//GL_CHECK(glBindBuffer(m_Type, 0));
	//GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, GetHandle()));

	// Apply the vertex declaration
	const GLsizei stride = declaration.GetVertexStride();
	for (int32 idx = 0; idx < declaration.GetElementCount(); ++idx)
	{
		const FVertexElement& element = *declaration.GetElement(idx);

		const GLint size = GL::GetVertexAttributeElementCount(element.ElementFormat);
		const GLenum type = GL::GetVertexAttributeDataType(element.ElementFormat);
		const bool normalized = GL::IsVertexElementNormalized(element);
		const void* offset = reinterpret_cast<const void*>(static_cast<size_t>(element.Offset));

		GL_CHECK(glEnableVertexAttribArray(static_cast<GLuint>(idx)));
		GL_CHECK(glVertexAttribPointer(static_cast<GLuint>(idx), size, type, normalized, stride, offset));
	}

	//GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	//GL_CHECK(glBindVertexArray(0));
}