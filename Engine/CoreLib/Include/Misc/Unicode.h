#pragma once

#include "Containers/Array.h"
#include "Containers/Optional.h"
#include "Containers/Span.h"
#include "Engine/IntTypes.h"

// TODO Stop using the conversion result types and return a TErrorOr<...>

namespace Unicode
{
	struct FCountCodePointsResult
	{
		int32 NumCodePoints = 0;
		bool bValid = false;
	};

	FCountCodePointsResult CountCodePoints(const char* chars);
	FCountCodePointsResult CountCodePoints(TSpan<const char> charSpan);
	FCountCodePointsResult CountCodePoints(const wchar_t* chars);
	FCountCodePointsResult CountCodePoints(TSpan<const wchar_t> charSpan);
	FCountCodePointsResult CountCodePoints(const char16_t* chars);
	FCountCodePointsResult CountCodePoints(TSpan<const char16_t> charSpan);
	FCountCodePointsResult CountCodePoints(const char32_t* chars);
	FCountCodePointsResult CountCodePoints(TSpan<const char32_t> charSpan);

	struct FToUtf8Result
	{
		TArray<char> Chars;
		bool bValid = false;
	};

	FToUtf8Result ToUtf8(TSpan<const wchar_t> charSpan);
	FToUtf8Result ToUtf8(TSpan<const char16_t> charSpan);
	FToUtf8Result ToUtf8(TSpan<const char32_t> charSpan);

	struct FToUtf16Result
	{
		TArray<char16_t> Chars;
		bool bValid = false;
	};

	FToUtf16Result ToUtf16(TSpan<const char> charSpan);
	FToUtf16Result ToUtf16(TSpan<const wchar_t> charSpan);
	FToUtf16Result ToUtf16(TSpan<const char32_t> charSpan);

	struct FToUtf32Result
	{
		TArray<char32_t> Chars;
		bool bValid = false;
	};

	FToUtf32Result ToUtf32(TSpan<const char> charSpan);
	FToUtf32Result ToUtf32(TSpan<const wchar_t> charSpan);
	FToUtf32Result ToUtf32(TSpan<const char16_t> charSpan);
}