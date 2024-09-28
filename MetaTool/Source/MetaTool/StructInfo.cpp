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

	return Properties.ContainsByPredicate([](const FParsedPropertyInfo& property)
	{
		return IsObjectBasedName(property.PropertyType);
	});
}

static bool IsObjectBasedTypeName(const FStringView typeName)
{
	if (typeName.IsEmpty())
	{
		return false;
	}

	if (typeName.StartsWith("U"_sv) || typeName.StartsWith("A"_sv))
	{
		return true;
	}

	if (typeName.StartsWith("TObjectPtr<"_sv) || typeName.StartsWith("TWeakObjectPtr<"_sv))
	{
		return typeName.EndsWith(">"_sv);
	}

	return typeName == "FObjectPtr"_sv || typeName == "FWeakObjectPtr"_sv;
}

static bool FindArrayElementTypeName(const FStringView typeName, FStringView& outElementTypeName)
{
	constexpr FStringView arrayMarker = "TArray<"_sv;
	if (typeName.StartsWith(arrayMarker) && typeName.EndsWith(">"_sv))
	{
		outElementTypeName = typeName.Substring(arrayMarker.Length(), typeName.Length() - arrayMarker.Length() - 1);
		return true;
	}
	return false;
}

bool FParsedStructInfo::IsObjectBasedName(const FStringView typeName)
{
	if (IsObjectBasedTypeName(typeName))
	{
		return true;
	}

	FStringView arrayElementTypeName;
	if (FindArrayElementTypeName(typeName, arrayElementTypeName))
	{
		return IsObjectBasedTypeName(arrayElementTypeName);
	}

	return false;
}

bool FParsedStructInfo::IsObjectClass() const
{
	return IsObjectBasedType(*this);
}