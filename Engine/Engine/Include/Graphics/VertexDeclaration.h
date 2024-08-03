#pragma once

#include "Engine/IntTypes.h"
#include "Engine/Hashing.h"
#include "Containers/Array.h"
#include "Templates/TypeTraits.h"
#include <initializer_list>

/**
 * @brief An enumeration of possible vertex element formats.
 */
enum class EVertexElementFormat : uint8
{
	/**
	 * @brief Single 32-bit floating point number.
	 */
	Single,

	/**
	 * @brief Two component 32-bit floating point number.
	 */
	Vector2,

	/**
	 * @brief Three component 32-bit floating point number.
	 */
	Vector3,

	/**
	 * @brief Four component 32-bit floating point number.
	 */
	Vector4,

	/**
	 * @brief Four component, packed unsigned byte, mapped to 0 to 1 range.
	 */
	Color,

	/**
	 * @brief Four component unsigned byte.
	 */
	Byte4,

	/**
	 * @brief Two component signed 16-bit integer.
	 */
	Short2,

	/**
	 * @brief Four component signed 16-bit integer.
	 */
	Short4,

	/**
	 * @brief Normalized, two component signed 16-bit integer.
	 */
	NormalizedShort2,

	/**
	 * @brief Normalized, four component signed 16-bit integer.
	 */
	NormalizedShort4,

	/**
	 * @brief Two component 16-bit floating point number.
	 */
	HalfVector2,

	/**
	 * @brief Four component 16-bit floating point number.
	 */
	HalfVector4
};

/**
 * @brief An enumeration of possible vertex element usages.
 */
enum class EVertexElementUsage : uint8
{
	/**
	 * @brief Position data.
	 */
	Position,

	/**
	 * @brief Color data.
	 */
	Color,

	/**
	 * @brief Texture coordinate data or can be used for user-defined data.
	 */
	TextureCoordinate,

	/**
	 * @brief Normal data.
	 */
	Normal,

	/**
	 * @brief Binormal data.
	 */
	Binormal,

	/**
	 * @brief Tangent data.
	 */
	Tangent,

	/**
	 * @brief Blending indices data.
	 */
	BlendIndices,

	/**
	 * @brief Blending weight data.
	 */
	BlendWeight,

	/**
	 * @brief Depth data.
	 */
	Depth,

	/**
	 * @brief Fog data.
	 */
	Fog,

	/**
	 * @brief Point size data. Usable for drawing point sprites.
	 */
	PointSize,

	/**
	 * @brief Sampler data for specifies the displacement value to look up.
	 */
	Sample,

	/**
	 * @brief Single, positive float value, specifies a tessellation factor used in the tessellation unit to control the rate of tessellation.
	 */
	TessellateFactor
};

/**
 * @brief Defines a vertex element, which is a single entry in a vertex declaration.
 */
class FVertexElement
{
public:

	/** @brief This elements offset in bytes. */
	int32 Offset = 0;

	/** @brief This element's format. */
	EVertexElementFormat ElementFormat;

	/** @brief This element's usage. */
	EVertexElementUsage ElementUsage;

	/** @brief This elements usage index. */
	int32 UsageIndex = 0;

	/**
	 * @brief Checks to see if this vertex element is equivalent to another.
	 *
	 * @param Other The other vertex element.
	 * @return True if this vertex element is equivalent to another, otherwise false.
	 */
	constexpr bool operator==(const FVertexElement& Other) const
	{
		return Offset == Other.Offset &&
		       ElementFormat == Other.ElementFormat &&
		       ElementUsage == Other.ElementUsage &&
		       UsageIndex == Other.UsageIndex;
	}

	/**
	 * @brief Checks to see if this vertex element is not equivalent to another.
	 *
	 * @param Other The other vertex element.
	 * @return True if this vertex element is not equivalent to another, otherwise false.
	 */
	constexpr bool operator!=(const FVertexElement& Other) const
	{
		return Offset != Other.Offset ||
		       ElementFormat != Other.ElementFormat ||
		       ElementUsage != Other.ElementUsage ||
		       UsageIndex != Other.UsageIndex;
	}
};

/**
 * @brief Defines a vertex declaration, which defines the layout of a vertex.
 */
class FVertexDeclaration final
{
public:

	using ArrayType = TArray<FVertexElement>;
	using SizeType = typename ArrayType::SizeType;

	/**
	 * @brief Sets default values for this vertex declaration's properties.
	 */
	FVertexDeclaration() = default;

	/**
	 * @brief Sets default values for this vertex declaration's properties.
	 *
	 * @param elements The vertex declaration's elements.
	 */
	FVertexDeclaration(std::initializer_list<FVertexElement> elements);

	/**
	 * @brief Gets the total number of elements in this vertex declaration.
	 *
	 * @return The total number of elements in this vertex declaration.
	 */
	[[nodiscard]] int32 GetElementCount() const
	{
		return m_Elements.Num();
	}

	/**
	 * @brief Gets the vertex element at the given index.
	 *
	 * @param index The index of the vertex element.
	 * @return The vertex element at the given index, or nullptr if the index is out of bounds.
	 */
	[[nodiscard]] const FVertexElement* GetElement(const SizeType index) const
	{
		return m_Elements.IsValidIndex(index) ? &m_Elements[index] : nullptr;
	}

	/**
	 * @brief Gets the total number of bytes between each vertex in a packed vertex array.
	 *
	 * @return The total number of bytes between each vertex in a packed vertex array.
	 */
	[[nodiscard]] int32 GetVertexStride() const
	{
		return m_VertexStride;
	}

	/**
	 * @brief Checks to see if this vertex declaration is empty.
	 *
	 * @return True if this vertex declaration is empty, otherwise false.
	 */
	[[nodiscard]] bool IsEmpty() const
	{
		return m_Elements.IsEmpty();
	}

	/**
	 * @brief Checks to see if this vertex declaration is equivalent to another.
	 *
	 * @param other The other vertex declaration.
	 * @return True if this vertex declaration is equivalent to another, otherwise false.
	 */
	bool operator==(const FVertexDeclaration& other) const;

	/**
	 * @brief Checks to see if this vertex declaration is not equivalent to another.
	 *
	 * @param other The other vertex declaration.
	 * @return True if this vertex declaration is not equivalent to another, otherwise false.
	 */
	bool operator!=(const FVertexDeclaration& other) const
	{
		return this->operator==(other) == false;
	}

	// STL compatibility BEGIN
	[[nodiscard]] auto begin() const noexcept { return m_Elements.begin(); }
	[[nodiscard]] auto cbegin() const noexcept { return m_Elements.cbegin(); }
	[[nodiscard]] auto end() const noexcept { return m_Elements.end(); }
	[[nodiscard]] auto cend() const noexcept { return m_Elements.cend(); }
	// STL compatibility END

private:

	ArrayType m_Elements;
	int32 m_VertexStride;
};