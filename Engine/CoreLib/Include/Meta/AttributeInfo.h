#pragma once

#include "Containers/Array.h"
#include "Containers/StringView.h"

/**
 * @brief Defines information about an attribute.
 */
class FAttributeInfo
{
public:

	/**
	 * @brief Sets default values for this attribute's properties.
	 *
	 * @param name The attribute's name.
	 */
	explicit FAttributeInfo(FStringView name);

	/**
	 * @brief Sets default values for this attribute's properties.
	 *
	 * @param name The attribute's name.
	 * @param value The attribute's value.
	 */
	FAttributeInfo(FStringView name, FStringView value);

	/**
	 * @brief Gets the attribute's name.
	 *
	 * @return The attribute's name.
	 */
	[[nodiscard]] FStringView GetName() const;

	/**
	 * @brief Gets the attribute's value.
	 *
	 * @return The attribute's value.
	 */
	[[nodiscard]] FStringView GetValue() const;

	/**
	 * @brief Checks to see if the attribute has a value.
	 *
	 * @return True if the attribute has a value, otherwise false.
	 */
	[[nodiscard]] bool HasValue() const;

private:

	FStringView m_Name;
	FStringView m_Value;
};

/**
 * @brief Defines information about an attribute collection.
 */
class FAttributeCollectionInfo
{
public:

	/**
	 * @brief Destroys this attribute collection info.
	 */
	virtual ~FAttributeCollectionInfo() = default;

	/**
	 * @brief Registers a name-only attribute.
	 *
	 * @param name The attribute name.
	 * @return The attribute.
	 */
	[[nodiscard]] FAttributeInfo& AddAttribute(FStringView name);

	/**
	 * @brief Registers an attribute with a value.
	 *
	 * @param name The attribute's name.
	 * @param name The attribute's value.
	 * @return The attribute.
	 */
	[[nodiscard]] FAttributeInfo& AddAttribute(FStringView name, FStringView value);

	/**
	 * @brief Gets the attribute at the given index.
	 *
	 * @param index The index.
	 * @return The attribute at \p index, or nullptr if one was not found.
	 */
	[[nodiscard]] const FAttributeInfo* GetAttribute(int32 index) const;

	/**
	 * @brief Gets the attribute with the given name.
	 *
	 * @param name The name of the attribute.
	 * @return The attribute, or nullptr if one was not found.
	 */
	[[nodiscard]] const FAttributeInfo* GetAttributeByName(FStringView name) const;

	/**
	 * @brief Gets the number of attributes.
	 *
	 * @return The number of attributes.
	 */
	[[nodiscard]] int32 GetNumAttributes() const;

	/**
	 * @brief Checks to see if there is an attribute with the given name.
	 *
	 * @param name The attribute name.
	 * @return True if there is an attribute with the given name, otherwise false.
	 */
	[[nodiscard]] bool HasAttribute(FStringView name) const;

private:

	TArray<FAttributeInfo> m_Attributes;
};