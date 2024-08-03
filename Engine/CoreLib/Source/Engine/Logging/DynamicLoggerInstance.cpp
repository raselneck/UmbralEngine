#include "Engine/Logging/DynamicLoggerInstance.h"
#include "Engine/Logging/FileLogListener.h"
#include "Engine/Logging/StdLogListener.h"
#include "Engine/InternalLogging.h"
#include "Memory/Memory.h"

bool FDynamicLoggerInstance::Initialize()
{
	m_Listeners.Add(MakeUnique<FStdLogListener>());

	// TODO VisualStudioLogListener

	TUniquePtr<FFileLogListener> fileLogger = MakeUnique<FFileLogListener>();
	if (fileLogger->Open("UmbralEngine.log"_sv))
	{
		m_Listeners.Add(MoveTemp(fileLogger));
	}
	else
	{
		WriteImpl(ELogLevel::Warning, "Failed to open log file for writing"_sv, {});
	}

	return true;
}

void FDynamicLoggerInstance::Flush()
{
	for (TUniquePtr<ILogListener>& listener : m_Listeners)
	{
		listener->Flush();
	}
}

void FDynamicLoggerInstance::Shutdown()
{
	m_Listeners.Clear();
}

void FDynamicLoggerInstance::WriteImpl(const ELogLevel logLevel, const FStringView message, const TSpan<Private::FStringFormatArgument> messageArgs)
{
	const FString formattedMessage = Private::CreateLogString(logLevel, message, messageArgs);
	for (TUniquePtr<ILogListener>& listener : m_Listeners)
	{
		listener->Write(logLevel, formattedMessage);
#if UMBRAL_DEBUG
		listener->Flush();
#endif
	}
}