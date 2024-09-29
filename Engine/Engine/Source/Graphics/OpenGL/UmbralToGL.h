#pragma once

#include "Containers/StringView.h"
#include "Graphics/BlendFunction.h"
#include "Graphics/BlendMode.h"
#include "Graphics/CompareFunction.h"
#include "Graphics/CullMode.h"
#include "Graphics/DepthFormat.h"
#include "Graphics/IndexBufferUsage.h"
#include "Graphics/IndexElementType.h"
#include "Graphics/PrimitiveType.h"
#include "Graphics/Shader.h"
#include "Graphics/StencilOperation.h"
#include "Graphics/TextureAddressMode.h"
#include "Graphics/TextureFilter.h"
#include "Graphics/TextureFormat.h"
#include "Graphics/VertexBufferUsage.h"
#include "Graphics/VertexDeclaration.h"
#include "Engine/Assert.h"
#include "Misc/SourceLocation.h"

#if WITH_ANGLE
#	include <GLES3/gl31.h>
#else
#	include <glad/glad.h>
#endif

#if UMBRAL_DEBUG
#	define GL_CHECK(Call) \
		do                                                                                  \
		{                                                                                   \
			(void)SetActiveContext();                                                       \
			if (::GL::CheckForActiveContext(this))                                          \
			{                                                                               \
				UMBRAL_DEBUG_BREAK();                                                       \
			}                                                                               \
			Call;                                                                           \
			if (::GL::CheckForError(UM_JOIN(#Call, _sv), UMBRAL_SOURCE_LOCATION))           \
			{                                                                               \
				UMBRAL_DEBUG_BREAK();                                                       \
			}                                                                               \
		} while (0)
#else
#	define GL_CHECK(Call) Call
#endif

namespace GL
{
	/**
	 * @brief Checks to see if the active OpenGL context matches that of the given graphics device.
	 *
	 * @param graphicsDevice The graphics device.
	 * @return True if there is an active OpenGL context mismatch, otherwise false.
	 */
	[[nodiscard]] bool CheckForActiveContext(const UGraphicsDevice* graphicsDevice);

	/**
	 * @brief Checks to see if the active OpenGL context matches the one the given graphics resource was created with.
	 *
	 * @param graphicsResource The graphics resource.
	 * @return True if there is an active OpenGL context mismatch, otherwise false.
	 */
	[[nodiscard]] bool CheckForActiveContext(const UGraphicsResource* graphicsResource);

	/**
	 * @brief Checks for an OpenGL error.
	 *
	 * @param call The text of the OpenGL call.
	 * @param sourceLocation The location in source the call was executed from.
	 * @return True if an error was reported, otherwise false.
	 */
	[[nodiscard]] bool CheckForError(FStringView call, FCppSourceLocation sourceLocation);

	/**
	 * @brief Gets the OpenGL blend mode from the given Umbral blend mode.
	 *
	 * @param blendMode The Umbral blend mode.
	 * @return The OpenGL blend mode.
	 */
	[[nodiscard]] constexpr GLenum GetBlendMode(const EBlendMode blendMode)
	{
		switch (blendMode)
		{
		case EBlendMode::One:                       return GL_ONE;
		case EBlendMode::Zero:                      return GL_ZERO;
		case EBlendMode::SourceColor:               return GL_SRC_COLOR;
		case EBlendMode::InverseSourceColor:        return GL_ONE_MINUS_SRC_COLOR;
		case EBlendMode::SourceAlpha:               return GL_SRC_ALPHA;
		case EBlendMode::InverseSourceAlpha:        return GL_ONE_MINUS_SRC_ALPHA;
		case EBlendMode::DestinationColor:          return GL_DST_COLOR;
		case EBlendMode::InverseDestinationColor:   return GL_ONE_MINUS_DST_COLOR;
		case EBlendMode::DestinationAlpha:          return GL_DST_ALPHA;
		case EBlendMode::InverseDestinationAlpha:   return GL_ONE_MINUS_DST_ALPHA;
		case EBlendMode::BlendFactor:               return GL_CONSTANT_COLOR;
		case EBlendMode::InverseBlendFactor:        return GL_ONE_MINUS_CONSTANT_COLOR;
		case EBlendMode::SourceAlphaSaturation:     return GL_SRC_ALPHA_SATURATE;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the OpenGL blend equation from the given Umbral blend function.
	 *
	 * @param blendFunction The Umbral blend function.
	 * @return The OpenGL blend equation.
	 */
	[[nodiscard]] constexpr GLenum GetBlendEquation(const EBlendFunction blendFunction)
	{
		switch (blendFunction)
		{
		case EBlendFunction::Add:               return GL_FUNC_ADD;
		case EBlendFunction::Subtract:          return GL_FUNC_SUBTRACT;
		case EBlendFunction::ReverseSubtract:   return GL_FUNC_REVERSE_SUBTRACT;
		case EBlendFunction::Max:               return GL_MAX;
		case EBlendFunction::Min:               return GL_MIN;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the OpenGL compare function from the given Umbral compare function.
	 *
	 * @param compareFunction The Umbral compare function.
	 * @return The OpenGL compare function.
	 */
	[[nodiscard]] constexpr GLenum GetCompareFunction(const ECompareFunction compareFunction)
	{
		switch (compareFunction)
		{
		case ECompareFunction::Always:          return GL_ALWAYS;
		case ECompareFunction::Never:           return GL_NEVER;
		case ECompareFunction::Less:            return GL_LESS;
		case ECompareFunction::LessEqual:       return GL_LEQUAL;
		case ECompareFunction::Equal:           return GL_EQUAL;
		case ECompareFunction::GreaterEqual:    return GL_GEQUAL;
		case ECompareFunction::Greater:         return GL_GREATER;
		case ECompareFunction::NotEqual:        return GL_NOTEQUAL;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the OpenGL cull mode from the given Umbral cull mode.
	 *
	 * @param stencilOperation The Umbral cull mode.
	 * @return The OpenGL cull mode.
	 */
	[[nodiscard]] constexpr GLenum GetCullMode(const ECullMode cullMode)
	{
		switch (cullMode)
		{
		case ECullMode::CullClockwiseFace:          return GL_CW;
		case ECullMode::CullCounterClockwiseFace:   return GL_CCW;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the depth bias scale for the given depth format.
	 *
	 * @param depthFormat The depth format.
	 * @return The depth bias scale.
	 */
	[[nodiscard]] constexpr float GetDepthBiasScale(const EDepthFormat depthFormat)
	{
		switch (depthFormat)
		{
		case EDepthFormat::None:            return 0.0f;
		case EDepthFormat::Depth16:         return static_cast<float>((1 << 16) - 1);
		case EDepthFormat::Depth24:         return static_cast<float>((1 << 24) - 1);
		case EDepthFormat::Depth24Stencil8: return static_cast<float>((1 << 24) - 1);
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the depth-stencil attachment for the given depth format.
	 *
	 * @param depthFormat The depth format.
	 * @return The depth-stencil attachment.
	 */
	[[nodiscard]] constexpr GLenum GetDepthStencilAttachment(const EDepthFormat depthFormat)
	{
		switch (depthFormat)
		{
		case EDepthFormat::Depth16:         return GL_DEPTH_ATTACHMENT;
		case EDepthFormat::Depth24:         return GL_DEPTH_ATTACHMENT;
		case EDepthFormat::Depth24Stencil8: return GL_DEPTH_STENCIL_ATTACHMENT;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the depth-stencil storage for the given depth format.
	 *
	 * @param depthFormat The depth format.
	 * @return The depth-stencil storage.
	 */
	[[nodiscard]] constexpr GLenum GetDepthStencilStorage(const EDepthFormat depthFormat)
	{
		switch (depthFormat)
		{
		case EDepthFormat::Depth16:         return GL_DEPTH_COMPONENT16;
		case EDepthFormat::Depth24:         return GL_DEPTH_COMPONENT24;
		case EDepthFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
		default: UM_ASSERT_NOT_REACHED();
		}
	};

	/**
	 * @brief Gets the equivalent OpenGL index buffer usage for the given Umbral index buffer usage.
	 *
	 * @param shaderType The Umbral index buffer usage.
	 * @return The OpenGL index buffer usage.
	 */
	[[nodiscard]] constexpr GLenum GetIndexBufferUsage(const EIndexBufferUsage bufferUsage)
	{
		switch (bufferUsage)
		{
		case EIndexBufferUsage::Static:    return GL_STATIC_DRAW;
		case EIndexBufferUsage::Dynamic:	return GL_STREAM_DRAW;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the equivalent OpenGL index element type for the given Umbral index element type.
	 *
	 * @param indexElementType The Umbral index element type.
	 * @return The OpenGL index element type.
	 */
	[[nodiscard]] constexpr GLenum GetIndexElementType(const EIndexElementType indexElementType)
	{
		switch (indexElementType)
		{
		case EIndexElementType::Byte:   return GL_UNSIGNED_BYTE;
		case EIndexElementType::Short:  return GL_UNSIGNED_SHORT;
		case EIndexElementType::Int:    return GL_UNSIGNED_INT;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the equivalent OpenGL primitive type for the given Umbral primitive type.
	 *
	 * @param primitiveType The Umbral primitive type.
	 * @return The OpenGL primitive type.
	 */
	[[nodiscard]] constexpr GLenum GetPrimitiveType(const EPrimitiveType primitiveType)
	{
		switch (primitiveType)
		{
		case EPrimitiveType::PointList:     return GL_POINTS;
		case EPrimitiveType::LineStrip:     return GL_LINE_STRIP;
		case EPrimitiveType::LineLoop:      return GL_LINE_LOOP;
		case EPrimitiveType::LineList:      return GL_LINES;
		case EPrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
		case EPrimitiveType::TriangleFan:   return GL_TRIANGLE_FAN;
		case EPrimitiveType::TriangleList:  return GL_TRIANGLES;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the equivalent OpenGL shader type for the given Umbral shader type.
	 *
	 * @param shaderType The Umbral shader type.
	 * @return The OpenGL shader type.
	 */
	[[nodiscard]] constexpr GLenum GetShaderType(const EShaderType shaderType)
	{
		switch (shaderType)
		{
		case EShaderType::Vertex: return GL_VERTEX_SHADER;
		case EShaderType::Fragment: return GL_FRAGMENT_SHADER;
		case EShaderType::Compute: return GL_COMPUTE_SHADER;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the OpenGL stencil operation from the given Umbral stencil operation.
	 *
	 * @param stencilOperation The Umbral stencil operation.
	 * @return The OpenGL stencil operation.
	 */
	[[nodiscard]] constexpr GLenum GetStencilOperation(const EStencilOperation stencilOperation)
	{
		switch (stencilOperation)
		{
		case EStencilOperation::Keep:                   return GL_KEEP;
		case EStencilOperation::Zero:                   return GL_ZERO;
		case EStencilOperation::Replace:                return GL_REPLACE;
		case EStencilOperation::Increment:              return GL_INCR_WRAP;
		case EStencilOperation::Decrement:              return GL_DECR_WRAP;
		case EStencilOperation::IncrementSaturation:    return GL_INCR;
		case EStencilOperation::DecrementSaturation:    return GL_DECR;
		case EStencilOperation::Invert:                 return GL_INVERT;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the OpenGL texture format for the given Umbral texture format.
	 *
	 * @param textureFormat The Umbral texture format.
	 * @return The OpenGL texture format.
	 */
	[[nodiscard]] constexpr GLenum GetTextureFormat(const ETextureFormat textureFormat)
	{
		switch (textureFormat)
		{
		case ETextureFormat::R8_UNORM:				return GL_RED;
		case ETextureFormat::R8_UINT:				return GL_RED_INTEGER;
		case ETextureFormat::R8_SNORM:				return GL_RED;
		case ETextureFormat::R8_SINT:				return GL_RED_INTEGER;

		case ETextureFormat::R16_FLOAT:				return GL_RED;
		case ETextureFormat::R16_UINT:				return GL_RED_INTEGER;
		case ETextureFormat::R16_SINT:				return GL_RED_INTEGER;

		case ETextureFormat::R32_FLOAT:				return GL_RED;
		case ETextureFormat::R32_UINT:				return GL_RED_INTEGER;
		case ETextureFormat::R32_SINT:				return GL_RED_INTEGER;

		case ETextureFormat::R8G8_UNORM:			return GL_RG;
		case ETextureFormat::R8G8_UINT:				return GL_RG_INTEGER;
		case ETextureFormat::R8G8_SNORM:			return GL_RG;
		case ETextureFormat::R8G8_SINT:				return GL_RG_INTEGER;

		case ETextureFormat::R16G16_FLOAT:			return GL_RG;
		case ETextureFormat::R16G16_UINT:			return GL_RG_INTEGER;
		case ETextureFormat::R16G16_SINT:			return GL_RG_INTEGER;

		case ETextureFormat::R32G32_FLOAT:			return GL_RG;
		case ETextureFormat::R32G32_UINT:			return GL_RG_INTEGER;
		case ETextureFormat::R32G32_SINT:			return GL_RG_INTEGER;

		case ETextureFormat::R11G11B10_FLOAT:		return GL_RGB;

		case ETextureFormat::R32G32B32_FLOAT:		return GL_RGB;
		case ETextureFormat::R32G32B32_UINT:		return GL_RGB_INTEGER;
		case ETextureFormat::R32G32B32_SINT:		return GL_RGB_INTEGER;

		case ETextureFormat::R8G8B8A8_UNORM:		return GL_RGBA;
		case ETextureFormat::R8G8B8A8_UINT:			return GL_RGBA_INTEGER;
		case ETextureFormat::R8G8B8A8_SNORM:		return GL_RGBA;
		case ETextureFormat::R8G8B8A8_SINT:			return GL_RGBA_INTEGER;

		case ETextureFormat::R10G10B10A2_UNORM:		return GL_RGBA;
		case ETextureFormat::R10G10B10A2_UINT:		return GL_RGBA_INTEGER;

		case ETextureFormat::R16G16B16A16_FLOAT:	return GL_RGBA;
		case ETextureFormat::R16G16B16A16_UINT:		return GL_RGBA_INTEGER;
		case ETextureFormat::R16G16B16A16_SINT:		return GL_RGBA_INTEGER;

		case ETextureFormat::R32G32B32A32_FLOAT:	return GL_RGBA;
		case ETextureFormat::R32G32B32A32_UINT:		return GL_RGBA_INTEGER;
		case ETextureFormat::R32G32B32A32_SINT:		return GL_RGBA_INTEGER;

		case ETextureFormat::B5G6R5_UNORM:			return GL_RGB; // return GL_BGR;
		case ETextureFormat::B5G5R5A1_UNORM:		return GL_RGBA; // return GL_BGRA;
		case ETextureFormat::B8G8R8A8_UNORM:		return GL_RGBA; // return GL_BGRA;
		case ETextureFormat::B4G4R4A4_UNORM:		return GL_RGBA; // return GL_BGRA;

		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the OpenGL texture internal format for the given Umbral texture format.
	 *
	 * @param textureFormat The Umbral texture format.
	 * @return The OpenGL texture internal format.
	 */
	[[nodiscard]] constexpr GLenum GetTextureInternalFormat(const ETextureFormat textureFormat)
	{
		switch (textureFormat)
		{
		case ETextureFormat::R8_UNORM:				return GL_R8;
		case ETextureFormat::R8_UINT:				return GL_R8UI;
		case ETextureFormat::R8_SNORM:				return GL_R8_SNORM;
		case ETextureFormat::R8_SINT:				return GL_R8I;

		case ETextureFormat::R16_FLOAT:				return GL_R16F;
		case ETextureFormat::R16_UINT:				return GL_R16UI;
		case ETextureFormat::R16_SINT:				return GL_R16I;

		case ETextureFormat::R32_FLOAT:				return GL_R32F;
		case ETextureFormat::R32_UINT:				return GL_R32UI;
		case ETextureFormat::R32_SINT:				return GL_R32I;

		case ETextureFormat::R8G8_UNORM:			return GL_RG8;
		case ETextureFormat::R8G8_UINT:				return GL_RG8UI;
		case ETextureFormat::R8G8_SNORM:			return GL_RG8_SNORM;
		case ETextureFormat::R8G8_SINT:				return GL_RG8I;

		case ETextureFormat::R16G16_FLOAT:			return GL_RG16F;
		case ETextureFormat::R16G16_UINT:			return GL_RG16UI;
		case ETextureFormat::R16G16_SINT:			return GL_RG16I;

		case ETextureFormat::R32G32_FLOAT:			return GL_RG32F;
		case ETextureFormat::R32G32_UINT:			return GL_RG32UI;
		case ETextureFormat::R32G32_SINT:			return GL_RG32I;

		case ETextureFormat::R11G11B10_FLOAT:		return GL_R11F_G11F_B10F;

		case ETextureFormat::R32G32B32_FLOAT:		return GL_RGB32F;
		case ETextureFormat::R32G32B32_UINT:		return GL_RGB32UI;
		case ETextureFormat::R32G32B32_SINT:		return GL_RGB32I;

		case ETextureFormat::R8G8B8A8_UNORM:		return GL_RGBA8;
		case ETextureFormat::R8G8B8A8_UINT:			return GL_RGBA8UI;
		case ETextureFormat::R8G8B8A8_SNORM:		return GL_RGBA8_SNORM;
		case ETextureFormat::R8G8B8A8_SINT:			return GL_RGBA8I;

		case ETextureFormat::R10G10B10A2_UNORM:		return GL_RGB10_A2;
		case ETextureFormat::R10G10B10A2_UINT:		return GL_RGB10_A2UI;

		case ETextureFormat::R16G16B16A16_FLOAT:	return GL_RGBA16F;
		case ETextureFormat::R16G16B16A16_UINT:		return GL_RGBA16UI;
		case ETextureFormat::R16G16B16A16_SINT:		return GL_RGBA16I;

		case ETextureFormat::R32G32B32A32_FLOAT:	return GL_RGBA32F;
		case ETextureFormat::R32G32B32A32_UINT:		return GL_RGBA32UI;
		case ETextureFormat::R32G32B32A32_SINT:		return GL_RGBA32I;

		case ETextureFormat::B5G6R5_UNORM:			return GL_RGB8;
		case ETextureFormat::B5G5R5A1_UNORM:		return GL_RGB5_A1;
		case ETextureFormat::B8G8R8A8_UNORM:		return GL_RGBA8;
		case ETextureFormat::B4G4R4A4_UNORM:		return GL_RGBA4;

		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the OpenGL texture data type for the given Umbral texture format.
	 *
	 * @param textureFormat The Umbral texture format.
	 * @return The OpenGL texture data type.
	 */
	[[nodiscard]] constexpr GLenum GetTextureDataType(const ETextureFormat textureFormat)
	{
		switch (textureFormat)
		{
		case ETextureFormat::R8_UNORM:				return GL_UNSIGNED_BYTE;
		case ETextureFormat::R8_UINT:				return GL_UNSIGNED_BYTE;
		case ETextureFormat::R8_SNORM:				return GL_BYTE;
		case ETextureFormat::R8_SINT:				return GL_BYTE;

		// TODO OpenGL spec says that 16-bit _FLOAT types can be GL_HALF_FLOAT *or* GL_FLOAT [^1]
		// https://docs.gl/es3/glTexImage2D
		case ETextureFormat::R16_FLOAT:				return GL_FLOAT;
		case ETextureFormat::R16_UINT:				return GL_UNSIGNED_SHORT;
		case ETextureFormat::R16_SINT:				return GL_SHORT;

		case ETextureFormat::R32_FLOAT:				return GL_FLOAT;
		case ETextureFormat::R32_UINT:				return GL_UNSIGNED_INT;
		case ETextureFormat::R32_SINT:				return GL_INT;

		case ETextureFormat::R8G8_UNORM:			return GL_UNSIGNED_BYTE;
		case ETextureFormat::R8G8_UINT:				return GL_UNSIGNED_BYTE;
		case ETextureFormat::R8G8_SNORM:			return GL_BYTE;
		case ETextureFormat::R8G8_SINT:				return GL_BYTE;

		case ETextureFormat::R16G16_FLOAT:			return GL_FLOAT; // [^1]
		case ETextureFormat::R16G16_UINT:			return GL_UNSIGNED_SHORT;
		case ETextureFormat::R16G16_SINT:			return GL_SHORT;

		case ETextureFormat::R32G32_FLOAT:			return GL_FLOAT;
		case ETextureFormat::R32G32_UINT:			return GL_UNSIGNED_INT;
		case ETextureFormat::R32G32_SINT:			return GL_INT;

		// TODO Could be GL_UNSIGNED_INT_10F_11F_11F_REV, GL_HALF_FLOAT, *or* GL_R11F_G11F_B10F
		case ETextureFormat::R11G11B10_FLOAT:		return GL_FLOAT;

		case ETextureFormat::R32G32B32_FLOAT:		return GL_FLOAT;
		case ETextureFormat::R32G32B32_UINT:		return GL_UNSIGNED_INT;
		case ETextureFormat::R32G32B32_SINT:		return GL_INT;

		case ETextureFormat::R8G8B8A8_UNORM:		return GL_UNSIGNED_BYTE;
		case ETextureFormat::R8G8B8A8_UINT:			return GL_UNSIGNED_BYTE;
		case ETextureFormat::R8G8B8A8_SNORM:		return GL_BYTE;
		case ETextureFormat::R8G8B8A8_SINT:			return GL_BYTE;

		case ETextureFormat::R10G10B10A2_UNORM:		return GL_UNSIGNED_INT_2_10_10_10_REV;
		case ETextureFormat::R10G10B10A2_UINT:		return GL_UNSIGNED_INT_2_10_10_10_REV;

		case ETextureFormat::R16G16B16A16_FLOAT:	return GL_FLOAT; // [^1]
		case ETextureFormat::R16G16B16A16_UINT:		return GL_UNSIGNED_SHORT;
		case ETextureFormat::R16G16B16A16_SINT:		return GL_SHORT;

		case ETextureFormat::R32G32B32A32_FLOAT:	return GL_FLOAT;
		case ETextureFormat::R32G32B32A32_UINT:		return GL_UNSIGNED_INT;
		case ETextureFormat::R32G32B32A32_SINT:		return GL_INT;

		case ETextureFormat::B5G6R5_UNORM:			return GL_UNSIGNED_BYTE; // *or* GL_UNSIGNED_SHORT_5_6_5
		case ETextureFormat::B5G5R5A1_UNORM:		return GL_UNSIGNED_SHORT_5_5_5_1; // *or* GL_UNSIGNED_BYTE
		case ETextureFormat::B8G8R8A8_UNORM:		return GL_UNSIGNED_BYTE;
		case ETextureFormat::B4G4R4A4_UNORM:		return GL_UNSIGNED_SHORT_4_4_4_4; // *or* GL_UNSIGNED_BYTE

		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the magnification filter for a texture filter.
	 *
	 * @param textureFilter The texture filter.
	 * @return THe magnification filter.
	 */
	[[nodiscard]] constexpr GLenum GetTextureMagFilter(const ETextureFilter textureFilter)
	{
		switch (textureFilter)
		{
		case ETextureFilter::Linear:                        return GL_LINEAR;
		case ETextureFilter::Point:                         return GL_NEAREST;
		case ETextureFilter::Anisotropic:                   return GL_LINEAR;
		case ETextureFilter::LinearMipPoint:                return GL_LINEAR;
		case ETextureFilter::PointMipLinear:                return GL_NEAREST;
		case ETextureFilter::MinLinearMagPointMipLinear:    return GL_NEAREST;
		case ETextureFilter::MinLinearMagPointMipPoint:     return GL_NEAREST;
		case ETextureFilter::MinPointMagLinearMipLinear:    return GL_LINEAR;
		case ETextureFilter::MinPointMagLinearMipPoint:     return GL_LINEAR;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the minification mip filter for a texture filter.
	 *
	 * @param textureFilter The texture filter.
	 * @return THe minification mip filter.
	 */
	[[nodiscard]] constexpr GLenum GetTextureMinMipFilter(const ETextureFilter textureFilter)
	{
		switch (textureFilter)
		{
		case ETextureFilter::Linear:                        return GL_LINEAR_MIPMAP_LINEAR;
		case ETextureFilter::Point:                         return GL_NEAREST_MIPMAP_NEAREST;
		case ETextureFilter::Anisotropic:                   return GL_LINEAR_MIPMAP_LINEAR;
		case ETextureFilter::LinearMipPoint:                return GL_LINEAR_MIPMAP_NEAREST;
		case ETextureFilter::PointMipLinear:                return GL_NEAREST_MIPMAP_LINEAR;
		case ETextureFilter::MinLinearMagPointMipLinear:    return GL_LINEAR_MIPMAP_LINEAR;
		case ETextureFilter::MinLinearMagPointMipPoint:     return GL_LINEAR_MIPMAP_NEAREST;
		case ETextureFilter::MinPointMagLinearMipLinear:    return GL_NEAREST_MIPMAP_LINEAR;
		case ETextureFilter::MinPointMagLinearMipPoint:     return GL_NEAREST_MIPMAP_NEAREST;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the minification filter for a texture filter.
	 *
	 * @param textureFilter The texture filter.
	 * @return THe minification filter.
	 */
	[[nodiscard]] constexpr GLenum GetTextureMinFilter(const ETextureFilter textureFilter)
	{
		switch (textureFilter)
		{
		case ETextureFilter::Linear:                        return GL_LINEAR;
		case ETextureFilter::Point:                         return GL_NEAREST;
		case ETextureFilter::Anisotropic:                   return GL_LINEAR;
		case ETextureFilter::LinearMipPoint:                return GL_LINEAR;
		case ETextureFilter::PointMipLinear:                return GL_NEAREST;
		case ETextureFilter::MinLinearMagPointMipLinear:    return GL_LINEAR;
		case ETextureFilter::MinLinearMagPointMipPoint:     return GL_LINEAR;
		case ETextureFilter::MinPointMagLinearMipLinear:    return GL_NEAREST;
		case ETextureFilter::MinPointMagLinearMipPoint:     return GL_NEAREST;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the OpenGL texture wrap mode for the given Umbral texture address mode.
	 *
	 * @param textureFormat The Umbral texture wrap mode.
	 * @return The OpenGL texture wrap mode.
	 */
	[[nodiscard]] constexpr GLenum GetTextureWrapMode(const ETextureAddressMode addressMode)
	{
		switch (addressMode)
		{
		case ETextureAddressMode::Wrap:     return GL_REPEAT;
		case ETextureAddressMode::Clamp:    return GL_CLAMP_TO_EDGE;
		case ETextureAddressMode::Mirror:   return GL_MIRRORED_REPEAT;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the data type of each attribute in a vertex element.
	 *
	 * @param elementFormat The vertex element format.
	 * @return The data type of each attribute in a vertex element.
	 */
	[[nodiscard]] constexpr GLenum GetVertexAttributeDataType(const EVertexElementFormat elementFormat)
	{
		switch (elementFormat)
		{
		case EVertexElementFormat::Single:              return GL_FLOAT;
		case EVertexElementFormat::Vector2:             return GL_FLOAT;
		case EVertexElementFormat::Vector3:             return GL_FLOAT;
		case EVertexElementFormat::Vector4:             return GL_FLOAT;
		case EVertexElementFormat::Color:               return GL_UNSIGNED_BYTE;
		case EVertexElementFormat::Byte4:               return GL_UNSIGNED_BYTE;
		case EVertexElementFormat::Short2:              return GL_SHORT;
		case EVertexElementFormat::Short4:              return GL_SHORT;
		case EVertexElementFormat::NormalizedShort2:    return GL_SHORT;
		case EVertexElementFormat::NormalizedShort4:    return GL_SHORT;
		case EVertexElementFormat::HalfVector2:         return GL_HALF_FLOAT;
		case EVertexElementFormat::HalfVector4:         return GL_HALF_FLOAT;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the number of attributes in a vertex element.
	 *
	 * @param elementFormat The vertex element's format.
	 * @return The number of attributes in a vertex element.
	 */
	[[nodiscard]] constexpr int32 GetVertexAttributeElementCount(const EVertexElementFormat elementFormat)
	{
		switch (elementFormat)
		{
		case EVertexElementFormat::Single:              return 1;
		case EVertexElementFormat::Vector2:             return 2;
		case EVertexElementFormat::Vector3:             return 3;
		case EVertexElementFormat::Vector4:             return 4;
		case EVertexElementFormat::Color:               return 4;
		case EVertexElementFormat::Byte4:               return 4;
		case EVertexElementFormat::Short2:              return 2;
		case EVertexElementFormat::Short4:              return 4;
		case EVertexElementFormat::NormalizedShort2:    return 2;
		case EVertexElementFormat::NormalizedShort4:    return 4;
		case EVertexElementFormat::HalfVector2:         return 2;
		case EVertexElementFormat::HalfVector4:         return 4;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Gets the equivalent OpenGL vertex buffer usage for the given Umbral vertex buffer usage.
	 *
	 * @param shaderType The Umbral vertex buffer usage.
	 * @return The OpenGL vertex buffer usage.
	 */
	[[nodiscard]] constexpr GLenum GetVertexBufferUsage(const EVertexBufferUsage bufferUsage)
	{
		switch (bufferUsage)
		{
		case EVertexBufferUsage::Static:    return GL_STATIC_DRAW;
		case EVertexBufferUsage::Dynamic:	return GL_STREAM_DRAW;
		default: UM_ASSERT_NOT_REACHED();
		}
	}

	/**
	 * @brief Checks to see if the given vertex element format is normalized.
	 *
	 * @param element The vertex element.
	 * @return True if the vertex element if normalized, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsVertexElementNormalized(const FVertexElement& element)
	{
		return element.ElementUsage == EVertexElementUsage::Color ||
		       element.ElementFormat == EVertexElementFormat::NormalizedShort2 ||
		       element.ElementFormat == EVertexElementFormat::NormalizedShort4;
	}
}