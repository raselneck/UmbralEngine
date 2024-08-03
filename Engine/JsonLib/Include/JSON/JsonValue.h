#pragma once

#include "Containers/Variant.h"
#include "JSON/JsonArray.h"
#include "JSON/JsonObject.h"
#include "JSON/JsonValueType.h"

/**
 * @brief Defines the base for JSON values.
 */
class FJsonValue final
{
public:

	/**
	 * @brief Sets default values for this JSON value's properties.
	 */
	FJsonValue() = default;

	/**
	 * @brief Explicitly initializes this JSON value as the given type.
	 *
	 * @param type The value type.
	 */
	explicit FJsonValue(EJsonValueType type);

	/**
	 * @brief Destroys this JSON value.
	 */
	virtual ~FJsonValue() = default;

	/**
	 * @brief Gets this JSON value as an array.
	 *
	 * @return This JSON value's underlying array value.
	 */
	[[nodiscard]] const FJsonArray* AsArray() const;

	/**
	 * @brief Gets this JSON value as an array.
	 *
	 * @return This JSON value's underlying array value.
	 */
	[[nodiscard]] FJsonArray* AsArray();

	/**
	 * @brief Gets this JSON value as a number.
	 *
	 * @return This JSON value's underlying number value.
	 */
	[[nodiscard]] double AsNumber() const;

	/**
	 * @brief Gets this JSON value as an object.
	 *
	 * @return This JSON value's underlying object value.
	 */
	[[nodiscard]] const FJsonObject* AsObject() const;

	/**
	 * @brief Gets this JSON value as an object.
	 *
	 * @return This JSON value's underlying object value.
	 */
	[[nodiscard]] FJsonObject* AsObject();

	/**
	 * @brief Gets this JSON value as a string.
	 *
	 * @return This JSON value's underlying string value.
	 */
	[[nodiscard]] const FString* AsString() const;

	/**
	 * @brief Gets this JSON value as a string.
	 *
	 * @return This JSON value's underlying string value.
	 */
	[[nodiscard]] FString* AsString();

	/**
	 * @brief Gets this JSON value as a string view.
	 *
	 * @return The string view of this JSON value's underlying string value.
	 */
	[[nodiscard]] FStringView AsStringView() const;

	/**
	 * @brief Creates a JSON value from an array.
	 *
	 * @param value The array value.
	 * @return The JSON value.
	 */
	[[nodiscard]] static FJsonValue CopyArray(const FJsonArray& value);

	/**
	 * @brief Creates a JSON value from an object.
	 *
	 * @param value The object value.
	 * @return The JSON value.
	 */
	[[nodiscard]] static FJsonValue CopyObject(const FJsonObject& value);

	/**
	 * @brief Creates a JSON value from a string.
	 *
	 * @param value The string value.
	 * @return The JSON value.
	 */
	[[nodiscard]] static FJsonValue CopyString(const FString& value);

	/**
	 * @brief Creates a JSON value from an array.
	 *
	 * @param value The array value.
	 * @return The JSON value.
	 */
	[[nodiscard]] static FJsonValue FromArray(FJsonArray value);

	/**
	 * @brief Creates a JSON value from a number.
	 *
	 * @param value The number value.
	 * @return The JSON value.
	 */
	[[nodiscard]] static FJsonValue FromNumber(double value);

	/**
	 * @brief Creates a JSON value from an object.
	 *
	 * @param value The object value.
	 * @return The JSON value.
	 */
	[[nodiscard]] static FJsonValue FromObject(FJsonObject value);

	/**
	 * @brief Creates a JSON value from a string.
	 *
	 * @param value The string value.
	 * @return The JSON value.
	 */
	[[nodiscard]] static FJsonValue FromString(FString value);

	/**
	 * @brief Creates a JSON value from a string view.
	 *
	 * @param value The string view value.
	 * @return The JSON value.
	 */
	[[nodiscard]] static FJsonValue FromString(FStringView value);

	/**
	 * @brief Gets this JSON value's type.
	 *
	 * @return This JSON value's type.
	 */
	[[nodiscard]] EJsonValueType GetType() const;

	/**
	 * @brief Checks to see if this JSON value is an array.
	 *
	 * @return True if this JSON value is an array, otherwise false.
	 */
	[[nodiscard]] bool IsArray() const;

	/**
	 * @brief Checks to see if this JSON value is null.
	 *
	 * @return True if this JSON value is null, otherwise false.
	 */
	[[nodiscard]] bool IsNull() const;

	/**
	 * @brief Checks to see if this JSON value is a number.
	 *
	 * @return True if this JSON value is a number, otherwise false.
	 */
	[[nodiscard]] bool IsNumber() const;

	/**
	 * @brief Checks to see if this JSON value is an object.
	 *
	 * @return True if this JSON value is an object, otherwise false.
	 */
	[[nodiscard]] bool IsObject() const;

	/**
	 * @brief Checks to see if this JSON value is a string.
	 *
	 * @return True if this JSON value is a string, otherwise false.
	 */
	[[nodiscard]] bool IsString() const;

	/**
	 * @brief Sets this JSON value to be an array.
	 *
	 * @param array The array value.
	 */
	void SetArray(const FJsonArray& array);

	/**
	 * @brief Sets this JSON value to be an array.
	 *
	 * @param array The array value.
	 */
	void SetArray(FJsonArray&& array);

	/**
	 * @brief Sets this JSON value to be null.
	 */
	void SetNull();

	/**
	 * @brief Sets this JSON value to be a number.
	 *
	 * @param number The number value.
	 */
	void SetNumber(double number);

	/**
	 * @brief Sets this JSON value to be an object.
	 *
	 * @param object The object value.
	 */
	void SetObject(const FJsonObject& object);

	/**
	 * @brief Sets this JSON value to be an object.
	 *
	 * @param object The object value.
	 */
	void SetObject(FJsonObject&& object);

	/**
	 * @brief Sets this JSON value to be a string.
	 *
	 * @param string The string value.
	 */
	void SetString(const FString& string);

	/**
	 * @brief Sets this JSON value to be a string.
	 *
	 * @param string The string value.
	 */
	void SetString(FString&& string);

	/**
	 * @brief Sets this JSON value to be a string.
	 *
	 * @param string The string value.
	 */
	void SetString(FStringView string);

	/**
	 * @brief Sets this JSON value's type. The new value will be default initialized.
	 *
	 * @param type The new value type.
	 */
	void SetValueByType(EJsonValueType type);

private:

	TVariant<FEmptyType, double, FString, FJsonArray, FJsonObject> m_Value;
};