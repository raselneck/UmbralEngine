#pragma once

#include "Containers/Array.h"
#include "Containers/StringView.h"
#include "Meta/AttributeInfo.h"
#include "Templates/Decay.h"
#include "Templates/IsBaseOf.h"
#include "Templates/IsConstVolatile.h"
#include "Templates/IsPointer.h"
#include "Templates/IsReference.h"

/**
 * @brief Defines meta information about a type.
 */
class FTypeInfo : public FAttributeCollectionInfo
{
public:

	/**
	 * @brief Destroys this type info.
	 */
	virtual ~FTypeInfo() override = default;

	/**
	 * @brief Sets default values for this type info's properties.
	 *
	 * @param name The type's name.
	 * @param size The type's size.
	 * @param alignment The type's alignment.
	 */
	FTypeInfo(FStringView name, int32 size, int32 alignment);

	/**
	 * @brief Gets the type's alignment.
	 *
	 * @return The type's alignment.
	 */
	[[nodiscard]] int32 GetAlignment() const;

	/**
	 * @brief Gets this type's name.
	 *
	 * @return This type's name.
	 */
	[[nodiscard]] FStringView GetName() const;

	/**
	 * @brief Gets the type's size.
	 *
	 * @return The type's size.
	 */
	[[nodiscard]] int32 GetSize() const;

private:

	FStringView m_Name;
	int32 m_Size;
	int32 m_Alignment;
};

namespace Private
{
	template<typename T>
	struct TTypeDefinition;
}

#define DECLARE_PRIMITIVE_TYPE_DEFINITION(Type)                 \
	template<> struct Private::TTypeDefinition<Type>            \
	{                                                           \
		static const FTypeInfo* Get();                          \
	}

#define DEFINE_PRIMITIVE_TYPE_DEFINITION(Type)                  \
	static const FTypeInfo UM_JOIN(GTypeInfo_, Type)            \
	{                                                           \
		UM_STRINGIFY_AS_VIEW(Type),                             \
		sizeof(Type),                                           \
		alignof(Type)                                           \
	};                                                          \
	const FTypeInfo* ::Private::TTypeDefinition<Type>::Get()    \
	{                                                           \
		return &UM_JOIN(GTypeInfo_, Type);                      \
	}

DECLARE_PRIMITIVE_TYPE_DEFINITION(void);
DECLARE_PRIMITIVE_TYPE_DEFINITION(bool);
DECLARE_PRIMITIVE_TYPE_DEFINITION(int8);
DECLARE_PRIMITIVE_TYPE_DEFINITION(int16);
DECLARE_PRIMITIVE_TYPE_DEFINITION(int32);
DECLARE_PRIMITIVE_TYPE_DEFINITION(int64);
DECLARE_PRIMITIVE_TYPE_DEFINITION(uint8);
DECLARE_PRIMITIVE_TYPE_DEFINITION(uint16);
DECLARE_PRIMITIVE_TYPE_DEFINITION(uint32);
DECLARE_PRIMITIVE_TYPE_DEFINITION(uint64);
DECLARE_PRIMITIVE_TYPE_DEFINITION(float);
DECLARE_PRIMITIVE_TYPE_DEFINITION(double);
DECLARE_PRIMITIVE_TYPE_DEFINITION(char);
DECLARE_PRIMITIVE_TYPE_DEFINITION(char8_t);
DECLARE_PRIMITIVE_TYPE_DEFINITION(char16_t);
DECLARE_PRIMITIVE_TYPE_DEFINITION(char32_t);
DECLARE_PRIMITIVE_TYPE_DEFINITION(FStringView);

template<typename T>
inline decltype(auto) GetType()
{
	using DecayedType = typename TDecay<T>::Type;
	static_assert(Not<TIsConst<DecayedType>>);
	static_assert(Not<TIsVolatile<DecayedType>>);
	static_assert(Not<TIsReference<DecayedType>>);

	const auto* typeInfo = Private::TTypeDefinition<DecayedType>::Get();

	static_assert(TIsBaseOf<FTypeInfo, RemovePointer<RemoveCV<decltype(typeInfo)>>>::Value);

	return typeInfo;
}