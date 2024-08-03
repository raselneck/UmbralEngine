#include "Lox/Statements.h"

FLoxExpressionStatement::FLoxExpressionStatement(TUniquePtr<FLoxExpression> expression)
	: Expression { MoveTemp(expression) }
{
}

TLoxErrorOr<void> FLoxExpressionStatement::Execute(FLoxScope& scope) const
{
	(void)scope;
	return {};
}