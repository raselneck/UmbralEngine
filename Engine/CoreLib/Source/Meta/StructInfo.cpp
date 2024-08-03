#include "Meta/StructInfo.h"

FStructInfo::FStructInfo(const FStringView name, const int32 size, const int32 alignment, const FStructInfo* baseType)
	: FTypeInfo(name, size, alignment)
	, m_BaseType { baseType }
{
}

FPropertyInfo& FStructInfo::AddProperty(const FStringView name, const FTypeInfo* valueType, const int32 offset)
{
	return m_Properties.Emplace(name, valueType, offset);
}

const FStructInfo* FStructInfo::GetBaseType() const
{
	return m_BaseType;
}

int32 FStructInfo::GetNumProperties() const
{
	int32 numProperties = m_Properties.Num();
	if (m_BaseType)
	{
		numProperties += m_BaseType->GetNumProperties();
	}
	return numProperties;
}

const FPropertyInfo* FStructInfo::GetProperty(const int32 index) const
{
	if (m_Properties.IsValidIndex(index))
	{
		return &m_Properties.At(index);
	}

	if (m_BaseType)
	{
		return m_BaseType->GetProperty(index - m_Properties.Num());
	}

	return nullptr;
}

const FPropertyInfo* FStructInfo::GetPropertyByName(FStringView name) const
{
	const FPropertyInfo* property = m_Properties.FindByPredicate([name](const FPropertyInfo& property)
	{
		return property.GetName() == name;
	});

	if (property == nullptr && m_BaseType != nullptr)
	{
		property = m_BaseType->GetPropertyByName(name);
	}

	return property;
}

bool FStructInfo::IsA(const FStructInfo* type) const
{
	if (type == nullptr)
	{
		return false;
	}

	const FStructInfo* typeToCompareTo = this;
	while (typeToCompareTo != nullptr)
	{
		if (typeToCompareTo == type)
		{
			return true;
		}

		typeToCompareTo = typeToCompareTo->GetBaseType();
	}

	return false;
}