#include "MetaTool/ClassInfo.h"
#include "MetaTool/StructInfo.h"

static bool IsObjectBasedType(const FParsedStructInfo& typeInfo)
{
	return FParsedClassInfo::IsObjectBasedName(typeInfo.TypeName) ||
	       FParsedClassInfo::IsObjectBasedName(typeInfo.BaseTypeName);
}

bool FParsedStructInfo::HasObjectProperties() const
{
	if (Properties.IsEmpty())
	{
		return false;
	}

	for (const FParsedPropertyInfo& property : this->Properties)
	{
		if (IsObjectBasedName(property.PropertyType))
		{
			return true;
		}
	}
	return false;
}

bool FParsedStructInfo::IsObjectBasedName(const FStringView typeName)
{
	if (typeName.IsEmpty())
	{
		return false;
	}

	if (typeName.StartsWith("U"_sv))
	{
		return true;
	}

	if (typeName.StartsWith("TObjectPtr<"_sv) || typeName.StartsWith("TWeakObjectPtr<"_sv))
	{
		return typeName.EndsWith(">"_sv);
	}

	return typeName == "FObjectPtr"_sv || typeName == "FWeakObjectPtr"_sv;
}

bool FParsedStructInfo::IsObjectClass() const
{
	return IsObjectBasedType(*this);
}