#pragma once

#include "Containers/String.h"
#include "Containers/Variant.h"
#include "Lox/Error.h"

/**
 * @brief An enumeration of possible value types.
 */
enum class ELoxValueType : uint8
{
	Null,
	Boolean,
	Float,
	Integer,
	String
};

/**
 * @brief Defines a Lox value.
 */
class FLoxValue
{
public:

	/**
	 * @brief Adds two values together.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return The sum.
	 */
	[[nodiscard]] static TErrorOr<FLoxValue> Add(const FLoxValue& first, const FLoxValue& second);

	/**
	 * @brief Evaluates this value as a Boolean value.
	 *
	 * @return This value as a Boolean value.
	 */
	[[nodiscard]] bool AsBool() const;

	/**
	 * @brief Evaluates this value as a floating-point number.
	 *
	 * @return This value as a floating-point number.
	 */
	[[nodiscard]] double AsFloat() const;

	/**
	 * @brief Evaluates this value as an integral number.
	 *
	 * @return This value as an integral number.
	 */
	[[nodiscard]] int64 AsInt() const;

	/**
	 * @brief Evaluates this value as a string.
	 *
	 * If this value is not a literal string, the underlying value will be converted to a string.
	 *
	 * @return This value as a string.
	 */
	[[nodiscard]] FString AsString() const;

	/**
	 * @brief Evaluates this value as a string view.
	 *
	 * If this value is not a string, then this will return an empty string view.
	 *
	 * @return This value as a string view.
	 */
	[[nodiscard]] FStringView AsStringView() const;

	/**
	 * @brief Attempts to compare two Lox values.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return The result of the comparison.
	 */
	[[nodiscard]] static TErrorOr<ECompareResult> Compare(const FLoxValue& first, const FLoxValue& second);

	/**
	 * @brief Divides one value by another.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return The quotient.
	 */
	[[nodiscard]] static TErrorOr<FLoxValue> Divide(const FLoxValue& first, const FLoxValue& second);

	/**
	 * @brief Creates a value from a Boolean value.
	 *
	 * @param value The value.
	 * @return The value.
	 */
	[[nodiscard]] static FLoxValue FromBool(bool value);

	/**
	 * @brief Creates a value from a floating-point value.
	 *
	 * @param value The value.
	 * @return The value.
	 */
	[[nodiscard]] static FLoxValue FromFloat(double value);

	/**
	 * @brief Creates a value from an integral value.
	 *
	 * @param value The value.
	 * @return The value.
	 */
	[[nodiscard]] static FLoxValue FromInt(int64 value);

	/**
	 * @brief Creates a value from a string value.
	 *
	 * @param value The value.
	 * @return The value.
	 */
	[[nodiscard]] static FLoxValue FromString(FStringView value);

	/**
	 * @brief Creates a value from a string value.
	 *
	 * @param value The value.
	 * @return The value.
	 */
	[[nodiscard]] static FLoxValue FromString(FString&& value);

	/**
	 * @brief Gets the type of the underlying value.
	 *
	 * @return The underlying value type.
	 */
	[[nodiscard]] ELoxValueType GetType() const;

	/**
	 * @brief Gets the name of this value's type.
	 *
	 * @return The name of this value's type.
	 */
	[[nodiscard]] FStringView GetTypeName() const;

	/**
	 * @brief Checks to see if this value is a Boolean.
	 *
	 * @return True if this value is a Boolean, otherwise false.
	 */
	[[nodiscard]] bool IsBool() const
	{
		return m_Value.Is<bool>();
	}

	/**
	 * @brief Checks to see if this value is a float.
	 *
	 * @return True if this value is a float, otherwise false.
	 */
	[[nodiscard]] bool IsFloat() const
	{
		return m_Value.Is<double>();
	}

	/**
	 * @brief Checks to see if this value is an int.
	 *
	 * @return True if this value is an int, otherwise false.
	 */
	[[nodiscard]] bool IsInt() const
	{
		return m_Value.Is<int64>();
	}

	/**
	 * @brief Checks to see if this value is null.
	 *
	 * @return True if this value is null, otherwise false.
	 */
	[[nodiscard]] bool IsNull() const
	{
		return m_Value.Is<FEmptyType>();
	}

	/**
	 * @brief Checks to see if this value is a number.
	 *
	 * @return True if this value is a number, otherwise false.
	 */
	[[nodiscard]] bool IsNumber() const
	{
		return IsFloat() || IsInt();
	}

	/**
	 * @brief Checks to see if this value is a string.
	 *
	 * @return True if this value is a string, otherwise false.
	 */
	[[nodiscard]] bool IsString() const
	{
		return m_Value.Is<FString>();
	}

	/**
	 * @brief Performs a logical "and" between two values.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return The result of the logical "and."
	 */
	[[nodiscard]] static TErrorOr<FLoxValue> LogicalAnd(const FLoxValue& first, const FLoxValue& second);

	/**
	 * @brief Performs a logical "not" on a value.
	 *
	 * @param value The value.
	 * @return The result of the logical "not."
	 */
	[[nodiscard]] static TErrorOr<FLoxValue> LogicalNot(const FLoxValue& value);

	/**
	 * @brief Performs a logical "or" between two values.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return The result of the logical "or."
	 */
	[[nodiscard]] static TErrorOr<FLoxValue> LogicalOr(const FLoxValue& first, const FLoxValue& second);

	// TODO LogicalXor

	/**
	 * @brief Multiplies two values together.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return The product.
	 */
	[[nodiscard]] static TErrorOr<FLoxValue> Multiply(const FLoxValue& first, const FLoxValue& second);

	/**
	 * @brief Negates the given Lox value.
	 *
	 * @param value The value.
	 * @return The negated value.
	 */
	[[nodiscard]] static TErrorOr<FLoxValue> Negate(const FLoxValue& value);

	/**
	 * @brief Subtracts one value from another.
	 *
	 * @param first The first value.
	 * @param second The second value.
	 * @return The difference.
	 */
	[[nodiscard]] static TErrorOr<FLoxValue> Subtract(const FLoxValue& first, const FLoxValue& second);

private:

	using VariantType = TVariant<FEmptyType, bool, int64, double, FString>;

	/**
	 * @brief Appends the underlying value to the given string builder.
	 *
	 * @param builder The string builder.
	 */
	void AppendValueToStringBuilder(FStringBuilder& builder) const;

	VariantType m_Value;
};

/**
 * @brief Defines a string formatter for a Lox value.
 */
template<>
struct TFormatter<FLoxValue>
{
	void BuildString(const FLoxValue& value, FStringBuilder& builder);
	bool Parse(FStringView formatString) const;
};