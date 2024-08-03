#pragma once

#include "MetaTool/AttributeInfo.h"
#include "MetaTool/EnumUtilities.h"
#include "MetaTool/FunctionInfo.h"
#include "MetaTool/PropertyInfo.h"

enum : uint32 { INVALID_LINE = static_cast<uint32>(-1) };

/**
 * @brief An enumeration of base type inheritance types.
 */
enum class ETypeInheritanceKind
{
	Invalid,
	Public,
	Protected,
	Private,
	Virtual
};

namespace Private
{
	template<>
	struct TEnumToString<ETypeInheritanceKind>
	{
		static constexpr FStringView Get(const ETypeInheritanceKind value)
		{
			switch (value)
			{
			case ETypeInheritanceKind::Public: return "public"_sv;
			case ETypeInheritanceKind::Protected: return "protected"_sv;
			case ETypeInheritanceKind::Private: return "private"_sv;
			case ETypeInheritanceKind::Virtual: return "virtual"_sv;
			case ETypeInheritanceKind::Invalid:
			default: return "<invalid>"_sv;
			}
		}
	};

	template<>
	struct TStringToEnum<ETypeInheritanceKind>
	{
		using EnumToStringType = TEnumToString<ETypeInheritanceKind>;

		static constexpr ETypeInheritanceKind Get(const FStringView text)
		{
#define RETURN_ENUM_TO_STRING_CASE(EnumName) \
				if (text == EnumToStringType::Get(ETypeInheritanceKind:: EnumName)) \
				{ \
					return ETypeInheritanceKind:: EnumName; \
				}

			RETURN_ENUM_TO_STRING_CASE(Public)
			RETURN_ENUM_TO_STRING_CASE(Protected)
			RETURN_ENUM_TO_STRING_CASE(Private)
			RETURN_ENUM_TO_STRING_CASE(Virtual)

			return ETypeInheritanceKind::Invalid;

#undef RETURN_ENUM_TO_STRING_CASE
		}
	};
}

/**
 * @brief Defines information about a struct.
 */
class FParsedStructInfo
{
public:

	/** @brief The struct's name. */
	FStringView TypeName;

	/** @brief The struct's base type name. */
	FStringView BaseTypeName;

	/** @brief The class's base class inheritance type. */
	ETypeInheritanceKind BaseTypeInheritanceKind = ETypeInheritanceKind::Invalid;

	/** @brief The struct's attributes. */
	TArray<FParsedAttributeInfo> Attributes;

	/** @brief The struct's properties. */
	TArray<FParsedPropertyInfo> Properties;

	/** @brief The line where the UM_GENERATED_BODY() macro can be found. */
	uint32 GeneratedBodyMacroLine = INVALID_LINE;

	/**
	 * @brief Destroys this parsed struct info.
	 */
	virtual ~FParsedStructInfo() = default;

	/**
	 * @brief Attempts to find an attribute with the given name.
	 *
	 * @param name The attribute name.
	 * @return The attribute if it was found, otherwise nullptr.
	 */
	inline const FParsedAttributeInfo* FindAttributeByName(const FStringView name) const
	{
		return Attributes.FindByPredicate([name](const FParsedAttributeInfo& attribute)
		{
			return attribute.Name == name;
		});
	}

	/**
	 * @brief Checks to see if this class has any object properties.
	 *
	 * @return True if this class has any object properties, otherwise false.
	 */
	[[nodiscard]] bool HasObjectProperties() const;

	/**
	 * @brief Checks to see if the given type name is object-based.
	 *
	 * @param typeName The type name.
	 * @return True if the given type name is object-based, otherwise false.
	 */
	[[nodiscard]] static bool IsObjectBasedName(FStringView typeName);

	/**
	 * @brief Checks to see if this class info is an object-based class.
	 *
	 * @return True if this class info is an object-based class.
	 */
	[[nodiscard]] bool IsObjectClass() const;
};