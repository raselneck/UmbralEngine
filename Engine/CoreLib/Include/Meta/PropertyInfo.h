#pragma once

#include "Engine/Assert.h"
#include "Meta/AttributeInfo.h"
#include "Meta/TypeInfo.h"

/**
 * @brief Defines meta information about a property.
 */
class FPropertyInfo : public FAttributeCollectionInfo
{
public:

	/**
	 * @brief Destroys this property info.
	 */
	virtual ~FPropertyInfo() override = default;

	/**
	 * @brief Sets default values for this property info's properties.
	 *
	 * @param name The property name.
	 * @param type The property's value type.
	 * @param offset The offset to the property from its containing type.
	 */
	FPropertyInfo(FStringView name, const FTypeInfo* valueType, int32 offset);

	/**
	 * @brief Gets the property's name.
	 *
	 * @return The property's name.
	 */
	[[nodiscard]] FStringView GetName() const;

	/**
	 * @brief Gets the offset to the property from its containing type.
	 *
	 * @return The offset to the property from its containing type.
	 */
	[[nodiscard]] int32 GetOffset() const;

	/**
	 * @brief Gets the property's value type.
	 *
	 * @return The property's value type.
	 */
	[[nodiscard]] const FTypeInfo* GetValueType() const;

	/**
	 * @brief Gets an instance value of this property from the given container.
	 *
	 * @tparam ValueType The property value type.
	 * @tparam ContainerType The container type.
	 * @param container The property container.
	 * @return The property value.
	 */
	template<typename ValueType, typename ContainerType>
	[[nodiscard]] ValueType* GetValue(ContainerType* container) const
	{
		if (UNLIKELY(container == nullptr))
		{
			return nullptr;
		}

		UM_ASSERT(DoesTypeHaveThisProperty(::GetType<ContainerType>()), "This property does not belong to the given container type");

		void* valuePointer = GetValuePointer(container);
		UM_ASSERT(valuePointer != nullptr, "Failed to get value pointer from container");

		return reinterpret_cast<ValueType*>(valuePointer);
	}

	/**
	 * @brief Gets an instance value of this property from the given container.
	 *
	 * @tparam ValueType The property value type.
	 * @tparam ContainerType The container type.
	 * @param container The property container.
	 * @return The property value.
	 */
	template<typename ValueType, typename ContainerType>
	[[nodiscard]] const ValueType* GetValue(const ContainerType* container) const
	{
		return GetValue(const_cast<ContainerType*>(container));
	}

private:

	/**
	 * @brief Checks to see if the given type has this property.
	 *
	 * @param typeInfo The type to check.
	 * @return True if the given type has this property, otherwise false.
	 */
	[[nodiscard]] bool DoesTypeHaveThisProperty(const FTypeInfo* typeInfo) const;

	/**
	 * @brief Gets a pointer to the value defined by this property.
	 *
	 * @param container The container type.
	 * @return A pointer to the value defined by this property.
	 */
	[[nodiscard]] void* GetValuePointer(void* container) const;

	FStringView m_Name;
	const FTypeInfo* m_ValueType;
	int32 m_Offset = 0;
};

// TODO Specialized property info for arrays, hash tables, etc