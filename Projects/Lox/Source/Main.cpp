#include "Engine/Cast.h"
#include "Engine/CommandLine.h"
#include "Engine/Console.h"
#include "HAL/File.h"
#include "HAL/Path.h"
#include "HAL/Timer.h"
#include "Lox/Scanner.h"
#include "Lox/Parser.h"
#include "Main/Main.h"
#include "Templates/VariadicTraits.h"
#include <cstdio>

class FLoxAstPrinter final : public TLoxExpressionVisitor<FString>
{
public:

	FLoxAstPrinter() = default;
	virtual ~FLoxAstPrinter() override = default;

	/**
	 * @brief Prints an AST for an expression to a string.
	 *
	 * @param expression The expression.
	 * @return The AST as a string.
	 */
	[[nodiscard]] FString PrintToString(FLoxExpression& expression)
	{
		return expression.AcceptVisitor(*this);
	}

private:

	/**
	 * @brief Surrounds the given expressions with a name and parentheses.
	 *
	 * @param name The name of the parenthesized group.
	 * @param expressions The expressions.
	 * @return The "parenthesized" result.
	 */
	template<typename... ExpressionTypes>
	FString Parenthesize(const FStringView name, ExpressionTypes& ... expressions)
	{
		FStringBuilder builder;
		builder.Append("("_sv).Append(name);
		TVariadicForEach<ExpressionTypes...>::Visit([&, this](auto expression)
		{
			builder.Append(" "_sv);

			const FString expressionAsString = expression->AcceptVisitor(*this);
			builder.Append(expressionAsString);

			return EIterationDecision::Continue;
		}, Forward<ExpressionTypes>(expressions)...);

		builder.Append(")"_sv);
		return builder.ReleaseString();
	}

	/** @copydoc TLoxExpressionVisitor::VisitBinaryExpression */
	virtual FString VisitBinaryExpression(FLoxBinaryExpression& expression) override
	{
		return Parenthesize(expression.Operator.Text, expression.Left, expression.Right);
	}

	/** @copydoc TLoxExpressionVisitor::VisitGroupedExpression */
	virtual FString VisitGroupedExpression(FLoxGroupedExpression& expression) override
	{
		return Parenthesize("group"_sv, expression.Inner);
	}

	/** @copydoc TLoxExpressionVisitor::VisitLiteralExpression */
	virtual FString VisitLiteralExpression(FLoxLiteralExpression& expression) override
	{
		if (expression.Literal.Value.IsNull())
		{
			return "null"_s;
		}

		return expression.Literal.Value.AsString();
	}

	/** @copydoc TLoxExpressionVisitor::VisitTernaryExpression */
	virtual FString VisitTernaryExpression(FLoxTernaryExpression& expression) override
	{
		return Parenthesize("?:"_sv, expression.Condition, expression.TrueExpression, expression.FalseExpression);
	}

	/** @copydoc TLoxExpressionVisitor::VisitUnaryExpression */
	virtual FString VisitUnaryExpression(FLoxUnaryExpression& expression) override
	{
		return Parenthesize(expression.Operator.Text, expression.Right);
	}
};

/**
 * @brief Defines an evaluator for Lox expressions.
 */
class FLoxExpressionEvaluator final : public TLoxExpressionVisitor<TLoxErrorOr<FLoxValue>>
{
public:

	FLoxExpressionEvaluator() = default;
	virtual ~FLoxExpressionEvaluator() override = default;

	/**
	 * @brief Evaluates the given expression.
	 *
	 * @param expression The expression.
	 * @return The result of the expression.
	 */
	[[nodiscard]] TLoxErrorOr<FLoxValue> Evaluate(FLoxExpression& expression)
	{
		return expression.AcceptVisitor(*this);
	}

private:

	/** @copydoc TLoxExpressionVisitor::VisitBinaryExpression */
	virtual TLoxErrorOr<FLoxValue> VisitBinaryExpression(FLoxBinaryExpression& expression) override
	{
		TLoxErrorOr<FLoxValue> leftValue = expression.Left->AcceptVisitor(*this);
		if (leftValue.IsError())
		{
			return leftValue;
		}

		TLoxErrorOr<FLoxValue> rightValue = expression.Right->AcceptVisitor(*this);
		if (rightValue.IsError())
		{
			return rightValue;
		}

		switch (expression.Operator.Type)
		{
		case ELoxTokenType::Plus:
		{
			TErrorOr<FLoxValue> result = FLoxValue::Add(leftValue.GetValue(), rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return result.GetValue();
		}

		case ELoxTokenType::Minus:
		{
			TErrorOr<FLoxValue> result = FLoxValue::Subtract(leftValue.GetValue(), rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return result.GetValue();
		}

		case ELoxTokenType::Asterisk:
		{
			TErrorOr<FLoxValue> result = FLoxValue::Multiply(leftValue.GetValue(), rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return result.GetValue();
		}

		case ELoxTokenType::Slash:
		{
			TErrorOr<FLoxValue> result = FLoxValue::Divide(leftValue.GetValue(), rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return result.GetValue();
		}

		case ELoxTokenType::Caret:
		{
			// TODO FLoxValue::Pow
			return MAKE_LOX_ERROR(expression.Operator, "Power operator (\"^\") not yet implemented");
		}

		case ELoxTokenType::Greater:
		{
			TErrorOr<ECompareResult> result = FLoxValue::Compare(leftValue.GetValue(), rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return FLoxValue::FromBool(result.GetValue() == ECompareResult::GreaterThan);
		}

		case ELoxTokenType::GreaterEqual:
		{
			TErrorOr<ECompareResult> result = FLoxValue::Compare(leftValue.GetValue(), rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return FLoxValue::FromBool(result.GetValue() != ECompareResult::LessThan);
		}

		case ELoxTokenType::Less:
		{
			TErrorOr<ECompareResult> result = FLoxValue::Compare(leftValue.GetValue(), rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return FLoxValue::FromBool(result.GetValue() == ECompareResult::LessThan);
		}

		case ELoxTokenType::LessEqual:
		{
			TErrorOr<ECompareResult> result = FLoxValue::Compare(leftValue.GetValue(), rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return FLoxValue::FromBool(result.GetValue() != ECompareResult::GreaterThan);
		}

		case ELoxTokenType::EqualEqual:
		{
			TErrorOr<ECompareResult> result = FLoxValue::Compare(leftValue.GetValue(), rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return FLoxValue::FromBool(result.GetValue() == ECompareResult::Equals);
		}

		case ELoxTokenType::BangEqual:
		{
			TErrorOr<ECompareResult> result = FLoxValue::Compare(leftValue.GetValue(), rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return FLoxValue::FromBool(result.GetValue() != ECompareResult::Equals);
		}

		default:
			return MAKE_LOX_ERROR(expression.Operator, "Invalid binary operator \"{}\"", expression.Operator.Text);
		}
	}

	/** @copydoc TLoxExpressionVisitor::VisitGroupedExpression */
	virtual TLoxErrorOr<FLoxValue> VisitGroupedExpression(FLoxGroupedExpression& expression) override
	{
		return expression.Inner->AcceptVisitor(*this);
	}

	/** @copydoc TLoxExpressionVisitor::VisitLiteralExpression */
	virtual TLoxErrorOr<FLoxValue> VisitLiteralExpression(FLoxLiteralExpression& expression) override
	{
		return expression.Literal.Value;
	}

	/** @copydoc TLoxExpressionVisitor::VisitTernaryExpression */
	virtual TLoxErrorOr<FLoxValue> VisitTernaryExpression(FLoxTernaryExpression& expression) override
	{
		TLoxErrorOr<FLoxValue> condition = expression.Condition->AcceptVisitor(*this);
		if (condition.IsError())
		{
			return condition;
		}

		if (condition.GetValue().AsBool())
		{
			return expression.TrueExpression->AcceptVisitor(*this);
		}
		return expression.FalseExpression->AcceptVisitor(*this);
	}

	/** @copydoc TLoxExpressionVisitor::VisitUnaryExpression */
	virtual TLoxErrorOr<FLoxValue> VisitUnaryExpression(FLoxUnaryExpression& expression) override
	{
		TLoxErrorOr<FLoxValue> rightValue = expression.Right->AcceptVisitor(*this);
		if (rightValue.IsError())
		{
			return rightValue;
		}

		switch (expression.Operator.Type)
		{
		case ELoxTokenType::Minus:
		{
			TErrorOr<FLoxValue> result = FLoxValue::Negate(rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return result.ReleaseValue();
		}

		case ELoxTokenType::Bang:
		{
			TErrorOr<FLoxValue> result = FLoxValue::LogicalNot(rightValue.GetValue());
			if (result.IsError())
			{
				return FLoxError { expression.Operator.SourceLocation, result.GetError().GetMessage() };
			}

			return result.ReleaseValue();
		}

		default:
			return MAKE_LOX_ERROR(expression.Operator, "Invalid unary operator \"{}\"", expression.Operator.Text);
		}
	}
};

/**
 * @brief Runs the given Lox code.
 *
 * @param code The code.
 */
static void RunLoxCode(const FString& code)
{
	FTimer timer;

	timer.Restart();
		FLoxScanner scanner;
		scanner.ScanTextForTokens(code);
	const FTimeSpan scanDuration = timer.Stop();

	if (scanner.HasErrors())
	{
		FScopedConsoleForegroundColor redForeground { EConsoleColor::Red };
		for (const FLoxError& error : scanner.GetErrors())
		{
			FConsole::WriteLine("{} {}"_sv, error.GetSourceLocation(), error.GetMessage());
		}
		return;
	}

	timer.Restart();
		FLoxParser parser;
		parser.ParseTokens(scanner.GetTokens());
	const FTimeSpan parseDuration = timer.Stop();

	if (parser.HasErrors())
	{
		FScopedConsoleForegroundColor redForeground { EConsoleColor::Red };
		for (const FLoxError& error : parser.GetErrors())
		{
			FConsole::WriteLine("{} {}"_sv, error.GetSourceLocation(), error.GetMessage());
		}
		return;
	}

	for (const TUniquePtr<FLoxStatement>& statement : parser.GetStatements())
	{
		if (const FLoxExpressionStatement* expressionStatement = Cast<FLoxExpressionStatement>(statement.Get()))
		{
#if 0
			FLoxAstPrinter printer;
			FString expressionAst = printer.PrintToString(const_cast<FLoxExpression&>(*expressionStatement->Expression));
			UM_LOG(Info, "{}", expressionAst);
#endif
#if 1
			timer.Restart();

			FLoxExpressionEvaluator evaluator;
			TLoxErrorOr<FLoxValue> result = evaluator.Evaluate(const_cast<FLoxExpression&>(*expressionStatement->Expression));
			if (result.IsError())
			{
				FScopedConsoleForegroundColor redForeground { EConsoleColor::Red };
				FConsole::WriteLine("{}"_sv, result.GetError());
			}
			else
			{
				FConsole::WriteLine("{}"_sv, result.GetValue().AsString());
			}

			const FTimeSpan evalDuration = timer.Stop();
			FConsole::WriteLine("Evaluation took {} microseconds"_sv, evalDuration.GetTotalMilliseconds() * 1000.0);
#endif
		}
	}

	FConsole::WriteLine("Tokenization took {} microseconds"_sv, scanDuration.GetTotalMilliseconds() * 1000.0);
	FConsole::WriteLine("Parsing took {} microseconds"_sv, parseDuration.GetTotalMilliseconds() * 1000.0);
}

/**
 * @brief Runs a Lox script file.
 *
 * @param filePath The path to the file.
 */
static void RunFile(const FStringView filePath)
{
	const TErrorOr<FString> fileContent = FFile::ReadAllText(filePath);
	if (fileContent.IsError())
	{
		FScopedConsoleForegroundColor redForeground { EConsoleColor::Red };
		FConsole::WriteLine("{}"_sv, fileContent.GetError().GetMessage());
	}
	else
	{
		RunLoxCode(fileContent.GetValue());
	}
}

/**
 * @brief Runs the REPL prompt.
 */
static void RunPrompt()
{
	while (true)
	{
		FConsole::Write(">> "_sv);

		const FString input = FConsole::ReadLine();
		if (input == "quit"_sv)
		{
			break;
		}

		RunLoxCode(input);
	}
}

int32 UmbralMain()
{
	switch (FCommandLine::GetArgc())
	{
	case 1:
		RunPrompt();
		break;

	case 2:
		RunFile(FCommandLine::GetArgument(1));
		break;

	default:
	{
		FScopedConsoleForegroundColor redForeground { EConsoleColor::Red };

		const FStringView exePath = FCommandLine::GetArgument(0);
		const FStringView exeName = FPath::GetBaseFileName(exePath);
		FConsole::WriteLine("Usage: {} [script]"_sv, exeName);
		break;
	}
	}

	return 0;
}