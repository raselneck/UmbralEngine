#pragma once

#include "Containers/Array.h"
#include "Containers/StaticArray.h"
#include "Graphics/GraphicsResource.h"
#include "Graphics/IndexBufferUsage.h"
#include "Graphics/IndexElementType.h"
#include "Templates/IntegralConstant.h"
#include "IndexBuffer.Generated.h"

/**
 * @brief Defines an index buffer.
 */
UM_CLASS(Abstract)
class UIndexBuffer : public UGraphicsResource
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets the total number of elements in this index buffer.
	 *
	 * @return The total number of elements in this index buffer.
	 */
	[[nodiscard]] int32 GetElementCount() const
	{
		return m_ElementCount;
	}

	/**
	 * @brief Gets the underlying type of the elements in this index buffer.
	 *
	 * @return The underlying type of the elements in this index buffer.
	 */
	[[nodiscard]] EIndexElementType GetElementType() const
	{
		return m_ElementType;
	}

	/**
	 * @brief Gets this index buffer's usage.
	 *
	 * @return This index buffer's usage.
	 */
	[[nodiscard]] EIndexBufferUsage GetUsage() const
	{
		return m_Usage;
	}

	/**
	 * @brief Sets this index buffer's data.
	 *
	 * @tparam ElementType The index element type.
	 * @param indices The indices to set.
	 */
	template<typename ElementType>
	void SetData(const TArray<ElementType>& indices)
	{
		static_assert(Private::IsValidIndexType<ElementType>);

		constexpr EIndexElementType indexType = Private::TFindIndexType<ElementType>::Value;
		DispatchSetData(indices.GetData(), sizeof(ElementType) * indices.Num(), indexType, indices.Num());
	}

	/**
	 * @brief Sets this index buffer's data.
	 *
	 * @tparam ElementType The index element type.
	 * @tparam NumElements The number of elements.
	 * @param indices The indices to set.
	 */
	template<typename ElementType, int32 NumElements>
	void SetData(const TStaticArray<ElementType, NumElements>& indices)
	{
		static_assert(Private::IsValidIndexType<ElementType>);

		constexpr EIndexElementType indexType = Private::TFindIndexType<ElementType>::Value;
		DispatchSetData(indices.GetData(), sizeof(ElementType) * NumElements, indexType, NumElements);
	}

	/**
	 * @brief Sets this index buffer's data.
	 *
	 * @tparam ElementType The index element type.
	 * @tparam NumElements The number of elements.
	 * @param indices The indices to set.
	 */
	template<typename ElementType, int32 NumElements>
	void SetData(const ElementType (&indices)[NumElements])
	{
		static_assert(Private::IsValidIndexType<ElementType>);

		constexpr EIndexElementType indexType = Private::TFindIndexType<ElementType>::Value;
		DispatchSetData(indices, sizeof(ElementType) * NumElements, indexType, NumElements);
	}

	/**
	 * @brief Sets this index buffer's data.
	 *
	 * @tparam ElementType The index element type.
	 * @param indices The indices to set.
	 * @param numElements The number of elements.
	 */
	template<typename ElementType>
	void SetData(const ElementType* indices, const int32 numIndices)
	{
		static_assert(Private::IsValidIndexType<ElementType>);

		UM_ASSERT(numIndices >= 0, "Number of indices must be positive");

		constexpr EIndexElementType indexType = Private::TFindIndexType<ElementType>::Value;
		DispatchSetData(indices, sizeof(ElementType) * numIndices, indexType, numIndices);
	}

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/**
	 * @brief Sets this index buffer's data. Note that this is called before updating the element type and count.
	 *
	 * @param data The index data.
	 * @param dataLength The length of the index data.
	 * @param elementType The index element type.
	 * @param elementCount The number of indices.
	 */
	virtual void SetData(const void* data, int32 dataLength, EIndexElementType elementType, int32 elementCount);

private:

	/**
	 * @brief Sets this index buffer's element type and count, then dispatches data to the virtual SetData function.
	 *
	 * @param data The index data.
	 * @param dataLength The length of the index data.
	 * @param elementType The index element type.
	 * @param elementCount The number of indices.
	 */
	void DispatchSetData(const void* data, int32 dataLength, EIndexElementType elementType, int32 elementCount);

	EIndexBufferUsage m_Usage = EIndexBufferUsage::None;
	EIndexElementType m_ElementType = EIndexElementType::None;
	int32 m_ElementCount = 0;
};