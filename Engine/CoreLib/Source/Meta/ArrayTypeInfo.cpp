#include "Meta/ArrayTypeInfo.h"

FArrayTypeInfo::FArrayTypeInfo(int32 size, int32 alignment, const FTypeInfo* elementType)
	: FTypeInfo(GenerateArrayTypeName(m_FormattedName, elementType), size, alignment)
	, m_ElementType { elementType }
{
}

const FTypeInfo* FArrayTypeInfo::GetElementType() const
{
	return m_ElementType;
}

FStringView FArrayTypeInfo::GenerateArrayTypeName(FString& targetString, const FTypeInfo* elementType)
{
	targetString = FString::Format("TArray<{}>"_sv, elementType->GetName());
	return targetString.AsStringView();
}