#include "Meta/EnumInfo.h"

///////////////////////////////////////////////////////////////////////////////
// FEnumEntryInfo

FEnumEntryInfo::FEnumEntryInfo(FStringView name, int64 value)
	: m_Name { name }
	, m_Value { value }
{
}

FStringView FEnumEntryInfo::GetName() const
{
	return m_Name;
}

int64 FEnumEntryInfo::GetValue() const
{
	return m_Value;
}

///////////////////////////////////////////////////////////////////////////////
// FEnumEntryInfo

FEnumInfo::FEnumInfo(FStringView name, const FTypeInfo* underlyingType)
	: FTypeInfo(name, underlyingType->GetSize(), underlyingType->GetAlignment())
	, m_UnderlyingType { underlyingType }
{
}

void FEnumInfo::AddEntry(const FStringView name, const int64 value)
{
	(void)m_Entries.Emplace(name, value);
}

const FEnumEntryInfo* FEnumInfo::GetEntryByIndex(const int32 index) const
{
	if (m_Entries.IsValidIndex(index))
	{
		return &m_Entries[index];
	}

	return nullptr;
}

const FEnumEntryInfo* FEnumInfo::GetEntryByName(FStringView name) const
{
	return m_Entries.FindByPredicate([name](const FEnumEntryInfo& entry)
	{
		return entry.GetName() == name;
	});
}

const FEnumEntryInfo* FEnumInfo::GetEntryByValue(const int64 value) const
{
	return m_Entries.FindByPredicate([value](const FEnumEntryInfo& entry)
	{
		return entry.GetValue() == value;
	});
}

int32 FEnumInfo::GetNumEntries() const
{
	return m_Entries.Num();
}

const FTypeInfo* FEnumInfo::GetUnderlyingType() const
{
	return m_UnderlyingType;
}