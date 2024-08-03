#pragma once

#include "Engine/Error.h"
#include "Lox/SourceLocation.h"

/**
 * @brief Defines a Lox error.
 */
using FLoxError = TError<FLoxSourceLocation>;

/**
 * Defines a Lox error or value.
 */
template<typename ValueType>
using TLoxErrorOr = TErrorOr<ValueType, FLoxSourceLocation>;

/**
 * @brief Defines a macro to make a formatted error that includes a token's source location.
 */
#define MAKE_LOX_ERROR(Token, Message, ...) FLoxError::Format(Token.SourceLocation, UM_JOIN(Message, _sv), ##__VA_ARGS__)