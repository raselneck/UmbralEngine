#pragma once

#include "Containers/StringView.h"
#include "Engine/Assert.h"
#include "Engine/MiscMacros.h"
#include "Misc/Badge.h"
#include "Misc/StringFormatting.h"

#define UM_LOG(LogLevel, Message, ...)                                                                          \
	do                                                                                                          \
	{                                                                                                           \
		if (ELogLevel:: LogLevel < ::Private::FLogger::GetLogLevel())                                           \
		{                                                                                                       \
			break;                                                                                              \
		}                                                                                                       \
		::Private::FLogger::GetInstance().Write(ELogLevel:: LogLevel, UM_JOIN(Message, _sv), ##__VA_ARGS__);    \
		if constexpr (ELogLevel:: LogLevel == ELogLevel::Fatal)                                                 \
		{                                                                                                       \
			UMBRAL_DEBUG_BREAK();                                                                               \
			::Private::SeriouslyDontTouch::CrashApplication();                                                  \
		}                                                                                                       \
	} while (0)

class FApplication;

/**
 * @brief An enumeration of possible log levels.
 */
enum class ELogLevel : uint8
{
	Verbose,
	Debug,
	Info,
	Warning,
	Error,
	Assert,
	Fatal
};

namespace Private
{
	class ILoggerInstance;

	/**
	 * @brief Defines access to the logger.
	 */
	class FLogger final
	{
	public:

		/**
		 * @brief Gets the logger instance.
		 *
		 * @return The logger instance.
		 */
		static Private::ILoggerInstance& GetInstance();

		/**
		 * @brief Gets the lowest log level allowed to be printed.
		 *
		 * @return The lowest log level allowed to be printed.
		 */
		static ELogLevel GetLogLevel();

		/**
		 * @brief Sets the lowest log level allowed to be printed.
		 *
		 * @param logLevel The lowest log level allowed to be printed.
		 */
		static void SetLogLevel(ELogLevel logLevel);
	};

	/**
	 * @brief Defines the interface for logger instances.
	 */
	class ILoggerInstance
	{
	public:

		/**
		 * @brief Destroys this logger instance.
		 */
		virtual ~ILoggerInstance() = default;

		/**
		 * @brief Attempts to initialize this logger instance.
		 *
		 * @return True if this logger instance was successfully initialized, otherwise false.
		 */
		virtual bool Initialize() = 0;

		/**
		 * @brief Flushes the logger instance.
		 */
		virtual void Flush() = 0;

		/**
		 * @brief Shuts this logger instance down.
		 */
		virtual void Shutdown() = 0;

		/**
		 * @brief Writes a formatted message to the log.
		 *
		 * @tparam ArgTypes The types of the arguments for the message.
		 * @param logLevel The log level.
		 * @param messageFormat The message format string.
		 * @param messageArgs The arguments for the message.
		 */
		template<typename... ArgTypes>
		void Write(const ELogLevel logLevel, const FStringView message, ArgTypes&&... messageArgs)
		{
			TArray<Private::FStringFormatArgument> formatArgs = Private::MakeFormatArgumentArray(Forward<ArgTypes>(messageArgs)...);
			WriteImpl(logLevel, message, formatArgs.AsSpan());
		}

	protected:

		/**
		 * @brief Writes the given message to the log.
		 *
		 * @param logLevel The log level.
		 * @param message The message.
		 * @param messageArgs The message formatting arguments.
		 */
		virtual void WriteImpl(ELogLevel logLevel, FStringView message, TSpan<Private::FStringFormatArgument> messageArgs) = 0;
	};
}