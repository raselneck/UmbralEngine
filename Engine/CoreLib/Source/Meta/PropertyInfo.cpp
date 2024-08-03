#include "Engine/Cast.h"
#include "Meta/PropertyInfo.h"
#include "Meta/StructInfo.h"

FPropertyInfo::FPropertyInfo(const FStringView name, const FTypeInfo* valueType, const int32 offset)
	: m_Name { name }
	, m_ValueType { valueType }
	, m_Offset { offset }
{
}

FStringView FPropertyInfo::GetName() const
{
	return m_Name;
}

int32 FPropertyInfo::GetOffset() const
{
	return m_Offset;
}

const FTypeInfo* FPropertyInfo::GetValueType() const
{
	return m_ValueType;
}

bool FPropertyInfo::DoesTypeHaveThisProperty(const FTypeInfo* typeInfo) const
{
	if (const FStructInfo* structInfo = Cast<FStructInfo>(typeInfo))
	{
		return structInfo->GetPropertyByName(m_Name) == this;
	}
	return false;
}

void* FPropertyInfo::GetValuePointer(void* container) const
{
	uint8* containerAsBytes = reinterpret_cast<uint8*>(container);
	uint8* valuePointer = containerAsBytes + static_cast<usize>(m_Offset);
	return static_cast<void*>(valuePointer);
}