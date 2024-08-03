#pragma once

#include "Containers/StringView.h"

namespace MacroNames
{
	constexpr FStringView ClassSpecifier           = "UM_CLASS"_sv;
	constexpr FStringView EnumSpecifier            = "UM_ENUM"_sv;
	constexpr FStringView FunctionSpecifier        = "UM_FUNCTION"_sv;
	constexpr FStringView GeneratedBodySpecifier   = "UM_GENERATED_BODY"_sv;
	constexpr FStringView PropertySpecifier        = "UM_PROPERTY"_sv;
	constexpr FStringView StructSpecifier          = "UM_STRUCT"_sv;
}