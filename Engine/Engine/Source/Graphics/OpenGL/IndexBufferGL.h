#pragma once

#include "Graphics/IndexBuffer.h"
#include "OpenGL/UmbralToGL.h"
#include "IndexBufferGL.Generated.h"

/**
 * @brief Defines an OpenGL-backed index buffer.
 */
UM_CLASS()
class UIndexBufferGL : public UIndexBuffer
{
	UM_GENERATED_BODY();

	static constexpr uint32 InvalidBufferHandle = static_cast<uint32>(-1);

public:

	/**
	 * @brief Gets this index buffer's buffer handle.
	 *
	 * @return This index buffer's buffer handle.
	 */
	[[nodiscard]] uint32 GetBufferHandle() const
	{
		return m_BufferHandle;
	}

protected:

	/** @copydoc UObject::Destroyed */
	virtual void Destroyed() override;

	/** @copydoc UIndexBuffer::SetData */
	virtual void SetData(const void* data, int32 dataLength, EIndexElementType indexType, int32 indexCount) override;

private:

	uint32 m_BufferHandle = InvalidBufferHandle;
};