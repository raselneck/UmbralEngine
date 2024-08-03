#include "Engine/Assert.h"
#include "Engine/Logging.h"
#include "Containers/StringView.h"
#include "Templates/StringTraits.h"
#include <exception> /* For std::terminate */

namespace Private
{
	bool AlwaysReturnsFalse()
	{
		return false;
	}

	// FIXME: Remove this function in favor of our own GetBaseName or something in FPath
	FStringView GetFileNameWithoutPath(const char* filePathPtr)
	{
		const int32 filePathLength = TStringTraits<char>::GetNullTerminatedCharCount(filePathPtr);
		const char* fileNamePtr = filePathPtr + filePathLength - 1;

		while (fileNamePtr > filePathPtr)
		{
			if (*fileNamePtr == '\\' || *fileNamePtr == '/')
			{
				const int32 fileNameLength = filePathLength - static_cast<int32>(fileNamePtr - filePathPtr) - 1;
				return FStringView { fileNamePtr + 1, fileNameLength };
			}

			--fileNamePtr;
		}

		return FStringView { filePathPtr, filePathLength };
	}

	// FIXME: Pass in string views instead of raw string literals here
	void LogAssert(const char* conditionText, const char* messageText, const char* filePtr, const int line, const char* functionNamePtr)
	{
		const FStringView condition { conditionText };
		const FStringView message { messageText };
		const FStringView fileName = GetFileNameWithoutPath(filePtr);
		const FStringView functionName { functionNamePtr };

		UM_LOG(Assert, "Assertion failed: {}\n\tWith: {}\n\tAt:   {}:{}\n\tIn:   {}", message, condition, fileName, line, functionName);
	}

	bool ShouldBreakForEnsure(const bool condition, const char* conditionText, const char* fileName, const int line)
	{
		if (condition)
		{
			return false;
		}

		UM_LOG(Error, "Ensure condition failed: {}\n\tAt:   {}:{}", conditionText, fileName, line);

		return true;
	}
}

namespace Private::SeriouslyDontTouch
{
	void CrashApplication()
	{
		// TODO We may need to only shut down the logging subsystem
		// TODO Allow the application to shutdown?
		std::terminate();
	}
}