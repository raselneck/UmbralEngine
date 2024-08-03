#include "Engine/Console.h"
#include "Engine/Logging/StdLogListener.h"

void FStdLogListener::Flush()
{
	FConsole::Flush();
}

void FStdLogListener::Write(const ELogLevel logLevel, const FStringView string) const
{
	const EConsoleColor logColor = [](const ELogLevel logLevel)
	{
		switch (logLevel)
		{
		case ELogLevel::Warning:
			return EConsoleColor::BrightYellow;

		case ELogLevel::Error:
		case ELogLevel::Assert:
		case ELogLevel::Fatal:
			return EConsoleColor::BrightRed;

		default:
			return EConsoleColor::Default;
		}
	}(logLevel);

	const FScopedConsoleForegroundColor logColorScope { logColor };
	FConsole::Write(string);
}