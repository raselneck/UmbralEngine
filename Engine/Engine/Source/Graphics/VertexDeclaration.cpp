#include "Graphics/VertexDeclaration.h"

/**
	 * @brief Gets the size, in bytes, of a vertex element.
	 *
	 * @param elementFormat The vertex element's format.
	 * @return The size, in bytes, of the vertex element.
 */
constexpr int32 GetVertexAttributeSize(const EVertexElementFormat elementFormat)
{
	switch (elementFormat)
	{
	case EVertexElementFormat::Single:              return 4;
	case EVertexElementFormat::Vector2:             return 8;
	case EVertexElementFormat::Vector3:             return 12;
	case EVertexElementFormat::Vector4:             return 16;
	case EVertexElementFormat::Color:               return 4;
	case EVertexElementFormat::Byte4:               return 4;
	case EVertexElementFormat::Short2:              return 4;
	case EVertexElementFormat::Short4:              return 8;
	case EVertexElementFormat::NormalizedShort2:    return 4;
	case EVertexElementFormat::NormalizedShort4:    return 8;
	case EVertexElementFormat::HalfVector2:         return 4;
	case EVertexElementFormat::HalfVector4:         return 8;
	default: UM_ASSERT_NOT_REACHED();
	}
}

static int32 CalculateVertexStride(const TArray<FVertexElement>& elements)
{
	int32 maxEnd = 0;

	for (const FVertexElement& element : elements)
	{
		const int32 end = element.Offset + GetVertexAttributeSize(element.ElementFormat);
		if (maxEnd < end)
		{
			maxEnd = end;
		}
	}

	return maxEnd;
}

FVertexDeclaration::FVertexDeclaration(std::initializer_list<FVertexElement> elements)
	: m_Elements { MoveTemp(elements) }
	, m_VertexStride { CalculateVertexStride(m_Elements) }
{
}

bool FVertexDeclaration::operator==(const FVertexDeclaration& other) const
{
	const int32 numElements = m_Elements.Num();
	if (numElements != other.GetElementCount())
	{
		return false;
	}

	for (int32 idx = 0; idx < numElements; ++idx)
	{
		if (m_Elements[idx] != other.m_Elements[idx])
		{
			return false;
		}
	}

	return true;
}