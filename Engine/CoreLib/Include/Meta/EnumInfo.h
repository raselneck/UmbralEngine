#pragma once

#include "Containers/Array.h"
#include "Containers/StringView.h"
#include "Meta/TypeInfo.h"
#include "Templates/IsEnum.h"

/**
 * @brief Defines meta information about an enum entry.
 */
class FEnumEntryInfo : public FAttributeCollectionInfo
{
public:

	/**
	 * @brief Sets default values for this enum entry's properties.
	 *
	 * @param name The entry's name.
	 * @param value The entry's value.
	 */
	FEnumEntryInfo(FStringView name, int64 value);

	/**
	 * @brief Destroys this enum entry info.
	 */
	virtual ~FEnumEntryInfo() override = default;

	/**
	 * @brief Gets the entry's name.
	 *
	 * @return The entry's name.
	 */
	[[nodiscard]] FStringView GetName() const;

	/**
	 * @brief Gets the entry's value.
	 *
	 * @return The entry's value.
	 */
	[[nodiscard]] int64 GetValue() const;

private:

	FStringView m_Name;
	int64 m_Value = 0;
};

/**
 * @brief Defines meta information about an enum.
 */
class FEnumInfo : public FTypeInfo
{
public:

	/**
	 * @brief Sets default values for this enum's properties.
	 *
	 * @param name The enum's name.
	 * @param underlyingType The enum's underlying type.
	 */
	FEnumInfo(FStringView name, const FTypeInfo* underlyingType);

	/**
	 * @brief Destroys this enum info.
	 */
	virtual ~FEnumInfo() override = default;

	/**
	 * @brief Registers an entry with this enum.
	 *
	 * @param name The entry's name.
	 * @param value The entry's value.
	 */
	void AddEntry(FStringView name, int64 value);

	/**
	 * @brief Registers an entry with this enum.
	 *
	 * @tparam EnumType The type of the enum value.
	 * @param name The entry's name.
	 * @param value The entry's value.
	 */
	template<typename EnumType>
	void AddEntry(FStringView name, const EnumType value)
		requires IsEnum<EnumType>
	{
		// TODO Can't do this directly because it'd cause a recursive crash when building the enum type info
		//UM_ASSERT(::GetType<EnumType>() == this, "Attempting to register unrelated enum value type");

		AddEntry(name, static_cast<int64>(value));
	}

	/**
	 * @brief Gets the entries in this enum.
	 *
	 * @return The entries in this enum.
	 */
	[[nodiscard]] TSpan<const FEnumEntryInfo> GetEntries() const
	{
		return m_Entries.AsSpan();
	}

	/**
	 * @brief Gets an entry by its index.
	 *
	 * @param index The entry's index.
	 * @return The entry at \p index, otherwise null if one was not found.
	 */
	[[nodiscard]] const FEnumEntryInfo* GetEntryByIndex(int32 index) const;

	/**
	 * @brief Gets an entry by its name.
	 *
	 * @param name The entry's name.
	 * @return The entry with the given \p name, otherwise null if one was not found.
	 */
	[[nodiscard]] const FEnumEntryInfo* GetEntryByName(FStringView name) const;

	/**
	 * @brief Gets an entry by its value.
	 *
	 * @param value The entry's value.
	 * @return The entry with the given \p value, otherwise null if one was not found.
	 */
	[[nodiscard]] const FEnumEntryInfo* GetEntryByValue(int64 value) const;

	/**
	 * @brief Gets an entry by its value.
	 *
	 * @tparam EnumType The type of the enum value.
	 * @param value The entry's value.
	 * @return The entry with the given \p value, otherwise null if one was not found.
	 */
	template<typename EnumType>
	[[nodiscard]] const FEnumEntryInfo* GetEntryByValue(const EnumType value) const
		requires IsEnum<EnumType>
	{
		return GetEntryByValue(static_cast<int64>(value));
	}

	/**
	 * @brief Gets the number of entries in the enum.
	 *
	 * @return The number of entries in the enum.
	 */
	[[nodiscard]] int32 GetNumEntries() const;

	/**
	 * @brief Gets the enum's underlying type.
	 *
	 * @return The enum's underlying type.
	 */
	[[nodiscard]] const FTypeInfo* GetUnderlyingType() const;

private:

	TArray<FEnumEntryInfo> m_Entries;
	const FTypeInfo* m_UnderlyingType = nullptr;
};