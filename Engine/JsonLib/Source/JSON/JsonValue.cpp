#include "Engine/Assert.h"
#include "JSON/JsonValue.h"

FJsonValue::FJsonValue(const EJsonValueType type)
{
	SetValueByType(type);
}

const FJsonArray* FJsonValue::AsArray() const
{
	return m_Value.GetValuePointer<FJsonArray>();
}

FJsonArray* FJsonValue::AsArray()
{
	return m_Value.GetValuePointer<FJsonArray>();
}

double FJsonValue::AsNumber() const
{
	if (const double* value = m_Value.GetValuePointer<double>())
	{
		return *value;
	}
	return 0.0;
}

const FJsonObject* FJsonValue::AsObject() const
{
	return m_Value.GetValuePointer<FJsonObject>();
}

FJsonObject* FJsonValue::AsObject()
{
	return m_Value.GetValuePointer<FJsonObject>();
}

const FString* FJsonValue::AsString() const
{
	return m_Value.GetValuePointer<FString>();
}

FString* FJsonValue::AsString()
{
	return m_Value.GetValuePointer<FString>();
}

FStringView FJsonValue::AsStringView() const
{
	if (const FString* value = AsString())
	{
		return value->AsStringView();
	}
	return {};
}

FJsonValue FJsonValue::CopyArray(const FJsonArray& value)
{
	FJsonValue result;
	result.m_Value.ResetToType<FJsonArray>(value);
	return result;
}

FJsonValue FJsonValue::CopyObject(const FJsonObject& value)
{
	FJsonValue result;
	result.m_Value.ResetToType<FJsonObject>(value);
	return result;
}

FJsonValue FJsonValue::CopyString(const FString& value)
{
	FJsonValue result;
	result.m_Value.ResetToType<FString>(value);
	return result;
}

FJsonValue FJsonValue::FromArray(FJsonArray value)
{
	FJsonValue result;
	result.m_Value.ResetToType<FJsonArray>(MoveTemp(value));
	return result;
}

FJsonValue FJsonValue::FromNumber(const double value)
{
	FJsonValue result;
	result.m_Value.ResetToType<double>(value);
	return result;
}

FJsonValue FJsonValue::FromObject(FJsonObject value)
{
	FJsonValue result;
	result.m_Value.ResetToType<FJsonObject>(MoveTemp(value));
	return result;
}

FJsonValue FJsonValue::FromString(FString value)
{
	FJsonValue result;
	result.m_Value.ResetToType<FString>(MoveTemp(value));
	return result;
}

FJsonValue FJsonValue::FromString(const FStringView value)
{
	FJsonValue result;
	result.m_Value.ResetToType<FString>(value);
	return result;
}

EJsonValueType FJsonValue::GetType() const
{
	return m_Value.Visit<EJsonValueType>(FVariantVisitor
	{
		[](const FEmptyType&)  { return EJsonValueType::Null; },
		[](const double&)      { return EJsonValueType::Number; },
		[](const FString&)     { return EJsonValueType::String; },
		[](const FJsonArray&)  { return EJsonValueType::Array; },
		[](const FJsonObject&) { return EJsonValueType::Object; }
	});
}

bool FJsonValue::IsArray() const
{
	return m_Value.Is<FJsonArray>();
}

bool FJsonValue::IsNull() const
{
	return m_Value.Is<FEmptyType>();
}

bool FJsonValue::IsNumber() const
{
	return m_Value.Is<double>();
}

bool FJsonValue::IsObject() const
{
	return m_Value.Is<FJsonObject>();
}

bool FJsonValue::IsString() const
{
	return m_Value.Is<FString>();
}

void FJsonValue::SetArray(const FJsonArray& array)
{
	m_Value.ResetToType<FJsonArray>(array);
}

void FJsonValue::SetArray(FJsonArray&& array)
{
	m_Value.ResetToType<FJsonArray>(MoveTemp(array));
}

void FJsonValue::SetNull()
{
	m_Value.ResetToType<FEmptyType>();
}

void FJsonValue::SetNumber(const double number)
{
	m_Value.ResetToType<double>(number);
}

void FJsonValue::SetObject(const FJsonObject& object)
{
	m_Value.ResetToType<FJsonObject>(object);
}

void FJsonValue::SetObject(FJsonObject&& object)
{
	m_Value.ResetToType<FJsonObject>(MoveTemp(object));
}

void FJsonValue::SetString(const FString& string)
{
	m_Value.ResetToType<FString>(string);
}

void FJsonValue::SetString(FString&& string)
{
	m_Value.ResetToType<FString>(MoveTemp(string));
}

void FJsonValue::SetString(const FStringView string)
{
	m_Value.ResetToType<FString>(string);
}

void FJsonValue::SetValueByType(EJsonValueType type)
{
	switch (type)
	{
	case EJsonValueType::Null:
		SetNull();
		break;
	case EJsonValueType::Number:
		SetNumber(0.0);
		break;
	case EJsonValueType::String:
		SetString(""_sv);
		break;
	case EJsonValueType::Array:
		m_Value.ResetToType<FJsonArray>();
		break;
	case EJsonValueType::Object:
		m_Value.ResetToType<FJsonObject>();
		break;
	default:
		UM_ASSERT_NOT_REACHED_MSG("Unsupported JSON value type supplied");
		break;
	}
}