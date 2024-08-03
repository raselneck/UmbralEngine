#include "JSON/JsonArray.h"
#include "JSON/JsonObject.h"
#include "JSON/JsonValue.h"

FJsonArray::FJsonArray(TArray<FJsonValue> values)
	: m_Values { MoveTemp(values) }
{
}

FJsonArray::FJsonArray(const TArray<FJsonValue>& values)
	: m_Values { values }
{
}

FJsonValue& FJsonArray::Add(const EJsonValueType type)
{
	return m_Values.Emplace(type);
}

FJsonValue& FJsonArray::Add(const FJsonArray& array)
{
	const SizeType index = Add(FJsonValue::CopyArray(array));
	return m_Values[index];
}

FJsonValue& FJsonArray::Add(FJsonArray&& array)
{
	const SizeType index = Add(FJsonValue::FromArray(MoveTemp(array)));
	return m_Values[index];
}

FJsonValue& FJsonArray::Add(const double number)
{
	const SizeType index = Add(FJsonValue::FromNumber(number));
	return m_Values[index];
}

FJsonValue& FJsonArray::Add(const FJsonObject& object)
{
	const SizeType index = Add(FJsonValue::CopyObject(object));
	return m_Values[index];
}

FJsonValue& FJsonArray::Add(FJsonObject&& object)
{
	const SizeType index = Add(FJsonValue::FromObject(MoveTemp(object)));
	return m_Values[index];
}

FJsonValue& FJsonArray::Add(const FString& string)
{
	const SizeType index = Add(FJsonValue::CopyString(string));
	return m_Values[index];
}

FJsonValue& FJsonArray::Add(FString&& string)
{
	const SizeType index = Add(FJsonValue::FromString(MoveTemp(string)));
	return m_Values[index];
}

FJsonValue& FJsonArray::Add(const FStringView string)
{
	const SizeType index = Add(FJsonValue::FromString(string));
	return m_Values[index];
}

FJsonArray::SizeType FJsonArray::Add(const FJsonValue& value)
{
	return m_Values.Add(value);
}

FJsonArray::SizeType FJsonArray::Add(FJsonValue&& value)
{
	return m_Values.Add(MoveTemp(value));
}

const FJsonValue& FJsonArray::At(const SizeType index) const
{
	return m_Values.At(index);
}

FJsonValue& FJsonArray::At(const SizeType index)
{
	return m_Values.At(index);
}

// STL compatibility BEGIN
FJsonArray::IteratorType      FJsonArray::begin()        { return m_Values.begin(); }
FJsonArray::ConstIteratorType FJsonArray::begin()  const { return m_Values.begin(); }
FJsonArray::ConstIteratorType FJsonArray::cbegin() const { return m_Values.cbegin(); }
FJsonArray::IteratorType      FJsonArray::end()          { return m_Values.end(); }
FJsonArray::ConstIteratorType FJsonArray::end()    const { return m_Values.end(); }
FJsonArray::ConstIteratorType FJsonArray::cend()   const { return m_Values.cend(); }
// STL compatibility END