#pragma once

#include "Containers/Optional.h"
#include "Containers/Variant.h"
#include "Containers/String.h"
#include "Memory/SharedPtr.h"
#include "Misc/SourceLocation.h"
#include "Misc/StringBuilder.h"
#include "Templates/IsSame.h"

/**
 * @brief Attempts to get the result of an expression that could return an error to a given declaration.
 *
 * @remarks If the expression results in an error, this macro will attempt to return that error.
 *
 * @param Decl The declaration for the value.
 * @param Expression The expression to evaluate.
 */
#define TRY_EVAL(Decl, Expression)                      \
	auto ANONYMOUS_VAR = (Expression);                  \
	if (ANONYMOUS_VAR.IsError())                        \
		return ANONYMOUS_VAR.ReleaseError();            \
	Decl = ANONYMOUS_VAR.ReleaseValue()

/**
 * @brief Attempts to evaluate the given expression that could return an error.
 *
 * @remarks If the expression results in an error, this macro will attempt to return that error.
 *
 * @param Expression The expression to evaluate.
 */
#define TRY_DO(Expression)                              \
	auto ANONYMOUS_VAR = (Expression);                  \
	static_assert(IsSame<decltype(ANONYMOUS_VAR), TErrorOr<void>>, "Potentially throwing away return value"); \
	if (ANONYMOUS_VAR.IsError())                        \
		return ANONYMOUS_VAR.ReleaseError()

/**
 * @brief Defines an error.
 *
 * @tparam SourceLocationType The type to use for source locations.
 */
template<typename SourceLocationType>
class TError
{
public:

	/**
	 * @brief Creates a new error.
	 *
	 * @param location The source location.
	 * @param message The message.
	 */
	TError(SourceLocationType location, const FString& message)
		: m_Message { message }
		, m_Location { MoveTemp(location) }
	{
	}

	/**
	 * @brief Creates a new error.
	 *
	 * @param location The source location.
	 * @param message The message.
	 */
	TError(SourceLocationType location, FString&& message)
		: m_Message { MoveTemp(message) }
		, m_Location { MoveTemp(location) }
	{
	}

	/**
	 * @brief Creates a new error.
	 *
	 * @param location The source location.
	 * @param message The message.
	 */
	TError(SourceLocationType location, const FStringView message)
		: m_Message { message }
		, m_Location { MoveTemp(location) }
	{
	}

	/**
	 * @brief Creates a formatted error.
	 *
	 * @tparam ArgTypes The argument types.
	 * @param location The source location.
	 * @param messageFormat The error message format.
	 * @param messageArgs The error message arguments.
	 * @return The formatted error.
	 */
	template<typename... ArgTypes>
	static TError Format(SourceLocationType location, const FStringView messageFormat, ArgTypes&&... messageArgs)
	{
		if constexpr (sizeof...(messageArgs) == 0)
		{
			return TError { MoveTemp(location), messageFormat };
		}
		else
		{
			return TError { MoveTemp(location), FString::Format(messageFormat, Forward<ArgTypes>(messageArgs)...) };
		}
	}

	/**
	 * @brief Constructs this error's inner error in place.
	 *
	 * @tparam ArgTypes
	 * @param location The error's source location.
	 * @param messageFormat The error message.
	 * @param messageArgs The format arguments for the error message.
	 * @return The inner error.
	 */
	template<typename... ArgTypes>
	[[maybe_unused]] TError& EmplaceInnerError(SourceLocationType location, const FStringView messageFormat, ArgTypes&&... messageArgs)
	{
		SetInnerError(Format(location, messageFormat, Forward<ArgTypes>(messageArgs)...));
		return GetInnerError();
	}

	/**
	 * @brief Gets the inner error.
	 *
	 * @return The inner error, or null if there is none.
	 */
	[[nodiscard]] const TError* GetInnerError() const
	{
		return m_InnerError.Get();
	}

	/**
	 * @brief Gets the inner error.
	 *
	 * @return The inner error, or null if there is none.
	 */
	[[nodiscard]] TError* GetInnerError()
	{
		return m_InnerError.Get();
	}

	/**
	 * @brief Gets this error's message.
	 *
	 * @return This error's message.
	 */
	[[nodiscard]] FStringView GetMessage() const
	{
		return m_Message.AsStringView();
	}

	/**
	 * @brief Gets this error's source location.
	 *
	 * @return This error's source location.
	 */
	[[nodiscard]] SourceLocationType GetSourceLocation() const
	{
		return m_Location;
	}

	/**
	 * @brief Checks to see if this error has an inner error.
	 *
	 * @return True if this error has an inner error, otherwise false.
	 */
	[[nodiscard]] bool HasInnerError() const
	{
		return m_InnerError.IsValid();
	}

	/**
	 * @brief Sets the inner error.
	 *
	 * @param innerError The inner error.
	 */
	void SetInnerError(const TError& innerError)
	{
		m_InnerError = MakeShared<TError>(innerError);
	}

	/**
	 * @brief Sets the inner error.
	 *
	 * @param innerError The inner error.
	 */
	void SetInnerError(TError&& innerError)
	{
		m_InnerError = MakeShared<TError>(MoveTemp(innerError));
	}

private:

	// TODO(FIXME) It's odd that we're using a shared pointer for the inner error, but doing so allows errors to be copied. May be a better solution?
	TSharedPtr<TError> m_InnerError;
	FString m_Message;
	SourceLocationType m_Location;
};

/**
 * @brief Defines an error.
 */
using FError = TError<FSourceLocation>;

/**
 * @brief Defines a string formatter for errors.
 *
 * @tparam SourceLocationType The source location type.
 */
template<typename SourceLocationType>
struct TFormatter<TError<SourceLocationType>>
{
	using ErrorType = TError<SourceLocationType>;

	/**
	 * @brief Builds the string for an error.
	 *
	 * @param value The error.
	 * @param builder The string builder.
	 */
	void BuildString(const ErrorType& value, FStringBuilder& builder) const
	{
		TFormatter<SourceLocationType> sourceLocationFormatter;
		sourceLocationFormatter.BuildString(value.GetSourceLocation(), builder);

		builder.Append(" "_sv);
		builder.Append(value.GetMessage());

		for (const ErrorType* innerError = value.GetInnerError(); innerError != nullptr; innerError = innerError->GetInnerError())
		{
			builder.Append("\n\t> "_sv);

			sourceLocationFormatter.BuildString(innerError->GetSourceLocation(), builder);

			builder.Append(" "_sv);
			builder.Append(innerError->GetMessage());
		}
	}

	/**
	 * @brief Attempts to parse a format string.
	 *
	 * @param formatString The format string.
	 * @return True if the format string was parsed, otherwise false.
	 */
	[[nodiscard]] bool Parse(const FStringView formatString) const
	{
		return formatString.IsEmpty();
	}
};

/**
 * @brief Defines a macro to make a formatted error that includes the current source location.
 */
#define MAKE_ERROR(Message, ...) FError::Format(UMBRAL_SOURCE_LOCATION, UM_JOIN(Message, _sv), ##__VA_ARGS__)

/**
 * @brief Defines a way to hold an error or a result value.
 *
 * @tparam T The result type.
 */
template<typename T, typename SourceLocationType = FSourceLocation>
class TErrorOr final
{
public:

	using ErrorType = TError<SourceLocationType>;

	/**
	 * @brief Sets default values for this object's properties.
	 */
	TErrorOr() = default;

	/**
	 * @brief Sets default values for this object's properties.
	 *
	 * @param value The value to set.
	 */
	TErrorOr(T value)
		: m_Value { MoveTemp(value) }
	{
	}

	/**
	 * @brief Sets default values for this object's properties.
	 *
	 * @param error The error to set.
	 */
	TErrorOr(ErrorType error)
		: m_Value { MoveTemp(error) }
	{
	}

	/**
	 * @brief Checks to see if this object contains an error.
	 *
	 * @return True if this object contains an error, otherwise false.
	 */
	[[nodiscard]] bool IsError() const
	{
		return m_Value.template Is<ErrorType>();
	}

	/**
	 * @brief Gets the underlying error.
	 *
	 * @return The underlying error.
	 */
	[[nodiscard]] const ErrorType& GetError() const
	{
		return m_Value.template GetValue<ErrorType>();
	}

	/**
	 * @brief Gets the underlying value.
	 *
	 * @return The underlying value.
	 */
	[[nodiscard]] const T& GetValue() const
	{
		return m_Value.template GetValue<T>();
	}

	/**
	 * @brief Releases ownership of the underlying error.
	 *
	 * @return The underlying error.
	 */
	[[nodiscard]] ErrorType ReleaseError()
	{
		return MoveTemp(m_Value.template GetValue<ErrorType>());
	}

	/**
	 * @brief Releases ownership of the underlying value.
	 *
	 * @return The underlying value.
	 */
	[[nodiscard]] T ReleaseValue()
	{
		return MoveTemp(m_Value.template GetValue<T>());
	}

private:

	TVariant<T, ErrorType> m_Value;
};

/**
 * @brief Specialization of TErrorOr for void return types.
 */
template<typename SourceLocationType>
class TErrorOr<void, SourceLocationType> final
{
public:

	using ErrorType = TError<SourceLocationType>;

	/**
	 * @brief Sets default values for this object's properties.
	 */
	TErrorOr() = default;

	/**
	 * @brief Sets default values for this object's properties.
	 *
	 * @param error The error to set.
	 */
	TErrorOr(ErrorType error)
		: m_ErrorValue { MoveTemp(error) }
	{
	}

	/**
	 * @brief Checks to see if this object contains an error.
	 *
	 * @return True if this object contains an error, otherwise false.
	 */
	[[nodiscard]] bool IsError() const
	{
		return m_ErrorValue.HasValue();
	}

	/**
	 * @brief Gets the underlying error.
	 *
	 * @return The underlying error.
	 */
	[[nodiscard]] const ErrorType& GetError() const
	{
		return m_ErrorValue.GetValue();
	}

	/**
	 * @brief Gets the underlying value.
	 *
	 * @return The underlying value.
	 */
	void GetValue() const { }

	/**
	 * @brief Releases ownership of the underlying value.
	 */
	void ReleaseValue() { }

	/**
	 * @brief Releases ownership of the underlying error.
	 *
	 * @return The error.
	 */
	[[nodiscard]] ErrorType ReleaseError()
	{
		return m_ErrorValue.ReleaseValue();
	}

private:

	TOptional<ErrorType> m_ErrorValue;
};