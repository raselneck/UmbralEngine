#include "Regex/Regex.h"
#include "re.h"

FStringView FRegex::Match(const FString& pattern, const FString& text)
{
	int32 matchLength = 0;
	const int32 matchIndex = re_match(pattern.GetChars(), text.GetChars(), &matchLength);

	if (matchIndex < 0)
	{
		return {};
	}

	return text.SubstringView(matchIndex, matchLength);
}

TArray<FStringView> FRegex::MatchAll(const FString& pattern, const FString& text)
{
	re_t regex = re_compile(pattern.GetChars());
	const char* originalChars = text.GetChars();
	const char* currentChars = originalChars;

	TArray<FStringView> matches;
	int32 matchLength = 0;
	int32 matchIndex = re_matchp(regex, currentChars, &matchLength);
	while (matchIndex >= 0)
	{
		const int32 actualMatchIndex = static_cast<int32>(currentChars - originalChars) + matchIndex;
		matches.Add(text.SubstringView(actualMatchIndex, matchLength));

		currentChars = originalChars + actualMatchIndex + matchLength;

		matchIndex = re_matchp(regex, currentChars, &matchLength);
	}

	return matches;
}

bool FRegex::Matches(const FString& pattern, const FString& text)
{
	const FStringView firstMatch = Match(pattern, text);
	return firstMatch.IsEmpty() == false;
}