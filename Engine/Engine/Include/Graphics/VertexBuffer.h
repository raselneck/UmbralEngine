#pragma once

#include "Containers/Array.h"
#include "Containers/StaticArray.h"
#include "Graphics/GraphicsResource.h"
#include "Graphics/Vertex.h"
#include "Graphics/VertexBufferUsage.h"
#include "Graphics/VertexDeclaration.h"
#include "Templates/TypeTraits.h"
#include "VertexBuffer.Generated.h"

/**
 * @brief Defines a vertex buffer.
 */
UM_CLASS(Abstract)
class UVertexBuffer : public UGraphicsResource
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets this vertex buffer's usage.
	 *
	 * @return This vertex buffer's usage.
	 */
	[[nodiscard]] EVertexBufferUsage GetUsage() const
	{
		return m_Usage;
	}

	/**
	 * @brief Gets the number of vertices currently stored in this vertex buffer.
	 *
	 * @return The number of vertices currently stored in this vertex buffer.
	 */
	[[nodiscard]] int32 GetVertexCount() const
	{
		return m_VertexCount;
	}

	/**
	 * @brief Gets the vertex declaration for the underlying vertex data.
	 *
	 * @return The vertex declaration for the underlying vertex data.
	 */
	[[nodiscard]] const FVertexDeclaration& GetVertexDeclaration() const
	{
		return m_VertexDeclaration;
	}

	/**
	 * @brief Sets this vertex buffer's data.
	 *
	 * @tparam VertexType The vertex type.
	 * @param vertices The vertices.
	 */
	template<typename VertexType>
	void SetData(const TArray<VertexType>& vertices)
	{
		static_assert(IsVertex<VertexType>);

		const int32 vertexCount = vertices.Num();
		DispatchSetData(vertices.GetData(), sizeof(VertexType) * vertexCount, VertexType::GetVertexDeclaration(), vertexCount);
	}

	/**
	 * @brief Sets this vertex buffer's data.
	 *
	 * @tparam VertexType The vertex type.
	 * @tparam NumVertices The number of vertices.
	 * @param vertices The vertices.
	 */
	template<typename VertexType, int32 NumVertices>
	void SetData(const TStaticArray<VertexType, NumVertices> &vertices)
	{
		static_assert(IsVertex<VertexType>);

		DispatchSetData(vertices.GetData(), sizeof(VertexType) * NumVertices, VertexType::GetVertexDeclaration(), NumVertices);
	}

	/**
	 * @brief Sets this vertex buffer's data.
	 *
	 * @tparam VertexType The vertex type.
	 * @tparam NumVertices The number of vertices.
	 * @param vertices The vertices.
	 */
	template<typename VertexType, int32 NumVertices>
	void SetData(const VertexType (&vertices)[NumVertices])
	{
		static_assert(IsVertex<VertexType>);

		DispatchSetData(vertices, sizeof(VertexType) * NumVertices, VertexType::GetVertexDeclaration(), NumVertices);
	}

	/**
	 * @brief Sets this vertex buffer's data.
	 *
	 * @tparam VertexType The vertex type.
	 * @param vertices The vertices.
	 * @param numVertices The number of vertices.
	 */
	template<typename VertexType>
	void SetData(const VertexType* vertices, const int32 numVertices)
	{
		static_assert(IsVertex<VertexType>);

		UM_ASSERT(numVertices >= 0, "Number of vertices must be positive");
		DispatchSetData(vertices, sizeof(VertexType) * numVertices, VertexType::GetVertexDeclaration(), numVertices);
	}

	/**
	 * @brief Sets this vertex buffer's data.
	 *
	 * @tparam VertexType The vertex type.
	 * @param vertices The vertices.
	 * @param numVertices The number of vertices.
	 * @param vertexDeclaration The declaration describing the layout of data in each \p VertexType.
	 */
	template<typename VertexType>
	void SetData(const VertexType* vertices, const int32 numVertices, const FVertexDeclaration& vertexDeclaration)
	{
		UM_ASSERT(numVertices >= 0, "Number of vertices must be positive");
		DispatchSetData(vertices, sizeof(VertexType) * numVertices, vertexDeclaration, numVertices);
	}

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/**
	 * @brief Sets this vertex buffer's data. Note that this is called before updating the vertex declaration and count.
	 *
	 * @param data The vertex data.
	 * @param dataLength The length of the vertex data.
	 * @param declaration The vertex declaration.
	 * @param vertexCount The number of vertices.
	 */
	virtual void SetData(const void* data, int32 dataLength, const FVertexDeclaration& declaration, int32 vertexCount);

private:

	/**
	 * @brief Sets this vertex buffer's declaration and vertex count, then dispatches data to the virtual SetData function.
	 *
	 * @param data The index data.
	 * @param dataLength The length of the index data.
	 * @param declaration The vertex declaration.
	 * @param vertexCount The number of vertices.
	 */
	void DispatchSetData(const void* data, int32 dataLength, const FVertexDeclaration& declaration, int32 vertexCount);

	FVertexDeclaration m_VertexDeclaration;
	EVertexBufferUsage m_Usage = EVertexBufferUsage::None;
	int32 m_VertexCount = 0;
};