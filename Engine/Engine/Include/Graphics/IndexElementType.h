#pragma once

#include "Engine/IntTypes.h"
#include "Meta/MetaMacros.h"
#include "Templates/IntegralConstant.h"
#include "IndexElementType.Generated.h"

/**
 * @brief An enumeration of possible index buffer element sizes.
 */
UM_ENUM()
enum class EIndexElementType
{
	/** @brief Denotes that the index buffer is invalid. */
	None,

	/** @brief Each element in the index buffer is an unsigned 8-bit integer.. */
	Byte,

	/** @brief Each element in the index buffer is an unsigned 16-bit integer.. */
	Short,

	/** @brief Each element in the index buffer is an unsigned 32-bit integer.. */
	Int
};

namespace Private
{
	template<typename T> struct TIsValidIndexType : FFalseType { };
	template<> struct TIsValidIndexType<int8>     : FTrueType { };
	template<> struct TIsValidIndexType<uint8>    : FTrueType { };
	template<> struct TIsValidIndexType<int16>    : FTrueType { };
	template<> struct TIsValidIndexType<uint16>   : FTrueType { };
	template<> struct TIsValidIndexType<int32>    : FTrueType { };
	template<> struct TIsValidIndexType<uint32>   : FTrueType { };

	template<typename T>
	inline constexpr bool IsValidIndexType = TIsValidIndexType<T>::Value;

	template<typename T> struct TFindIndexType;
	template<> struct TFindIndexType<int8>   { static constexpr EIndexElementType Value = EIndexElementType::Byte; };
	template<> struct TFindIndexType<uint8>  { static constexpr EIndexElementType Value = EIndexElementType::Byte; };
	template<> struct TFindIndexType<int16>  { static constexpr EIndexElementType Value = EIndexElementType::Short; };
	template<> struct TFindIndexType<uint16> { static constexpr EIndexElementType Value = EIndexElementType::Short; };
	template<> struct TFindIndexType<int32>  { static constexpr EIndexElementType Value = EIndexElementType::Int; };
	template<> struct TFindIndexType<uint32> { static constexpr EIndexElementType Value = EIndexElementType::Int; };
}