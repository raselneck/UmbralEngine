#include "JSON/JsonArray.h"
#include "JSON/JsonObject.h"
#include "JSON/JsonValue.h"

bool FJsonObject::Contains(const FStringView key) const
{
	return m_Values.ContainsKey(key);
}

bool FJsonObject::Contains(const FString& key) const
{
	return m_Values.ContainsKey(key);
}

const FJsonValue* FJsonObject::Find(const FStringView key) const
{
	return m_Values.Find(key);
}

const FJsonValue* FJsonObject::Find(const FString& key) const
{
	return m_Values.Find(key);
}

FJsonValue* FJsonObject::Find(const FStringView key)
{
	return m_Values.Find(key);
}

FJsonValue* FJsonObject::Find(const FString& key)
{
	return m_Values.Find(key);
}

FJsonValue& FJsonObject::Set(const FStringView key, const EJsonValueType type)
{
	FJsonValue& result = m_Values[key];
	result.SetValueByType(type);
	return result;
}

void FJsonObject::Set(const FStringView key, const FJsonArray& array)
{
	m_Values[key].SetArray(array);
}

void FJsonObject::Set(const FStringView key, FJsonArray&& array)
{
	m_Values[key].SetArray(MoveTemp(array));
}

void FJsonObject::Set(const FStringView key, const double number)
{
	m_Values[key].SetNumber(number);
}

void FJsonObject::Set(const FStringView key, const FJsonObject& object)
{
	m_Values[key].SetObject(object);
}

void FJsonObject::Set(const FStringView key, FJsonObject&& object)
{
	m_Values[key].SetObject(MoveTemp(object));
}

void FJsonObject::Set(const FStringView key, const FString& string)
{
	m_Values[key].SetString(string);
}

void FJsonObject::Set(const FStringView key, FString&& string)
{
	m_Values[key].SetString(MoveTemp(string));
}

void FJsonObject::Set(const FStringView key, const FStringView string)
{
	m_Values[key].SetString(string);
}

void FJsonObject::Set(const FStringView key, const FJsonValue& value)
{
	m_Values[key] = value;
}

void FJsonObject::Set(const FStringView key, FJsonValue&& value)
{
	m_Values[key] = MoveTemp(value);
}

void FJsonObject::SetPair(const FJsonObjectKeyValuePair& pair)
{
	m_Values.Add(pair);
}

void FJsonObject::SetPair(FJsonObjectKeyValuePair&& pair)
{
	m_Values.Add(MoveTemp(pair));
}

bool FJsonObject::Unset(const FStringView key)
{
	return m_Values.Remove(key);
}

bool FJsonObject::Unset(const FString& key)
{
	return m_Values.Remove(key);
}