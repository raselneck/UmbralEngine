#include "Engine/Logging.h"

namespace Private
{
	static ELogLevel GLogLevel = ELogLevel::Info;

	ELogLevel FLogger::GetLogLevel()
	{
		return GLogLevel;
	}

	void FLogger::SetLogLevel(ELogLevel logLevel)
	{
		GLogLevel = logLevel;
	}
}