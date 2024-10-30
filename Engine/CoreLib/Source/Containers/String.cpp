#include "Containers/String.h"
#include "Containers/Function.h"
#include "Containers/InternalString.h"
#include "Containers/StringView.h"
#include "Engine/Internationalization.h"
#include "Engine/Logging.h"
#include "Math/Math.h"
#include "Memory/Memory.h"
#include "Misc/CString.h"
#include "Misc/StringBuilder.h"
#include "Misc/Unicode.h"
#include <cstdio>

static_assert(sizeof(FString::CharType) == 1, "Switching off of UTF-8 strings? That's more work than you expect...");

namespace Private
{
	DEFINE_PRIMITIVE_TYPE_DEFINITION(FString)

	FLongStringData::SizeType FLongStringData::Length() const
	{
		if (Chars.IsEmpty())
		{
			return 0;
		}

		return Chars.Num() - 1;
	}

	FShortStringData::FShortStringData()
		: Chars { }
		, Length { 0 }
	{
		FMemory::ZeroOutArray(Chars);
	}

	FShortStringData::FShortStringData(const FShortStringData& other)
		: Chars { }
		, Length { 0 }
	{
		FMemory::Copy(this, &other, sizeof(FShortStringData));
	}

	FShortStringData::FShortStringData(FShortStringData&& other) noexcept
		: Chars { }
		, Length { 0 }
	{
		FMemory::Copy(this, &other, sizeof(FShortStringData));

		FMemory::ZeroOutArray(other.Chars);
		other.Length = 0;
	}

	FShortStringData& FShortStringData::operator=(const FShortStringData& other)
	{
		if (&other == this)
		{
			return *this;
		}

		FMemory::Copy(this, &other, sizeof(FShortStringData));
		return *this;
	}

	FShortStringData& FShortStringData::operator=(FShortStringData&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}

		FMemory::Copy(this, &other, sizeof(FShortStringData));

		FMemory::ZeroOutArray(other.Chars);
		other.Length = 0;

		return *this;
	}
}

FString::FString(const CharType* chars)
{
	const SizeType numChars = TraitsType::GetNullTerminatedLength(chars);
	if (numChars == 0)
	{
		return;
	}

	Append(FStringView { chars, numChars });
}

FString::FString(const CharType* chars, const SizeType numChars)
{
	if (chars == nullptr || numChars == 0)
	{
		return;
	}

	UM_ASSERT(numChars > 0, "Cannot create a string with a negative number of characters");
	Append(FStringView { chars, numChars });
}

FString::FString(const FStringView chars)
{
	if (chars.IsEmpty())
	{
		return;
	}

	Append(chars);
}

FString::FString(const TSpan<const char> chars)
{
	if (chars.IsEmpty())
	{
		return;
	}

	Append(chars);
}

FString::FString(TBadge<FStringBuilder>, TArray<FString::CharType> chars)
{
	UM_ASSERT(chars.IsEmpty() || chars.Last() == TraitsType::NullChar, "Given character array is malformed");

	Private::FLongStringData& longData = m_CharData.ResetToType<Private::FLongStringData>();
	longData.Chars = MoveTemp(chars);
}

void FString::Append(const CharType* chars, const SizeType numChars)
{
	if (chars == nullptr || numChars <= 0)
	{
		return;
	}

	Append(FStringView { chars, numChars });
}

void FString::Append(const FStringView value)
{
	if (value.IsEmpty())
	{
		return;
	}

	ConvertToLongStringDataIfNecessary(value.Length());

	m_CharData.Visit(FVariantVisitor
	{
		[value](Private::FShortStringData& shortData)
		{
			FMemory::Copy(shortData.Chars + shortData.Length, value.GetChars(), value.Length());
		    shortData.Length += static_cast<uint8>(value.Length());
		},
		[value](Private::FLongStringData& longData)
		{
			const bool needNullTerminator = longData.Chars.IsEmpty();
			const SizeType indexToCopyTo = longData.Chars.IsEmpty() ? 0 : longData.Chars.Num() - 1;

			longData.Chars.AddZeroed(value.Length() + (needNullTerminator ? 1 : 0));
			FMemory::Copy(longData.Chars.GetData() + indexToCopyTo, value.GetChars(), value.Length());

			UM_ENSURE(longData.Chars.Last() == TraitsType::NullChar);
		}
	});
}

void FString::Append(const TSpan<const char> value)
{
	Append(FStringView { value });
}

FString FString::AsLower() const
{
	FString result { *this };
	result.ToLower();
	return result;
}

FString FString::AsUpper() const
{
	FString result { *this };
	result.ToUpper();
	return result;
}

bool FString::EndsWith(const FStringView other, EIgnoreCase ignoreCase) const
{
	return AsStringView().EndsWith(other, ignoreCase);
}

FString FString::FormatCStyle(const FStringView format, ...)
{
	va_list list {};
	va_start(list, format);
	FString result = FormatVarArgList(format, list);
	va_end(list);

	return result;
}

FString FString::FormatVarArgList(const FStringView formatView, va_list args)
{
	// NOTE This SUCKS but sometimes the format view is not null-terminated
	const FString format { formatView };

	CharType buffer[2048];
	FMemory::ZeroOutArray(buffer);

#if UMBRAL_PLATFORM_IS_WINDOWS
	const int32 numCharsFormatted = ::vsprintf_s(buffer, UM_ARRAY_SIZE(buffer) - 1, format.GetChars(), args);
#else
	const int32 numCharsFormatted = ::vsnprintf(buffer, UM_ARRAY_SIZE(buffer) - 1, format.GetChars(), args);
#endif

	FString result;
	if (UM_ENSURE(numCharsFormatted > 0))
	{
		result.Append(buffer, numCharsFormatted);
	}

	return result;
}

FString FString::FromByteArray(const TArray<uint8>& bytes)
{
	TArray<uint8> copyOfBytes = bytes;
	return FromByteArray(MoveTemp(copyOfBytes));
}

FString FString::FromByteArray(TArray<uint8>&& bytes)
{
	FString result;

	TArray<char> chars = bytes.ReleaseAs<char>();
	if (chars.Num() > 0 && chars.Last() != TraitsType::NullChar)
	{
		chars.Add(TraitsType::NullChar);
	}

	result.m_CharData.ResetToType<Private::FLongStringData>(MoveTemp(chars));

	return result;
}

FString FString::FromUtf16(const char16_t* chars)
{
	const int32 numChars = TCharTraits<char16_t>::GetNullTerminatedLength(chars);
	if (numChars == 0)
	{
		return {};
	}

	return FromUtf16(TSpan<const char16_t> { chars, numChars });
}

FString FString::FromUtf16(TSpan<const char16_t> charSpan)
{
	FString result;

	Unicode::FToUtf8Result convertResult = Unicode::ToUtf8(charSpan);
	if (convertResult.bValid)
	{
		Private::FLongStringData& longData = result.m_CharData.ResetToType<Private::FLongStringData>();
		longData.Chars = MoveTemp(convertResult.Chars);
	}

	return result;
}

FString FString::FromUtf32(const char32_t* chars)
{
	const int32 numChars = TCharTraits<char32_t>::GetNullTerminatedLength(chars);
	if (numChars == 0)
	{
		return {};
	}

	return FromUtf32(TSpan<const char32_t> { chars, numChars });
}

FString FString::FromUtf32(TSpan<const char32_t> charSpan)
{
	FString result;

	Unicode::FToUtf8Result convertResult = Unicode::ToUtf8(charSpan);
	if (convertResult.bValid)
	{
		Private::FLongStringData& longData = result.m_CharData.ResetToType<Private::FLongStringData>();
		longData.Chars = MoveTemp(convertResult.Chars);
	}

	return result;
}

FString FString::FromWide(const wchar_t* chars)
{
	const int32 numChars = TCharTraits<wchar_t>::GetNullTerminatedLength(chars);
	if (numChars == 0)
	{
		return {};
	}

	return FromWide(TSpan<const wchar_t> { chars, numChars });
}

FString FString::FromWide(TSpan<const wchar_t> charSpan)
{
	FString result;

	Unicode::FToUtf8Result convertResult = Unicode::ToUtf8(charSpan);
	if (convertResult.bValid)
	{
		Private::FLongStringData& longData = result.m_CharData.ResetToType<Private::FLongStringData>();
		longData.Chars = MoveTemp(convertResult.Chars);
	}

	return result;
}

const FString::CharType* FString::GetChars() const
{
	return const_cast<FString*>(this)->GetChars();
}

FString::CharType* FString::GetChars()
{
	return m_CharData.Visit<CharType*>(FVariantVisitor
	{
		[](Private::FShortStringData& shortData)
		{
			return shortData.Chars;
		},
		[](Private::FLongStringData& longData)
		{
			return longData.Chars.GetData();
		}
	});
}

bool FString::IsEmpty() const
{
	return m_CharData.Visit<bool>(FVariantVisitor
	{
		[](const Private::FShortStringData& shortData)
		{
			return shortData.Length == 0;
		},
		[](const Private::FLongStringData& longData)
		{
			return longData.Chars.IsEmpty();
		}
	});
}

bool FString::IsValidIndex(const SizeType index) const
{
	return m_CharData.Visit<bool>(FVariantVisitor
	{
		[index](const Private::FShortStringData& shortData)
		{
			return index >= 0 && index < shortData.Length;
		},
		[index](const Private::FLongStringData& longData)
		{
			return longData.Chars.IsValidIndex(index);
		}
	});
}

TOptional<FString::CharType> FString::Last() const
{
	const CharType* chars = GetChars();
	const SizeType length = Length();

	if (chars == nullptr || length <= 0)
	{
		return nullopt;
	}

	return chars[length - 1];
}

FString::SizeType FString::Length() const
{
	return m_CharData.Visit<SizeType>(FVariantVisitor
	{
		[](const Private::FShortStringData& shortData)
		{
			return static_cast<SizeType>(shortData.Length);
		},
		[](const Private::FLongStringData& longData)
		{
			return longData.Length();
		}
	});
}

void FString::Prepend(const CharType* chars, SizeType numChars)
{
	if (chars == nullptr || numChars <= 0)
	{
		return;
	}

	Prepend(FStringView { chars, numChars });
}

void FString::Prepend(const FStringView value)
{
	if (value.IsEmpty())
	{
		return;
	}

	ConvertToLongStringDataIfNecessary(value.Length());

	m_CharData.Visit(FVariantVisitor
	{
		[value](Private::FShortStringData& shortData)
		{
			// Move the existing characters
			FMemory::CopyInReverse(shortData.Chars + value.Length(), shortData.Chars, shortData.Length);

			// Copy `value` to the beginning
			FMemory::Copy(shortData.Chars, value.GetChars(), value.Length());

			shortData.Length += static_cast<uint8>(value.Length());
		},
		[value](Private::FLongStringData& longData)
		{
			const bool needNullTerminator = longData.Chars.IsEmpty();
			const int32 originalLength = longData.Length();

			longData.Chars.AddZeroed(value.Length() + (needNullTerminator ? 1 : 0));

			// Move the existing characters
			if (originalLength > 0)
			{
			    const SizeType indexToCopyTo = value.Length();
				FMemory::CopyInReverse(longData.Chars.GetData() + indexToCopyTo, longData.Chars.GetData(), originalLength);
			}

			// Copy `value` to the beginning
			FMemory::Copy(longData.Chars.GetData(), value.GetChars(), value.Length());

			UM_ENSURE(longData.Chars.Last() == TraitsType::NullChar);
		}
	});
}

void FString::Reserve(const SizeType numChars)
{
	if (numChars <= 0)
	{
		return;
	}

	// TODO Implement this so we don't always have to use long string data :^)

	ConvertToLongStringData();

	Private::FLongStringData& longData = m_CharData.GetValue<Private::FLongStringData>();
	longData.Chars.Reserve(numChars);
}

void FString::Reset()
{
	m_CharData.Visit(FVariantVisitor
	{
		[](Private::FShortStringData& shortData)
		{
			FMemory::ZeroOutArray(shortData.Chars);
		    shortData.Length = 0;
		},
		[](Private::FLongStringData& longData)
		{
			longData.Chars.Reset();
		}
	});
}

FString FString::MakeFormattedString(const FStringView format, const TSpan<Private::FStringFormatArgument> args)
{
	FStringBuilder builder;
	builder.Reserve(format.Length() + 1);

	Private::AppendFormattedString(builder, format, args);

	return builder.ReleaseString();
}

void FString::SplitByChars(FStringView chars, EStringSplitOptions options, TArray<FString>& result) const
{
	if (IsEmpty())
	{
		return;
	}

	if (chars.IsEmpty())
	{
		result.Add(*this);
	}

	Private::SplitString<FString>(*this, options, chars, result,
	                              Private::GetNextIndexOfClosestCharToSplitOn<FString>);
}

void FString::SplitByCharsIntoViews(FStringView chars, EStringSplitOptions options, TArray<FStringView>& result) const
{
	if (IsEmpty())
	{
		return;
	}

	if (chars.IsEmpty())
	{
		result.Add(*this);
	}

	Private::SplitString<FString, FStringView>(*this, options, chars, result,
	                                           Private::GetNextIndexOfClosestCharToSplitOn<FString>);
}

void FString::SplitByString(FStringView substring, EStringSplitOptions options, TArray<FString>& result) const
{
	if (IsEmpty())
	{
		return;
	}

	if (substring.IsEmpty())
	{
		result.Add(*this);
	}

	Private::SplitString<FString, FString>(*this, options, substring, result,
	                                       Private::GetNextIndexOfSubstringToSplitOn<FString>);
}

void FString::SplitByStringIntoViews(FStringView substring, EStringSplitOptions options, TArray<FStringView>& result) const
{
	if (IsEmpty())
	{
		return;
	}

	if (substring.IsEmpty())
	{
		result.Add(*this);
	}

	Private::SplitString<FString, FStringView>(*this, options, substring, result,
	                                           Private::GetNextIndexOfSubstringToSplitOn<FStringView>);
}

bool FString::StartsWith(const FStringView other, EIgnoreCase ignoreCase) const
{
	return AsStringView().StartsWith(other, ignoreCase);
}

FString FString::Substring(const SizeType index, const SizeType length) const
{
	return FString { SubstringView(index, length) };
}

FStringView FString::SubstringView(const SizeType index, const SizeType length) const
{
	// TODO Do we want to assert here, or just return empty strings

	UM_ASSERT(index >= 0, "Substring start index must be positive");
	UM_ASSERT(index < Length(), "Substring start index must be less than this string view's length");
	UM_ASSERT(length >= 0, "Substring length must be positive");
	UM_ASSERT(index <= Length() - length, "Substring length is out of bounds");

	if (length == 0)
	{
		return {};
	}
	if (index == 0 && length == Length())
	{
		return *this;
	}

	return FStringView { GetChars() + index, length };
}

void FString::ToLower()
{
	TArray<char> result = FInternationalization::ConvertStringToLower(AsSpan());
	UM_ENSURE(result.Last() == TraitsType::NullChar);

	Private::FLongStringData& longData = m_CharData.ResetToType<Private::FLongStringData>();
	longData.Chars = MoveTemp(result);
}

void FString::ToUpper()
{
	TArray<char> result = FInternationalization::ConvertStringToUpper(AsSpan());
	UM_ENSURE(result.Last() == TraitsType::NullChar);

	Private::FLongStringData& longData = m_CharData.ResetToType<Private::FLongStringData>();
	longData.Chars = MoveTemp(result);
}

TArray<char16_t> FString::ToUtf16Chars() const
{
	TArray<char16_t> result;

	Unicode::FToUtf16Result convertResult = Unicode::ToUtf16(AsSpan());
	if (convertResult.bValid)
	{
		result = MoveTemp(convertResult.Chars);
	}

	return result;
}

TArray<char32_t> FString::ToUtf32Chars() const
{
	TArray<char32_t> result;

	Unicode::FToUtf32Result convertResult = Unicode::ToUtf32(AsSpan());
	if (convertResult.bValid)
	{
		result = MoveTemp(convertResult.Chars);
	}

	return result;
}

TArray<wchar_t> FString::ToWideChars() const
{
	UM_ASSERT_NOT_REACHED_MSG("FString::ToWideChars not yet implemented");
}

void FString::ConvertToLongStringDataIfNecessary(const SizeType numCharsBeingAppended)
{
	if (IsUsingLongStringData())
	{
		return;
	}

	UM_ASSERT(numCharsBeingAppended > 0, "Attempting to append zero (or a negative amount of) characters to string");

	const SizeType numChars = m_CharData.GetValue<Private::FShortStringData>().Length;
	if (numChars + numCharsBeingAppended <= Private::FShortStringData::MaxLength)
	{
		return;
	}

	ConvertToLongStringData(numCharsBeingAppended);
}

void FString::ConvertToLongStringData(const SizeType numAdditionalChars)
{
	if (IsUsingLongStringData())
	{
		return;
	}

	const Private::FShortStringData copyOfShortStringData = m_CharData.GetValue<Private::FShortStringData>();

	Private::FLongStringData& longData = m_CharData.ResetToType<Private::FLongStringData>();

	// Don't do anything more if we don't need to copy any char data
	if (copyOfShortStringData.Length == 0 && numAdditionalChars == 0)
	{
		return;
	}

	longData.Chars.Reserve(copyOfShortStringData.Length + numAdditionalChars + 1);

	if (copyOfShortStringData.Length > 0)
	{
		longData.Chars.Append(copyOfShortStringData.Chars, copyOfShortStringData.Length + 1);
	}
}

bool FString::IsUsingLongStringData() const
{
	return m_CharData.Is<Private::FLongStringData>();
}

bool FString::IsUsingShortStringData() const
{
	return m_CharData.Is<Private::FShortStringData>();
}
