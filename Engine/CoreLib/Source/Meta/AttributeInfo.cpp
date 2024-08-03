#include "Meta/AttributeInfo.h"

///////////////////////////////////////////////////////////////////////////////
// FAttributeInfo

FAttributeInfo::FAttributeInfo(const FStringView name)
	: m_Name { name }
{
}

FAttributeInfo::FAttributeInfo(const FStringView name, const FStringView value)
	: m_Name { name }
	, m_Value { value }
{
}

FStringView FAttributeInfo::GetName() const
{
	return m_Name;
}

FStringView FAttributeInfo::GetValue() const
{
	return m_Value;
}

bool FAttributeInfo::HasValue() const
{
	return m_Value.IsEmpty() == false;
}

///////////////////////////////////////////////////////////////////////////////
// FAttributeCollectionInfo

FAttributeInfo& FAttributeCollectionInfo::FAttributeCollectionInfo::AddAttribute(FStringView name)
{
	return m_Attributes.Emplace(name);
}

FAttributeInfo& FAttributeCollectionInfo::AddAttribute(FStringView name, FStringView value)
{
	return m_Attributes.Emplace(name, value);
}

const FAttributeInfo* FAttributeCollectionInfo::GetAttribute(int32 index) const
{
	if (m_Attributes.IsValidIndex(index))
	{
		return &m_Attributes[index];
	}

	return nullptr;
}

const FAttributeInfo* FAttributeCollectionInfo::GetAttributeByName(const FStringView name) const
{
	return m_Attributes.FindByPredicate([name](const FAttributeInfo& attribute)
	{
		return attribute.GetName() == name;
	});
}

int32 FAttributeCollectionInfo::GetNumAttributes() const
{
	return m_Attributes.Num();
}

bool FAttributeCollectionInfo::HasAttribute(const FStringView name) const
{
	return GetAttributeByName(name) != nullptr;
}