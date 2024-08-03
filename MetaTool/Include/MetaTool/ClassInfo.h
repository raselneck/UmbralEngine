#pragma once

#include "MetaTool/FunctionInfo.h"
#include "MetaTool/StructInfo.h"

/**
 * @brief Defines information about a parsed class.
 */
class FParsedClassInfo : public FParsedStructInfo
{
public:

	/** @brief The class's functions. */
	TArray<FFunctionInfo> Functions;

	/**
	 * @brief Destroys this parsed class info.
	 */
	virtual ~FParsedClassInfo() override = default;
};