#include "Meta/ClassInfo.h"

FClassInfo::FClassInfo(const FStringView name, const int32 size, const int32 alignment, const FClassInfo* baseType)
	: FStructInfo(name, size, alignment, baseType)
{
}

bool FClassInfo::CanConstruct() const
{
	return m_ConstructFunction != nullptr;
}

bool FClassInfo::CanConstructAtLocation() const
{
	return m_ConstructAtLocationFunction != nullptr;
}

void* FClassInfo::Construct() const
{
	if (m_ConstructFunction == nullptr)
	{
		return nullptr;
	}

	constexpr TBadge<FClassInfo> badge;
	return m_ConstructFunction(badge);
}

void FClassInfo::ConstructAtLocation(void* location) const
{
	if (m_ConstructAtLocationFunction == nullptr || location == nullptr)
	{
		return;
	}

	constexpr TBadge<FClassInfo> badge;
	m_ConstructAtLocationFunction(badge, location);
}

void FClassInfo::SetConstructClassFunction(FConstructClassFunction function)
{
	m_ConstructFunction = function;
}

void FClassInfo::SetConstructClassAtLocationFunction(FConstructClassAtLocationFunction function)
{
	m_ConstructAtLocationFunction = function;
}