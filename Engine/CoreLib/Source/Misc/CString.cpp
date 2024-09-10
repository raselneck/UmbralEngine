#include "Engine/Assert.h"
#include "Memory/Memory.h"
#include "Misc/CString.h"
#include "Templates/IsInt.h"
#include "Templates/NumericLimits.h"
#include <cstring>
#include <SDL_stdinc.h>

#if UMBRAL_PLATFORM_IS_WINDOWS
#	include <Shlwapi.h>
#	undef StrChr
#	undef StrCmp
#	undef StrStr
#	pragma comment(lib, "Shlwapi.lib")
#elif __has_include(<strings.h>)
#	include <strings.h>
#	define WITH_STRINGS_HEADER 1
#endif

#ifndef WITH_STRINGS_HEADER
#	define WITH_STRINGS_HEADER 0
#endif

void FCString::FCStringDeleter::Delete(char* chars)
{
	FMemory::Free(chars);
}

FCString::FCString(const char* chars)
	: m_Chars { CopyCharArray(chars) }
{
}

FCString::FCString(const FCString& other)
	: m_Chars { CopyCharArray(other.GetChars()) }
{
}

FCString::FCString(FCString&& other) noexcept
	: m_Chars { MoveTemp(other.m_Chars) }
{
	other.m_Chars = nullptr;
}

bool FCString::IsAlpha(CharType ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool FCString::IsAlphaNumeric(CharType ch)
{
	return IsAlpha(ch) || IsNumeric(ch);
}

bool FCString::IsNumeric(CharType ch)
{
	return ch >= '0' && ch <= '9';
}

ECompareResult FCString::StrCaseCmp(const CharType* first, const CharType* second, SizeType numChars)
{
	UM_ASSERT(numChars >= 0, "Invalid number of characters supplied");

	const int32 result = SDL_strncasecmp(first, second, static_cast<size_t>(numChars));
	if (result < 0)
	{
		return ECompareResult::LessThan;
	}
	if (result > 0)
	{
		return ECompareResult::GreaterThan;
	}
	return ECompareResult::Equals;
}

ECompareResult FCString::StrCmp(const CharType* first, const CharType* second, SizeType numChars)
{
	UM_ASSERT(numChars >= 0, "Invalid number of characters supplied");

	const int32 result = SDL_strncmp(first, second, static_cast<size_t>(numChars));
	if (result < 0)
	{
		return ECompareResult::LessThan;
	}
	if (result > 0)
	{
		return ECompareResult::GreaterThan;
	}
	return ECompareResult::Equals;
}

FCString::SizeType FCString::StrCaseChr(const CharType* haystack, const SizeType haystackLength, const CharType needle)
{
	return StrCaseStr(haystack, haystackLength, &needle, 1);
}

FCString::SizeType FCString::StrChr(const CharType* haystack, const SizeType haystackLength, const CharType needle)
{
	return StrStr(haystack, haystackLength, &needle, 1);
}

static FCString::SizeType FindSubstringIndex(const FCString::CharType* haystack,
                                             const FCString::SizeType haystackLength,
                                             const FCString::CharType* needle,
                                             const FCString::SizeType needleLength,
                                             const FCString::StringCompareFunction compareFunction)
{
	using UnsignedSizeType = MakeUnsigned<FCString::SizeType>;

	if (static_cast<UnsignedSizeType>(needleLength) > static_cast<UnsignedSizeType>(haystackLength))
	{
		return INDEX_NONE;
	}

	for (FCString::SizeType idx = 0; idx < haystackLength; ++idx)
	{
		if (compareFunction(haystack + idx, needle, needleLength) == ECompareResult::Equals)
		{
			return idx;
		}
	}

	return INDEX_NONE;
}

FCString::SizeType FCString::StrCaseStr(const CharType* haystack, const SizeType haystackLength, const CharType* needle, const SizeType needleLength)
{
	return FindSubstringIndex(haystack, haystackLength, needle, needleLength, StrCaseCmp);
}

FCString::SizeType FCString::StrStr(const CharType* haystack, const SizeType haystackLength, const CharType* needle, const SizeType needleLength)
{
	return FindSubstringIndex(haystack, haystackLength, needle, needleLength, StrCmp);
}

FCString::CharType FCString::ToLower(CharType ch)
{
	return static_cast<CharType>(SDL_tolower(ch));
}

FCString::CharType FCString::ToUpper(CharType ch)
{
	return static_cast<CharType>(SDL_toupper(ch));
}

FCString& FCString::operator=(const FCString& other)
{
	if (&other == this)
	{
		return *this;
	}

	m_Chars.Reset();
	m_Chars = CopyCharArray(other.GetChars());

	return *this;
}

FCString& FCString::operator=(FCString&& other) noexcept
{
	if (&other == this)
	{
		return *this;
	}

	m_Chars = MoveTemp(other.m_Chars);

	return *this;
}

TUniquePtr<char, FCString::FCStringDeleter> FCString::CopyCharArray(const char* chars)
{
	if (chars == nullptr)
	{
		return nullptr;
	}

	const SizeType numChars = CharTraits::GetNullTerminatedLength(chars);
	if (numChars == 0)
	{
		return nullptr;
	}

	char* copyOfChars = FMemory::AllocateArray<char>(numChars + 1);
	FMemory::Copy(copyOfChars, chars, numChars);
	copyOfChars[numChars] = CharTraits::NullChar;

	return TUniquePtr<char, FCString::FCStringDeleter> { copyOfChars };
}