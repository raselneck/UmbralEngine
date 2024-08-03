#pragma once

#include "Containers/Array.h"
#include "Containers/StringView.h"
#include "Meta/PropertyInfo.h"
#include "Meta/TypeInfo.h"

/**
 * @brief Defines meta information about a struct.
 */
class FStructInfo : public FTypeInfo
{
public:

	/**
	 * @brief Destroys this struct info.
	 */
	virtual ~FStructInfo() override = default;

	/**
	 * @brief Sets default values for this struct info's properties.
	 *
	 * @param name The type's name.
	 * @param size The type's size.
	 * @param alignment The type's alignment.
	 * @param baseType The type's base type.
	 */
	FStructInfo(FStringView name, int32 size, int32 alignment, const FStructInfo* baseType);

	/**
	 * @brief Adds a property to this struct info.
	 *
	 * @param name The property's name.
	 * @param valueType The property's value type.
	 * @param offset The offset to the property from an instance of this struct.
	 * @return The added property.
	 */
	[[nodiscard]] FPropertyInfo& AddProperty(FStringView name, const FTypeInfo* valueType, int32 offset);

	/**
	 * @brief Gets the struct's base type (if one exists).
	 *
	 * @return The struct's base type.
	 */
	[[nodiscard]] const FStructInfo* GetBaseType() const;

	/**
	 * @brief Gets the number of properties in the struct.
	 *
	 * @return The number of properties in the struct.
	 */
	[[nodiscard]] int32 GetNumProperties() const;

	/**
	 * @brief Gets this struct's properties.
	 *
	 * @return This struct's properties.
	 */
	[[nodiscard]] TSpan<const FPropertyInfo> GetProperties() const
	{
		return m_Properties.AsSpan();
	}

	/**
	 * @brief Gets a property of the struct at the given index.
	 *
	 * @param index The index of the property.
	 * @return The property at \p index, or null if one was not found.
	 */
	[[nodiscard]] const FPropertyInfo* GetProperty(int32 index) const;

	/**
	 * @brief Attempts to get a property by its name.
	 *
	 * @param name The name of the property.
	 * @return The property, or null if it was not found.
	 */
	[[nodiscard]] const FPropertyInfo* GetPropertyByName(FStringView name) const;

	/**
	 * @brief Checks to see if the struct inherits from the given type.
	 *
	 * @param type The type to check.
	 * @return True if the struct inherits from the type defined by \p type, otherwise false.
	 */
	[[nodiscard]] bool IsA(const FStructInfo* type) const;

private:

	TArray<FPropertyInfo> m_Properties;
	const FStructInfo* m_BaseType = nullptr;
};