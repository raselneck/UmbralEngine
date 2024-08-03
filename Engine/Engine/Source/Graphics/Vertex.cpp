#include "Graphics/Vertex.h"
#include <cstddef> /* For offsetof */

///////////////////////////////////////////////////////////////////////////////
// FVertexPosition

const FVertexDeclaration& FVertexPosition::GetVertexDeclaration()
{
	static FVertexDeclaration declaration
	{{
		MAKE_VERTEX_ELEMENT(FVertexPosition, Position, Vector3, Position)
	}};

	return declaration;
}

///////////////////////////////////////////////////////////////////////////////
// FVertexPositionColor

const FVertexDeclaration& FVertexPositionColor::GetVertexDeclaration()
{
	static FVertexDeclaration declaration
	{{
		MAKE_VERTEX_ELEMENT(FVertexPositionColor, Position, Vector3, Position),
		MAKE_VERTEX_ELEMENT(FVertexPositionColor, Color, Color, Color)
	}};

	return declaration;
}

///////////////////////////////////////////////////////////////////////////////
// FVertexPositionColorNormal

const FVertexDeclaration& FVertexPositionColorNormal::GetVertexDeclaration()
{
	static FVertexDeclaration declaration
	{{
		MAKE_VERTEX_ELEMENT(FVertexPositionColorNormal, Position, Vector3, Position),
		MAKE_VERTEX_ELEMENT(FVertexPositionColorNormal, Color, Color, Color),
		MAKE_VERTEX_ELEMENT(FVertexPositionColorNormal, Normal, Vector3, Normal)
	}};

	return declaration;
}

///////////////////////////////////////////////////////////////////////////////
// FVertexPositionColorTexture

const FVertexDeclaration& FVertexPositionColorTexture::GetVertexDeclaration()
{
	static FVertexDeclaration declaration
	{{
		MAKE_VERTEX_ELEMENT(FVertexPositionColorTexture, Position, Vector3, Position),
		MAKE_VERTEX_ELEMENT(FVertexPositionColorTexture, Color, Color, Color),
		MAKE_VERTEX_ELEMENT(FVertexPositionColorTexture, UV, Vector2, TextureCoordinate)
	}};

	return declaration;
}

///////////////////////////////////////////////////////////////////////////////
// FVertexPositionNormal

const FVertexDeclaration& FVertexPositionNormal::GetVertexDeclaration()
{
	static FVertexDeclaration declaration
	{{
		MAKE_VERTEX_ELEMENT(FVertexPositionNormal, Position, Vector3, Position),
		MAKE_VERTEX_ELEMENT(FVertexPositionNormal, Normal, Vector3, Normal)
	}};

	return declaration;
}

///////////////////////////////////////////////////////////////////////////////
// FVertexPositionNormalTexture

const FVertexDeclaration& FVertexPositionNormalTexture::GetVertexDeclaration()
{
	static FVertexDeclaration declaration
	{{
		MAKE_VERTEX_ELEMENT(FVertexPositionNormalTexture, Position, Vector3, Position),
		MAKE_VERTEX_ELEMENT(FVertexPositionNormalTexture, Normal, Vector3, Normal),
		MAKE_VERTEX_ELEMENT(FVertexPositionNormalTexture, UV, Vector2, TextureCoordinate)
	}};

	return declaration;
}

///////////////////////////////////////////////////////////////////////////////
// FVertexPositionTexture

const FVertexDeclaration& FVertexPositionTexture::GetVertexDeclaration()
{
	static FVertexDeclaration declaration
	{{
		MAKE_VERTEX_ELEMENT(FVertexPositionTexture, Position, Vector3, Position),
		MAKE_VERTEX_ELEMENT(FVertexPositionTexture, UV, Vector2, TextureCoordinate)
	}};

	return declaration;
}

///////////////////////////////////////////////////////////////////////////////
// Static assertions to ensure all predefined vertex types are valid

#define CHECK_VERTEX_TYPE(Type) static_assert(TIsVertex<Type>::Value, #Type " is an invalid vertex type")
CHECK_VERTEX_TYPE(FVertexPosition);
CHECK_VERTEX_TYPE(FVertexPositionColor);
CHECK_VERTEX_TYPE(FVertexPositionColorNormal);
CHECK_VERTEX_TYPE(FVertexPositionColorTexture);
CHECK_VERTEX_TYPE(FVertexPositionNormalTexture);
CHECK_VERTEX_TYPE(FVertexPositionTexture);
#undef CHECK_VERTEX_TYPE