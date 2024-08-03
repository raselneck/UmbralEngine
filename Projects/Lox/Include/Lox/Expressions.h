#pragma once

#include "Containers/Any.h"
#include "Lox/Scope.h"
#include "Lox/Token.h"
#include "Memory/UniquePtr.h"
#include "Templates/AndNotOr.h"
#include "Templates/IsSame.h"

class ILoxExpressionVisitor;

template<typename ReturnType>
class TLoxExpressionVisitor;

/**
 * @brief Defines the base for all Lox expressions.
 */
class FLoxExpression
{
public:

	/**
	 * @brief Destroys this Lox expression.
	 */
	virtual ~FLoxExpression() = default;

	/**
	 * @brief Accepts an expression visitor.
	 *
	 * @param visitor The visitor.
	 * @param userParam A user-defined context object to supply to the visitor.
	 */
	virtual void AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam) = 0;

	/**
	 * @brief Accepts an expression visitor.
	 *
	 * @param visitor The visitor.
	 */
	template<typename ReturnType>
	[[nodiscard]] ReturnType AcceptVisitor(TLoxExpressionVisitor<ReturnType>& visitor);

	/**
	 * @brief Evaluates this expression.
	 *
	 * @param scope The current evaluation scope.
	 * @return The result of the evaluation.
	 */
	[[nodiscard]] virtual FLoxValue Evaluate(FLoxScope& scope) const = 0;

protected:

	/**
	 * @brief Sets default values for this expression's properties.
	 */
	FLoxExpression() = default;
};

/**
 * @brief Defines a binary expression.
 */
class FLoxBinaryExpression : public FLoxExpression
{
public:

	/** @brief The expression operator. */
	FLoxToken Operator;

	/** @brief The left-hand side of the binary expression. */
	TUniquePtr<FLoxExpression> Left;

	/** @brief The right-hand side of the binary expression. */
	TUniquePtr<FLoxExpression> Right;

	/**
	 * @brief Sets default values for this binary expression.
	 *
	 * @param left The left expression.
	 * @param op The token representing the operator.
	 * @param right The right expression.
	 */
	FLoxBinaryExpression(TUniquePtr<FLoxExpression> left, FLoxToken op, TUniquePtr<FLoxExpression> right);

	/** @copydoc FLoxExpression::AcceptVisitor */
	virtual void AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam) override;

	/** @copydoc FLoxExpression::Evaluate */
	virtual FLoxValue Evaluate(FLoxScope& scope) const override;
};

/**
 * @brief Defines a grouped expression.
 */
class FLoxGroupedExpression : public FLoxExpression
{
public:

	/** @brief The inner expression. */
	TUniquePtr<FLoxExpression> Inner;

	/**
	 * @brief Sets default values for this grouper expression.
	 *
	 * @param inner The inner expression.
	 */
	explicit FLoxGroupedExpression(TUniquePtr<FLoxExpression> inner);

	/** @copydoc FLoxExpression::AcceptVisitor */
	virtual void AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam) override;

	/** @copydoc FLoxExpression::Evaluate */
	virtual FLoxValue Evaluate(FLoxScope& scope) const override;
};

/**
 * @brief Defines a grouped expression.
 */
class FLoxLiteralExpression : public FLoxExpression
{
public:

	/** @brief The literal token. */
	FLoxToken Literal;

	/**
	 * @brief Sets default values for this literal expression.
	 *
	 * @param literal The token representing the literal value.
	 */
	explicit FLoxLiteralExpression(FLoxToken literal);

	/** @copydoc FLoxExpression::AcceptVisitor */
	virtual void AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam) override;

	/** @copydoc FLoxExpression::Evaluate */
	virtual FLoxValue Evaluate(FLoxScope& scope) const override;
};

/**
 * @brief Defines a ternary expression.
 */
class FLoxTernaryExpression : public FLoxExpression
{
public:

	/** @brief The expression representing the ternary condition. */
	TUniquePtr<FLoxExpression> Condition;

	/** @brief The expression to be evaluated if Condition is true. */
	TUniquePtr<FLoxExpression> TrueExpression;

	/** @brief The expression to be evaluated if Condition is false. */
	TUniquePtr<FLoxExpression> FalseExpression;

	/**
	 * @brief Sets default values for this ternary expression's properties.
	 *
	 * @param condition The condition.
	 * @param trueExpression The true expression.
	 * @param falseExpression The false expression.
	 */
	FLoxTernaryExpression(TUniquePtr<FLoxExpression> condition, TUniquePtr<FLoxExpression> trueExpression, TUniquePtr<FLoxExpression> falseExpression);

	/** @copydoc FLoxExpression::AcceptVisitor */
	virtual void AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam) override;

	/** @copydoc FLoxExpression::Evaluate */
	virtual FLoxValue Evaluate(FLoxScope& scope) const override;
};

/**
 * @brief Defines a unary expression.
 */
class FLoxUnaryExpression : public FLoxExpression
{
public:

	/** @brief The expression operator. */
	FLoxToken Operator;

	/** @brief The right-hand side of the binary expression. */
	TUniquePtr<FLoxExpression> Right;

	/**
	 * @brief Sets default values for this unary expression's properties.
	 *
	 * @param op The operator token.
	 * @param right The right side of the expression.
	 */
	FLoxUnaryExpression(FLoxToken op, TUniquePtr<FLoxExpression> right);

	/** @copydoc FLoxExpression::AcceptVisitor */
	virtual void AcceptVisitor(ILoxExpressionVisitor& visitor, void* userParam) override;

	/** @copydoc FLoxExpression::Evaluate */
	virtual FLoxValue Evaluate(FLoxScope& scope) const override;
};

/**
 * @brief Defines the base for all Lox expression visitors.
 *
 * @tparam ReturnType The return type for each visit call.
 */
class ILoxExpressionVisitor
{
public:

	/**
	 * @brief Destroys this expression visitor.
	 */
	virtual ~ILoxExpressionVisitor() = default;

	/**
	 * @brief Visits a Lox binary expression.
	 *
	 * @param expression The binary expression.
	 * @param userParam The user parameter.
	 */
	virtual void VisitBinaryExpression(FLoxBinaryExpression& expression, void* userParam) = 0;

	/**
	 * @brief Visits a Lox grouped expression.
	 *
	 * @param expression The grouped expression.
	 * @param userParam The user parameter.
	 */
	virtual void VisitGroupedExpression(FLoxGroupedExpression& expression, void* userParam) = 0;

	/**
	 * @brief Visits a Lox literal expression.
	 *
	 * @param expression The literal expression.
	 * @param userParam The user parameter.
	 */
	virtual void VisitLiteralExpression(FLoxLiteralExpression& expression, void* userParam) = 0;

	/**
	 * @brief Visits a Lox ternary expression.
	 *
	 * @param expression The ternary expression.
	 * @param userParam The user parameter.
	 */
	virtual void VisitTernaryExpression(FLoxTernaryExpression& expression, void* userParam) = 0;

	/**
	 * @brief Visits a Lox unary expression.
	 *
	 * @param expression The unary expression.
	 * @param userParam The user parameter.
	 */
	virtual void VisitUnaryExpression(FLoxUnaryExpression& expression, void* userParam) = 0;

protected:

	/**
	 * @brief Sets default values for this visitor's properties.
	 */
	ILoxExpressionVisitor() = default;
};

/**
 * @brief Defines a Lox expression visitor that can return any type when visiting expressions.
 *
 * @tparam ReturnType The visit return type.
 */
template<typename ReturnType>
class TLoxExpressionVisitor : public ILoxExpressionVisitor
{
public:

	/**
	 * @brief Destroys this Lox expression visitor.
	 */
	virtual ~TLoxExpressionVisitor() = default;

	/**
	 * @brief Visits a Lox binary expression.
	 *
	 * @param expression The binary expression.
	 */
	[[nodiscard]] virtual ReturnType VisitBinaryExpression(FLoxBinaryExpression& expression) = 0;

	/**
	 * @brief Visits a Lox grouped expression.
	 *
	 * @param expression The grouped expression.
	 */
	[[nodiscard]] virtual ReturnType VisitGroupedExpression(FLoxGroupedExpression& expression) = 0;

	/**
	 * @brief Visits a Lox literal expression.
	 *
	 * @param expression The literal expression.
	 */
	[[nodiscard]] virtual ReturnType VisitLiteralExpression(FLoxLiteralExpression& expression) = 0;

	/**
	 * @brief Visits a Lox ternary expression.
	 *
	 * @param expression The ternary expression.
	 */
	[[nodiscard]] virtual ReturnType VisitTernaryExpression(FLoxTernaryExpression& expression) = 0;

	/**
	 * @brief Visits a Lox unary expression.
	 *
	 * @param expression The unary expression.
	 */
	[[nodiscard]] virtual ReturnType VisitUnaryExpression(FLoxUnaryExpression& expression) = 0;

protected:

	/**
	 * @brief Sets default values for this expression visitor's properties.
	 */
	TLoxExpressionVisitor() = default;

private:

	/** @copydoc ILoxExpressionVisitor::VisitBinaryExpression */
	virtual void VisitBinaryExpression(FLoxBinaryExpression& expression, void* userParam) override final
	{
		*reinterpret_cast<ReturnType*>(userParam) = VisitBinaryExpression(expression);
	}

	/** @copydoc ILoxExpressionVisitor::VisitGroupedExpression */
	virtual void VisitGroupedExpression(FLoxGroupedExpression& expression, void* userParam) override final
	{
		*reinterpret_cast<ReturnType*>(userParam) = VisitGroupedExpression(expression);
	}

	/** @copydoc ILoxExpressionVisitor::VisitLiteralExpression */
	virtual void VisitLiteralExpression(FLoxLiteralExpression& expression, void* userParam) override final
	{
		*reinterpret_cast<ReturnType*>(userParam) = VisitLiteralExpression(expression);
	}

	/** @copydoc ILoxExpressionVisitor::VisitTernaryExpression */
	virtual void VisitTernaryExpression(FLoxTernaryExpression& expression, void* userParam) override final
	{
		*reinterpret_cast<ReturnType*>(userParam) = VisitTernaryExpression(expression);
	}

	/** @copydoc ILoxExpressionVisitor::VisitUnaryExpression */
	virtual void VisitUnaryExpression(FLoxUnaryExpression& expression, void* userParam) override final
	{
		*reinterpret_cast<ReturnType*>(userParam) = VisitUnaryExpression(expression);
	}
};

template<typename ReturnType>
ReturnType FLoxExpression::AcceptVisitor(TLoxExpressionVisitor<ReturnType>& visitor)
{
	ReturnType result;
	AcceptVisitor(visitor, &result);
	return result;
}