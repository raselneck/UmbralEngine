#pragma once

#include "Containers/HashMap.h"
#include "Containers/StringView.h"
#include "Lox/Value.h"

/**
 * @brief Defines a scope for variables in Lox.
 */
class FLoxScope final
{
public:

private:

	THashMap<FString, FLoxValue> m_Variables;
};