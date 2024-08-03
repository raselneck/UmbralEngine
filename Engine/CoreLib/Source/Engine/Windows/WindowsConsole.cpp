#include "Engine/Logging.h"
#include "Engine/Windows/WindowsConsole.h"
#include <cstdio>

FString FWindowsConsole::ReadLine()
{
	char buffer[2048];
	FMemory::ZeroOutArray(buffer);

	const char* result = ::fgets(buffer, UM_ARRAY_SIZE(buffer), stdin);
	if (result == nullptr)
	{
		if (::feof(stdin))
		{
			UM_LOG(Error, "End of input stream reached");
		}
		if (::ferror(stdin))
		{
			::strerror_s(buffer, UM_ARRAY_SIZE(buffer), errno);
			UM_LOG(Error, "Input stream error; message={}", buffer);
		}
		return {};
	}

	int32 bufferLength = FStringView::CharTraitsType::GetNullTerminatedLength(buffer);
	while (bufferLength > 0 && buffer[bufferLength - 1] == '\n')
	{
		--bufferLength;
	}

	return FString { buffer, bufferLength };
}