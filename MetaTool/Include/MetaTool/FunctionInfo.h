#pragma once

#include "Containers/Array.h"
#include "Containers/StringView.h"

/**
 * @brief Defines information about a function's parameter.
 */
class FFunctionParamInfo
{
public:

	/** @brief The parameter's name. */
	FStringView ParamName;

	/** @brief The parameter's type. */
	FStringView ParamType;
};

/**
 * @brief Defines an enumeration of flags for a function.
 */
namespace EFunctionFlags
{
	enum Type
	{
		None    = 0,
		Virtual = 1 << 0,
		Const   = 1 << 1
	};
}

/**
 * @brief Defines information about a function.
 */
class FFunctionInfo
{
public:

	/** @brief The function's name. */
	FStringView FunctionName;

	/** @brief The function's return type. */
	FStringView FunctionReturnType;

	/** @brief The function's attributes. */
	TArray<FStringView> FunctionAttributes;

	/** @brief The function's flags. */
	EFunctionFlags::Type FunctionFlags = EFunctionFlags::None;
};