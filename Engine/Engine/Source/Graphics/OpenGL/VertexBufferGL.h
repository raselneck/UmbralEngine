#pragma once

#include "Graphics/VertexBuffer.h"
#include "OpenGL/UmbralToGL.h"
#include "VertexBufferGL.Generated.h"

/**
 * @brief Defines an OpenGL-backed vertex buffer.
 */
UM_CLASS()
class UVertexBufferGL : public UVertexBuffer
{
	UM_GENERATED_BODY();

	static constexpr uint32 InvalidArrayHandle = static_cast<uint32>(-1);
	static constexpr uint32 InvalidBufferHandle = static_cast<uint32>(-1);

public:

	/**
	 * @brief Gets this vertex buffer's array handle.
	 *
	 * @return This vertex buffer's array handle.
	 */
	[[nodiscard]] uint32 GetArrayHandle() const
	{
		return m_ArrayHandle;
	}

	/**
	 * @brief Gets this vertex buffer's buffer handle.
	 *
	 * @return This vertex buffer's buffer handle.
	 */
	[[nodiscard]] uint32 GetBufferHandle() const
	{
		return m_BufferHandle;
	}

protected:

	/** @copydoc UObject::Destroyed */
	virtual void Destroyed() override;

	/** @copydoc UVertexBuffer::SetData */
	virtual void SetData(const void* data, int32 dataLength, const FVertexDeclaration& declaration, int32 vertexCount) override;

private:

	FVertexDeclaration m_VertexDeclaration;
	uint32 m_ArrayHandle = InvalidArrayHandle;
	uint32 m_BufferHandle = InvalidBufferHandle;
};