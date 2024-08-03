#include "Meta/PointerTypeInfo.h"

static FStringView GeneratePointerTypeName(FString& targetString, const FTypeInfo* valueType)
{
	targetString = FString::Format("{}*"_sv, valueType->GetName());
	return targetString;
}

FPointerTypeInfo::FPointerTypeInfo(const FTypeInfo* valueType)
	: FTypeInfo(GeneratePointerTypeName(m_FormattedName, valueType), sizeof(void*), alignof(void*))
	, m_ValueType { valueType }
{
}

const FTypeInfo* FPointerTypeInfo::GetValueType() const
{
	return m_ValueType;
}