#pragma once

#include "Graphics/Color.h"
#include "Graphics/VertexDeclaration.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Templates/TypeTraits.h"

/**
 * @brief A macro for defining an FVertexElement.
 *
 * @param Type The owning type.
 * @param Member The name of the field member in the owning type.
 * @param Format The element's format.
 * @param Usage The element's usage.
 */
#define MAKE_VERTEX_ELEMENT(Type, Member, Format, Usage)            \
	FVertexElement {                                                \
		.Offset = static_cast<int32>(UM_OFFSET_OF(Type, Member)),   \
		.ElementFormat = EVertexElementFormat::Format,              \
		.ElementUsage = EVertexElementUsage::Usage,                 \
		.UsageIndex = 0                                             \
	}

/**
 * @brief Defines a vertex type that only contains a position.
 */
class FVertexPosition
{
public:

	/** @brief This vertex's position. */
	FVector3 Position;

	/**
	 * @brief Gets this vertex type's declaration.
	 *
	 * @returns This vertex type's declaration.
	 */
	static const FVertexDeclaration& GetVertexDeclaration();
};

/**
 * @brief Defines a vertex type that contains a position and a color.
 */
class FVertexPositionColor
{
public:

	/** @brief This vertex's position. */
	FVector3 Position;

	/** @brief This vertex's color. */
	FColor Color;

	/**
	 * @brief Gets this vertex type's declaration.
	 *
	 * @returns This vertex type's declaration.
	 */
	static const FVertexDeclaration& GetVertexDeclaration();
};

/**
 * @brief Defines a vertex type that contains a position, a color, and a normal.
 */
class FVertexPositionColorNormal
{
public:

	/** @brief This vertex's position. */
	FVector3 Position;

	/** @brief This vertex's color. */
	FColor Color;

	/** @brief This vertex's normal. */
	FVector3 Normal;

	/**
	 * @brief Gets this vertex type's declaration.
	 *
	 * @returns This vertex type's declaration.
	 */
	static const FVertexDeclaration& GetVertexDeclaration();
};

/**
 * @brief Defines a vertex type that contains a position, a color, and a texture coordinate.
 */
class FVertexPositionColorTexture
{
public:

	/** @brief This vertex's position. */
	FVector3 Position;

	/** @brief This vertex's color. */
	FColor Color;

	/** @brief This vertex's texture coordinate. */
	FVector2 UV;

	/**
	 * @brief Gets this vertex type's declaration.
	 *
	 * @returns This vertex type's declaration.
	 */
	static const FVertexDeclaration& GetVertexDeclaration();
};

/**
 * @brief Defines a vertex type that contains a position and a normal.
 */
class FVertexPositionNormal
{
public:

	/** @brief This vertex's position. */
	FVector3 Position;

	/** @brief This vertex's normal. */
	FVector3 Normal;

	/**
	 * @brief Gets this vertex type's declaration.
	 *
	 * @returns This vertex type's declaration.
	 */
	static const FVertexDeclaration& GetVertexDeclaration();
};

/**
 * @brief Defines a vertex type that contains a position, a normal, and a texture coordinate.
 */
class FVertexPositionNormalTexture
{
public:

	/** @brief This vertex's position. */
	FVector3 Position;

	/** @brief This vertex's normal. */
	FVector3 Normal;

	/** @brief This vertex's texture coordinate. */
	FVector2 UV;

	/**
	 * @brief Gets this vertex type's declaration.
	 *
	 * @returns This vertex type's declaration.
	 */
	static const FVertexDeclaration& GetVertexDeclaration();
};

/**
 * @brief Defines a vertex type that contains a position and a texture coordinate.
 */
class FVertexPositionTexture
{
public:

	/** @brief This vertex's position. */
	FVector3 Position;

	/** @brief This vertex's texture coordinate. */
	FVector2 UV;

	/**
	 * @brief Gets this vertex type's declaration.
	 *
	 * @returns This vertex type's declaration.
	 */
	static const FVertexDeclaration& GetVertexDeclaration();
};

/**
 * @brief Defines a type trait that can be used to verify if a type is a valid vertex type.
 */
template<typename T>
struct TIsVertex
{
	static constexpr bool Value = requires()
	{
		T::GetVertexDeclaration();
		requires(TIsSame<const FVertexDeclaration&, decltype(T::GetVertexDeclaration())>::Value);
	};
};

template<typename T>
inline constexpr bool IsVertex = TIsVertex<T>::Value;