#pragma once

#include "Containers/Array.h"
#include "Containers/String.h"
#include "JSON/JsonValueType.h"

class FJsonObject;
class FJsonValue;

/**
 * @brief Defines an array of JSON values.
 */
class FJsonArray final
{
public:

	using UnderlyingType = TArray<FJsonValue>;
	using SizeType = typename UnderlyingType::SizeType;
	using IteratorType = typename UnderlyingType::IteratorType;
	using ConstIteratorType = typename UnderlyingType::ConstIteratorType;

	/**
	 * @brief Sets default values for this JSON array's properties.
	 */
	FJsonArray() = default;

	/**
	 * @brief Initializes this JSON array with the given JSON value array.
	 *
	 * @param values The array of values to own.
	 */
	explicit FJsonArray(TArray<FJsonValue> values);

	/**
	 * @brief Initializes this JSON array with the given JSON value array.
	 *
	 * @param values The array of values to copy.
	 */
	explicit FJsonArray(const TArray<FJsonValue>& values);

	/**
	 * @brief Adds a JSON value with the given type.
	 *
	 * @param type The value type.
	 * @return The added JSON value.
	 */
	[[nodiscard]] FJsonValue& Add(EJsonValueType type);

	/**
	 * @brief Adds a copy of a JSON array to this array.
	 *
	 * @param array The array to copy.
	 * @return The added value.
	 */
	[[maybe_unused]] FJsonValue& Add(const FJsonArray& array);

	/**
	 * @brief Adds a JSON array to this array.
	 *
	 * @param array The array to add.
	 * @return The added value.
	 */
	[[maybe_unused]] FJsonValue& Add(FJsonArray&& array);

	/**
	 * @brief Adds a number to this array.
	 *
	 * @param number The number to add.
	 * @return The added value.
	 */
	[[maybe_unused]] FJsonValue& Add(double number);

	/**
	 * @brief Adds a copy of a JSON object to this array.
	 *
	 * @param object The object to copy.
	 * @return The added value.
	 */
	[[maybe_unused]] FJsonValue& Add(const FJsonObject& object);

	/**
	 * @brief Adds a JSON object to this array.
	 *
	 * @param object The object to add.
	 * @return The added value.
	 */
	[[maybe_unused]] FJsonValue& Add(FJsonObject&& object);

	/**
	 * @brief Adds a copy of a string to this array.
	 *
	 * @param string The string to copy.
	 * @return The added value.
	 */
	[[maybe_unused]] FJsonValue& Add(const FString& string);

	/**
	 * @brief Adds a string to this array.
	 *
	 * @param string The string to add.
	 * @return The added value.
	 */
	[[maybe_unused]] FJsonValue& Add(FString&& string);

	/**
	 * @brief Adds a string to this array.
	 *
	 * @param string The string to add.
	 * @return The added value.
	 */
	[[maybe_unused]] FJsonValue& Add(FStringView string);

	/**
	 * @brief Adds a copy of a JSON value to this array.
	 *
	 * @param value The value to copy.
	 * @return The index of the added value.
	 */
	[[maybe_unused]] SizeType Add(const FJsonValue& value);

	/**
	 * @brief Adds a JSON value to this array.
	 *
	 * @param value The value to add.
	 * @return The index of the added value.
	 */
	[[maybe_unused]] SizeType Add(FJsonValue&& value);

	/**
	 * @brief Gets the JSON value at the given index.
	 *
	 * @param index The index.
	 * @return The value at \p index if there is one, otherwise nullptr.
	 */
	[[nodiscard]] const FJsonValue& At(SizeType index) const;

	/**
	 * @brief Gets the JSON value at the given index.
	 *
	 * @param index The index.
	 * @return The value at \p index if there is one, otherwise nullptr.
	 */
	[[nodiscard]] FJsonValue& At(SizeType index);

	/**
	 * @brief Gets the number of values in this JSON array.
	 *
	 * @return The number of values in this JSON array.
	 */
	[[nodiscard]] SizeType Num() const
	{
		return m_Values.Num();
	}

	/**
	 * @brief Gets the JSON value at the given index.
	 *
	 * @param index The index.
	 * @return The value at \p index if there is one, otherwise nullptr.
	 */
	[[nodiscard]] const FJsonValue& operator[](const SizeType index) const
	{
		return At(index);
	}

	/**
	 * @brief Gets the JSON value at the given index.
	 *
	 * @param index The index.
	 * @return The value at \p index if there is one, otherwise nullptr.
	 */
	[[nodiscard]] FJsonValue& operator[](const SizeType index)
	{
		return At(index);
	}

	// STL compatibility BEGIN
	[[nodiscard]] IteratorType      begin();
	[[nodiscard]] ConstIteratorType begin() const;
	[[nodiscard]] ConstIteratorType cbegin() const;
	[[nodiscard]] IteratorType      end();
	[[nodiscard]] ConstIteratorType end() const;
	[[nodiscard]] ConstIteratorType cend() const;
	// STL compatibility END

private:

	TArray<FJsonValue> m_Values;
};