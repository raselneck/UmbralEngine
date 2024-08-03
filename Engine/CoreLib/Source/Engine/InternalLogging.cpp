#include "Containers/InternalString.h"
#include "Engine/InternalLogging.h"
#include "Engine/Logging/DynamicLoggerInstance.h"
#include "HAL/DateTime.h"
#include "Misc/StringBuilder.h"

namespace Private
{
	static FDynamicLoggerInstance GLoggerInstance;

	ILoggerInstance& FLogger::GetInstance()
	{
		return GLoggerInstance;
	}

	constexpr FStringView GetLogTagForLogLevel(const ELogLevel logLevel)
	{
		switch (logLevel)
		{
		case ELogLevel::Fatal:
			return "[FATAL]"_sv;
		case ELogLevel::Assert:
			return "[ASSRT]"_sv;
		case ELogLevel::Error:
			return "[ERROR]"_sv;
		case ELogLevel::Warning:
			return "[WARNG]"_sv;
		case ELogLevel::Debug:
			return "[DEBUG]"_sv;
		case ELogLevel::Info:
			return "[INFO ]"_sv;
		case ELogLevel::Verbose:
			return "[VRBOS]"_sv;
		default:
			UM_ASSERT_NOT_REACHED();
		}
	}

	FString CreateLogString(ELogLevel logLevel, FStringView message)
	{
		return CreateLogString(logLevel, message, {});
	}

	FString CreateLogString(ELogLevel logLevel, FStringView message, const TSpan<Private::FStringFormatArgument> messageArgs)
	{
		FStringBuilder builder;
		builder.Reserve(message.Length() + messageArgs.Num() * 4 + 20);

		// Append the log time
		TFormatter<FDateTime> dateTimeFormatter;
		dateTimeFormatter.Parse("%H:%M:%S.%s"_sv);
		dateTimeFormatter.BuildString(FDateTime::Now(), builder);

		// Append the log tag
		builder.Append(" "_sv);
		builder.Append(GetLogTagForLogLevel(logLevel));

		// Append the message
		builder.Append(" "_sv);
		Private::AppendFormattedString(builder, message, messageArgs);

		return builder.Append("\n"_sv).ReleaseString();
	}

	void InitializeLogging()
	{
		if (GLoggerInstance.IsInitialized())
		{
			return;
		}

		GLoggerInstance.Initialize();
	}

	void ShutdownLogging()
	{
		GLoggerInstance.Shutdown();
	}
}