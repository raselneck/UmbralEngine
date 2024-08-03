#include "Lox/Value.h"
#include "Math/Math.h"
#include "Misc/StringBuilder.h"

/**
 * @brief Compares two floating-point values.
 *
 * @param first The first value.
 * @param second The second value.
 * @return The comparison result.
 */
static constexpr ECompareResult CompareFloatValues(const double first, const double second)
{
	if (FMath::IsNearlyEqual(first, second))
	{
		return ECompareResult::Equals;
	}

	return first < second ? ECompareResult::LessThan : ECompareResult::GreaterThan;
}

TErrorOr<FLoxValue> FLoxValue::Add(const FLoxValue& first, const FLoxValue& second)
{
	const auto AddValuesAsString = [&]()
	{
		FStringBuilder result;
		first.AppendValueToStringBuilder(result);
		second.AppendValueToStringBuilder(result);
		return FLoxValue::FromString(result.ReleaseString());
	};

	if (first.IsNull() || first.IsBool())
	{
		if (second.IsString())
		{
			return AddValuesAsString();
		}
	}
	else if (first.IsInt())
	{
		if (second.IsInt())
		{
			return FLoxValue::FromInt(first.AsInt() + second.AsInt());
		}
		if (second.IsFloat())
		{
			return FLoxValue::FromFloat(first.AsFloat() + second.AsFloat());
		}
		if (second.IsString())
		{
			return AddValuesAsString();
		}
	}
	else if (first.IsFloat())
	{
		if (second.IsInt())
		{
			return FLoxValue::FromInt(first.AsInt() + second.AsInt());
		}
		if (second.IsFloat())
		{
			return FLoxValue::FromFloat(first.AsFloat() + second.AsFloat());
		}
		if (second.IsString())
		{
			return AddValuesAsString();
		}
	}
	else if (first.IsString())
	{
		return AddValuesAsString();
	}

	return MAKE_ERROR("Cannot add \"{}\" to \"{}\"", first.GetTypeName(), second.GetTypeName());
}

bool FLoxValue::AsBool() const
{
	return m_Value.Visit<bool>(FVariantVisitor
	{
		[](const FEmptyType) { return false; },
		[](const bool value) { return value; },
		[](const int64 value) { return value != 0; },
		[](const double value) { return FMath::IsNearlyZero(value) == false; },
		[](const FString& value) { return value.Length() > 0; }
	});
}

double FLoxValue::AsFloat() const
{
	return m_Value.Visit<double>(FVariantVisitor
	{
		[](const FEmptyType) { return 0.0; },
		[](const bool value) { return value ? 1.0 : 0.0; },
		[](const int64 value) { return static_cast<double>(value); },
		[](const double value) { return value; },
		[](const FString&) { return 0.0; }
	});
}

int64 FLoxValue::AsInt() const
{
	return m_Value.Visit<int64>(FVariantVisitor
	{
		[](const FEmptyType) -> int64 { return 0; },
		[](const bool value) -> int64 { return value ? 1 : 0; },
		[](const int64 value) -> int64 { return value; },
		[](const double value) -> int64 { return static_cast<int64>(value); },
		[](const FString&) -> int64 { return 0; }
	});
}

FString FLoxValue::AsString() const
{
	FStringBuilder result;

	m_Value.Visit(FVariantVisitor
	{
		[&](const FEmptyType) { result.Append("null"_sv); },
		[&](const bool value) { result.Append(value ? "true"_sv : "false"_sv); },
		[&](const auto& value) { result.Append(value); }
	});

	return result.ReleaseString();
}

FStringView FLoxValue::AsStringView() const
{
	return m_Value.Visit<FStringView>(FVariantVisitor
	{
		[&](const FString& value) { return value.AsStringView(); },
		[&](const auto&) -> FStringView { return {}; }
	});
}

TErrorOr<ECompareResult> FLoxValue::Compare(const FLoxValue& first, const FLoxValue& second)
{
	if (first.IsBool())
	{
		if (second.IsBool())
		{
			const bool firstValue = first.AsBool();
			const bool secondValue = second.AsBool();
			return TComparisonTraits<bool>::Compare(firstValue, secondValue);
		}
	}
	else if (first.IsInt())
	{
		if (second.IsInt())
		{
			const int64 firstValue = first.AsInt();
			const int64 secondValue = second.AsInt();
			return TComparisonTraits<int64>::Compare(firstValue, secondValue);
		}
		else if (second.IsFloat())
		{
			const double firstValue = first.AsFloat();
			const double secondValue = second.AsFloat();
			return CompareFloatValues(firstValue, secondValue);
		}
	}
	else if (first.IsFloat())
	{
		if (second.IsNumber())
		{
			const double firstValue = first.AsFloat();
			const double secondValue = second.AsFloat();
			return CompareFloatValues(firstValue, secondValue);
		}
	}
	else if (first.IsString())
	{
		if (second.IsString())
		{
			const FStringView firstValue = first.AsStringView();
			const FStringView secondValue = second.AsStringView();
			return firstValue.Compare(secondValue);
		}
	}

	return MAKE_ERROR("Cannot compare \"{}\" and \"{}\"", first.GetTypeName(), second.GetTypeName());
}

TErrorOr<FLoxValue> FLoxValue::Divide(const FLoxValue& first, const FLoxValue& second)
{
	if (first.IsInt())
	{
		if (second.IsInt())
		{
			const int64 secondValue = second.AsInt();
			if (secondValue == 0)
			{
				return MAKE_ERROR("Attempting to divide by zero");
			}

			return FLoxValue::FromInt(first.AsInt() / secondValue);
		}
		if (second.IsFloat())
		{
			const double secondValue = second.AsFloat();
			if (FMath::IsNearlyZero(secondValue))
			{
				return MAKE_ERROR("Attempting to divide by zero");
			}

			return FLoxValue::FromFloat(first.AsFloat() / secondValue);
		}
	}
	else if (first.IsFloat())
	{
		if (second.IsInt() || second.IsFloat())
		{
			const double secondValue = second.AsFloat();
			if (FMath::IsNearlyZero(secondValue))
			{
				return MAKE_ERROR("Attempting to divide by zero (divisor={})", secondValue);
			}

			return FLoxValue::FromFloat(first.AsFloat() / second.AsFloat());
		}
	}

	return MAKE_ERROR("Cannot divide \"{}\" by \"{}\"", first.GetTypeName(), second.GetTypeName());
}

FLoxValue FLoxValue::FromBool(bool value)
{
	FLoxValue result;
	result.m_Value.ResetToType<bool>(value);
	return result;
}

FLoxValue FLoxValue::FromFloat(const double value)
{
	FLoxValue result;
	result.m_Value.ResetToType<double>(value);
	return result;
}

FLoxValue FLoxValue::FromInt(const int64 value)
{
	FLoxValue result;
	result.m_Value.ResetToType<int64>(value);
	return result;
}

FLoxValue FLoxValue::FromString(const FStringView value)
{
	FLoxValue result;
	result.m_Value.ResetToType<FString>(value);
	return result;
}

FLoxValue FLoxValue::FromString(FString&& value)
{
	FLoxValue result;
	result.m_Value.ResetToType<FString>(MoveTemp(value));
	return result;
}

ELoxValueType FLoxValue::GetType() const
{
	return m_Value.Visit<ELoxValueType>(FVariantVisitor
	{
		[](const FEmptyType) { return ELoxValueType::Null; },
		[](const bool) { return ELoxValueType::Boolean; },
		[](const int64) { return ELoxValueType::Integer; },
		[](const double) { return ELoxValueType::Float; },
		[](const FString&) { return ELoxValueType::String; }
	});
}

FStringView FLoxValue::GetTypeName() const
{
	switch (GetType())
	{
	case ELoxValueType::Null:    return "null"_sv;
	case ELoxValueType::Boolean: return "bool"_sv;
	case ELoxValueType::Float:   return "float"_sv;
	case ELoxValueType::Integer: return "int"_sv;
	case ELoxValueType::String:  return "string"_sv;
	default:                     return "<unknown>"_sv;
	}
}

TErrorOr<FLoxValue> FLoxValue::LogicalAnd(const FLoxValue& first, const FLoxValue& second)
{
	return FLoxValue::FromBool(first.AsBool() && second.AsBool());
}

TErrorOr<FLoxValue> FLoxValue::LogicalNot(const FLoxValue& value)
{
	if (value.IsBool())
	{
		return FLoxValue::FromBool(!value.AsBool());
	}
	else if (value.IsInt())
	{
		return FLoxValue::FromBool(value.AsInt() == 0);
	}
	else if (value.IsFloat())
	{
		return FLoxValue::FromBool(FMath::IsNearlyZero(value.AsFloat()));
	}

	return MAKE_ERROR("Cannot perform logical not on \"{}\" value", value.GetTypeName());
}

TErrorOr<FLoxValue> FLoxValue::LogicalOr(const FLoxValue& first, const FLoxValue& second)
{
	return FLoxValue::FromBool(first.AsBool() || second.AsBool());
}

TErrorOr<FLoxValue> FLoxValue::Multiply(const FLoxValue& first, const FLoxValue& second)
{
	if (first.IsInt())
	{
		if (second.IsInt())
		{
			return FLoxValue::FromInt(first.AsInt() * second.AsInt());
		}
		if (second.IsFloat())
		{
			return FLoxValue::FromFloat(first.AsFloat() * second.AsFloat());
		}
	}
	else if (first.IsFloat())
	{
		if (second.IsInt() || second.IsFloat())
		{
			return FLoxValue::FromFloat(first.AsFloat() * second.AsFloat());
		}
	}

	return MAKE_ERROR("Cannot multiply \"{}\" with \"{}\"", first.GetTypeName(), second.GetTypeName());
}

TErrorOr<FLoxValue> FLoxValue::Negate(const FLoxValue& value)
{
	if (value.IsInt())
	{
		return FLoxValue::FromInt(-value.AsInt());
	}
	else if (value.IsFloat())
	{
		return FLoxValue::FromFloat(-value.AsFloat());
	}

	return MAKE_ERROR("Cannot negate \"{}\" value", value.GetTypeName());
}

TErrorOr<FLoxValue> FLoxValue::Subtract(const FLoxValue& first, const FLoxValue& second)
{
	if (first.IsInt())
	{
		if (second.IsInt())
		{
			return FLoxValue::FromInt(first.AsInt() - second.AsInt());
		}
		if (second.IsFloat())
		{
			return FLoxValue::FromFloat(first.AsFloat() - second.AsFloat());
		}
	}
	else if (first.IsFloat())
	{
		if (second.IsInt() || second.IsFloat())
		{
			return FLoxValue::FromFloat(first.AsFloat() - second.AsFloat());
		}
	}

	return MAKE_ERROR("Cannot subtract \"{}\" from \"{}\"", first.GetTypeName(), second.GetTypeName());
}

void FLoxValue::AppendValueToStringBuilder(FStringBuilder& builder) const
{
	m_Value.Visit(FVariantVisitor
	{
		[&](const FEmptyType) { builder.Append("null"_sv); },
		[&](const bool value) { builder.Append(value ? "true"_sv : "false"_sv); },
		[&](const auto& value) { builder.Append(value); }
	});
}

void TFormatter<FLoxValue>::BuildString(const FLoxValue& value, FStringBuilder& builder)
{
	builder.Append(value.GetTypeName());
	builder.Append(":"_sv);

	if (value.IsString())
	{
		builder.Append('"', 1);
		builder.Append(value.AsString());
		builder.Append('"', 1);
	}
	else
	{
		builder.Append(value.AsString());
	}
}

bool TFormatter<FLoxValue>::Parse(const FStringView formatString) const
{
	return formatString.IsEmpty();
}