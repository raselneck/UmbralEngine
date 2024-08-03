#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"
#include "MetaTool/AttributeInfo.h"

/**
 * @brief Defines meta information about a parsed property.
 */
class FParsedPropertyInfo
{
public:

	/** @brief The property's name. */
	FStringView PropertyName;

	/** @brief The property's type. */
	FStringView PropertyType;

	/** @brief The property's attributes. */
	TArray<FParsedAttributeInfo> Attributes;

	/** @brief The property's #define constraints. */
	TArray<FString> DefineConstraints;

	/**
	 * @brief Checks to see if this property is object-based.
	 *
	 * @return True if this property is object-based, otherwise false.
	 */
	[[nodiscard]] bool IsObjectProperty() const;
};