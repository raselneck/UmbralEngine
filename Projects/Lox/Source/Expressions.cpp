#include "Engine/Assert.h"
#include "Lox/Expressions.h"

FLoxBinaryExpression::FLoxBinaryExpression(TUniquePtr<FLoxExpression> left, FLoxToken op, TUniquePtr<FLoxExpression> right)
	: Operator { MoveTemp(op) }
	, Left { MoveTemp(left) }
	, Right { MoveTemp(right) }
{
}

void FLoxBinaryExpression::AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam)
{
	return visitor.VisitBinaryExpression(*this, userParam);
}

FLoxValue FLoxBinaryExpression::Evaluate(FLoxScope& scope) const
{
	(void)scope;
	return {};
}

FLoxGroupedExpression::FLoxGroupedExpression(TUniquePtr<FLoxExpression> inner)
	: Inner { MoveTemp(inner) }
{
}

void FLoxGroupedExpression::AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam)
{
	return visitor.VisitGroupedExpression(*this, userParam);
}

FLoxValue FLoxGroupedExpression::Evaluate(FLoxScope& scope) const
{
	UM_ASSERT(Inner.IsValid(), "Inner expression not set");

	return Inner->Evaluate(scope);
}

FLoxLiteralExpression::FLoxLiteralExpression(FLoxToken literal)
	: Literal { MoveTemp(literal) }
{
}

void FLoxLiteralExpression::AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam)
{
	return visitor.VisitLiteralExpression(*this, userParam);
}

FLoxValue FLoxLiteralExpression::Evaluate(FLoxScope& scope) const
{
	(void)scope;

	return Literal.Value;
}

FLoxTernaryExpression::FLoxTernaryExpression(TUniquePtr<FLoxExpression> condition,
                                             TUniquePtr<FLoxExpression> trueExpression,
                                             TUniquePtr<FLoxExpression> falseExpression)
	: Condition { MoveTemp(condition) }
	, TrueExpression { MoveTemp(trueExpression) }
	, FalseExpression { MoveTemp(falseExpression) }
{
}

void FLoxTernaryExpression::AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam)
{
	visitor.VisitTernaryExpression(*this, userParam);
}

FLoxValue FLoxTernaryExpression::Evaluate(FLoxScope& scope) const
{
	(void)scope;
	return {};
}

FLoxUnaryExpression::FLoxUnaryExpression(FLoxToken op, TUniquePtr<FLoxExpression> right)
	: Operator { MoveTemp(op) }
	, Right { MoveTemp(right) }
{
}

void FLoxUnaryExpression::AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam)
{
	return visitor.VisitUnaryExpression(*this, userParam);
}

FLoxValue FLoxUnaryExpression::Evaluate(FLoxScope& scope) const
{
	(void)scope;
	return {};
}