#pragma once

#include "Lox/Error.h"
#include "Lox/Expressions.h"
#include "Lox/Scope.h"

/**
 * @brief Defines the base for all Lox statements.
 */
class FLoxStatement
{
public:

	/**
	 * @brief Destroys this statement.
	 */
	virtual ~FLoxStatement() = default;

	/**
	 * @brief Executes this statement.
	 *
	 * @param scope The scope to execute this statement in.
	 */
	virtual TLoxErrorOr<void> Execute(FLoxScope& scope) const = 0;

protected:

	/**
	 * @brief Sets default values for this statement's properties.
	 */
	FLoxStatement() = default;
};

/**
 * @brief Defines an expression statement.
 */
class FLoxExpressionStatement : public FLoxStatement
{
public:

	/** @brief This statement's expression. */
	TUniquePtr<FLoxExpression> Expression;

	/**
	 * @brief Sets default values for this statement's properties.
	 *
	 * @param expression The expression.
	 */
	explicit FLoxExpressionStatement(TUniquePtr<FLoxExpression> expression);

	/** @copydoc FLoxStatement::Execute */
	virtual TLoxErrorOr<void> Execute(FLoxScope& scope) const override;
};