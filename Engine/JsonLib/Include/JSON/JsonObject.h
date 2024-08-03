#pragma once

#include "Containers/HashMap.h"
#include "Containers/String.h"
#include "JSON/JsonValueType.h"

class FJsonArray;
class FJsonValue;

/**
 * @brief The key-value pair type using by JSON objects.
 */
using FJsonObjectKeyValuePair = TKeyValuePair<FString, FJsonValue>;

/**
 * @brief Defines an array of JSON values.
 */
class FJsonObject final
{
public:

	/**
	 * @brief Checks to see if this object contains a value with the given key.
	 *
	 * @param key The key.
	 * @return True if this object contains a value with the given key, otherwise false.
	 */
	[[nodiscard]] bool Contains(FStringView key) const;

	/**
	 * @brief Checks to see if this object contains a value with the given key.
	 *
	 * @param key The key.
	 * @return True if this object contains a value with the given key, otherwise false.
	 */
	[[nodiscard]] bool Contains(const FString& key) const;

	/**
	 * @brief Finds the value with the given key.
	 *
	 * @param key The key.
	 * @return The value corresponding to \p key, otherwise nullptr.
	 */
	[[nodiscard]] const FJsonValue* Find(FStringView key) const;

	/**
	 * @brief Finds the value with the given key.
	 *
	 * @param key The key.
	 * @return The value corresponding to \p key, otherwise nullptr.
	 */
	[[nodiscard]] const FJsonValue* Find(const FString& key) const;

	/**
	 * @brief Finds the value with the given key.
	 *
	 * @param key The key.
	 * @return The value corresponding to \p key, otherwise nullptr.
	 */
	[[nodiscard]] FJsonValue* Find(FStringView key);

	/**
	 * @brief Finds the value with the given key.
	 *
	 * @param key The key.
	 * @return The value corresponding to \p key, otherwise nullptr.
	 */
	[[nodiscard]] FJsonValue* Find(const FString& key);

	/**
	 * @brief Finds the number of values in this JSON array.
	 *
	 * @return The number of values in this JSON array.
	 */
	[[nodiscard]] int32 Num() const
	{
		return m_Values.Num();
	}

	/**
	 * @brief Sets a value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param type The value's type.
	 * @return The value.
	 */
	[[maybe_unused]] FJsonValue& Set(FStringView key, EJsonValueType type);

	/**
	 * @brief Sets an array value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param array The array value.
	 */
	void Set(FStringView key, const FJsonArray& array);

	/**
	 * @brief Sets an array value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param array The array value.
	 */
	void Set(FStringView key, FJsonArray&& array);

	/**
	 * @brief Sets a number value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param number The number value.
	 */
	void Set(FStringView key, double number);

	/**
	 * @brief Sets an object value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param object The object value.
	 */
	void Set(FStringView key, const FJsonObject& object);

	/**
	 * @brief Sets an object value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param object The object value.
	 */
	void Set(FStringView key, FJsonObject&& object);

	/**
	 * @brief Sets a string value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param string The string value.
	 */
	void Set(FStringView key, const FString& string);

	/**
	 * @brief Sets a string value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param string The string value.
	 */
	void Set(FStringView key, FString&& string);

	/**
	 * @brief Sets a string value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param string The string value.
	 */
	void Set(FStringView key, FStringView string);

	/**
	 * @brief Sets a value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param value The value.
	 */
	void Set(FStringView key, const FJsonValue& value);

	/**
	 * @brief Sets a value in this JSON object.
	 *
	 * @param key The value's key.
	 * @param value The value.
	 */
	void Set(FStringView key, FJsonValue&& value);

	/**
	 * @brief Sets a key-value pair.
	 *
	 * @param pair The pair.
	 */
	void SetPair(const FJsonObjectKeyValuePair& pair);

	/**
	 * @brief Sets a key-value pair.
	 *
	 * @param pair The pair.
	 */
	void SetPair(FJsonObjectKeyValuePair&& pair);

	/**
	 * @brief Attempts to unset the value defined to by the given key.
	 *
	 * @param key The key.
	 * @return True if the value was unset, otherwise false.
	 */
	[[maybe_unused]] bool Unset(FStringView key);

	/**
	 * @brief Attempts to unset the value defined to by the given key.
	 *
	 * @param key The key.
	 * @return True if the value was unset, otherwise false.
	 */
	[[maybe_unused]] bool Unset(const FString& key);

private:

	THashMap<FString, FJsonValue> m_Values;
};