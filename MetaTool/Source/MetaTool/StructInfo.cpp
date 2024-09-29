#include "Engine/Logging.h"
#include "MetaTool/ClassInfo.h"
#include "MetaTool/StructInfo.h"
#include "Parsing/Scanner.h"

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

static bool IsObjectOrActorName(const FStringView typeName)
{
	return typeName.StartsWith('U') || // Not foolproof
	       typeName.StartsWith('A') || // Not foolproof
	       typeName == "TObjectPtr"_sv ||
	       typeName == "TWeakObjectPtr"_sv ||
	       typeName == "FObjectPtr"_sv ||
	       typeName == "FWeakObjectPtr"_sv;
}

static inline bool IsObjectOrActorName(const FToken& token)
{
	return IsObjectOrActorName(token.Text);
}

static bool IsObjectBasedTypeName(const TSpan<const FToken> tokens)
{
	if (IsObjectOrActorName(tokens[0]))
	{
		return true;
	}

	if (tokens[0].Text == "TArray"_sv)
	{
		// Ill-formed TArray property
		if (tokens[1].Text != "<"_sv || tokens[tokens.Num() - 1].Text != ">"_sv)
		{
			return false;
		}

		return IsObjectBasedTypeName(tokens.Slice(2, tokens.Num() - 3));
	}

	return false;
}

bool FParsedStructInfo::IsObjectBasedName(const FStringView typeName)
{
	FScanner scanner;
	scanner.ScanTextForTokens(typeName);

	const TSpan<const FToken> tokens = scanner.GetTokens();
	if (tokens.IsEmpty())
	{
		return false;
	}

	if (scanner.HasErrors())
	{
		UM_LOG(Error, "Failed to parse type name \"{}\". Reason(s):", typeName);
		for (const FParseError& error : scanner.GetErrors())
		{
			UM_LOG(Error, "  {} {}", error.GetSourceLocation(), error.GetMessage());
		}
		return false;
	}

	return IsObjectBasedTypeName(scanner.GetTokens());
}

bool FParsedStructInfo::IsObjectClass() const
{
	return IsObjectBasedType(*this);
}