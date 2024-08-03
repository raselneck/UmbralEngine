#include "Engine/Logging.h"
#include "Engine/Posix/PosixConsole.h"
#include <cstdio>
#include <cstring> /* For strerror */

FString FPosixConsole::ReadLine()
{
	char* stringBuffer = nullptr;
	size_t bufferSize = 0;
	ssize_t stringLength = ::getline(&stringBuffer, &bufferSize, stdin);

	if (stringLength < 0)
	{
		const FStringView errorMessage { ::strerror(errno) };
		UM_LOG(Error, "Failed to read line from stdin; error=\"{}\"", errorMessage);
		return {};
	}

	// Remove any trailing new line characters
	while (stringLength > 0 && stringBuffer[stringLength - 1] == '\n')
	{
		--stringLength;
	}

	FString result;
	result.Append(stringBuffer, static_cast<FString::SizeType>(stringLength));

	::free(stringBuffer);

	return result;
}