#include "Containers/InternalString.h"
#include "Containers/StaticArray.h"
#include "Engine/Assert.h"
#include "Engine/Logging.h"
#include "Engine/Platform.h"
#include "Math/Math.h"
#include <cinttypes> /* For PRI* macros */
#include <cstdio> /* For std::snprintf */

const FToCharsArgs FToCharsArgs::Default;

FToCharsArgs FToCharsArgs::PadLeft(const char padChar, const int32 padAmount)
{
	UM_ASSERT(padAmount >= 0, "Cannot pad left with a negative character count");

	FToCharsArgs result;
	result.PadLeftAmount = padAmount;
	result.PadLeftChar = padChar;
	return result;
}

FToCharsArgs FToCharsArgs::PadRight(const char padChar, const int32 padAmount)
{
	UM_ASSERT(padAmount >= 0, "Cannot right left with a negative character count");

	FToCharsArgs result;
	result.PadRightAmount = padAmount;
	result.PadRightChar = padChar;
	return result;
}

FToCharsArgs FToCharsArgs::ParseFormat(const FStringView /*format*/)
{
	// TODO Actually parse the format string
	return {};
}

namespace Private
{
	FStringView RemoveLastCharacter(FStringView value)
	{
		if (value.IsEmpty())
		{
			return value;
		}

		return FStringView { value.GetChars(), value.Length() - 1 };
	}

	template<int32 N, typename... ArgTypes>
	FStringView SafeFormatValue(TStaticArray<char, N>& formatBuffer, const char* formatString, ArgTypes... args)
	{
		const int32 formatLength = std::snprintf(formatBuffer.GetData(), static_cast<size_t>(formatBuffer.Num()), formatString, args...);
		return FStringView { formatBuffer.GetData(), formatLength };
	}

	template<int32 N, typename IntegerType>
	FStringView SafeFormatIntegerValue(TStaticArray<char, N>& formatBuffer, const IntegerType value, const FToCharsArgs& args)
	{
		if constexpr (IsSigned<IntegerType>)
		{
			if (value < 0)
			{
				return SafeFormatValue(formatBuffer, "%" PRId64, value);
			}
		}

		switch (args.NumericBase)
		{
		case ENumericBase::Octal:
			return SafeFormatValue(formatBuffer, "%" PRIo64, static_cast<uint64>(value));

		case ENumericBase::Hexadecimal:
			return SafeFormatValue(formatBuffer, "%" PRIx64, static_cast<uint64>(value));

		case ENumericBase::Binary:
			UM_LOG(Error, "Integer to binary is not currently implemented");
			[[fallthrough]];
		case ENumericBase::Decimal:
		default:
			if constexpr (IsSigned<IntegerType>)
			{
				return SafeFormatValue(formatBuffer, "%" PRId64, value);
			}
			else
			{
				return SafeFormatValue(formatBuffer, "%" PRIu64, value);
			}
		}
	}

	void AppendCharsForDouble(FStringBuilder& builder, const double value, const FToCharsArgs& args)
	{
		TStaticArray<char, 32> formatBuffer;
		FStringView formattedValue;
		if (args.NumDecimals.HasValue())
		{
			const int32 numDecimals = FMath::Max(0, args.NumDecimals.GetValue());
			formattedValue = SafeFormatValue(formatBuffer, "%.*f", numDecimals, value);
		}
		else
		{
			formattedValue = SafeFormatValue(formatBuffer, "%f", value);
		}

		// HACK Find the argument for snprintf that will automatically do this
		// If we were not told to pad the number on the right, let's remove trailing zeroes
		if (args.PadRightAmount.IsEmpty() && args.NumDecimals.IsEmpty())
		{
			// Remove trailing zeroes
			while (formattedValue.Length() > 1 && formatBuffer[formattedValue.Length() - 1] == '0')
			{
				formattedValue = RemoveLastCharacter(formattedValue);
			}

			// Remove unnecessary decimal
			if (formattedValue.Length() > 1 && formatBuffer[formattedValue.Length() - 1] == '.')
			{
				formattedValue = RemoveLastCharacter(formattedValue);
			}
		}

		AppendStringWithArguments(builder, formattedValue, args);
	}

	void AppendCharsForFloat(FStringBuilder& builder, const float value, const FToCharsArgs& args)
	{
		// TODO Do we want to limit the number of precision characters for floats here?
		return AppendCharsForDouble(builder, value, args);
	}

	void AppendCharsForPointer(FStringBuilder& builder, const void* value, const FToCharsArgs& args)
	{
		TStaticArray<char, 32> formatBuffer;

		// NOTE We could use %p, but that produces implementation-defined output
#if UMBRAL_ARCH_IS_32BIT
		const FStringView formattedValue = SafeFormatValue(formatBuffer, "0x%08" PRIx32, reinterpret_cast<uint32>(value));
#else
		const FStringView formattedValue = SafeFormatValue(formatBuffer, "0x%016" PRIx64, reinterpret_cast<uint64>(value));
#endif

		AppendStringWithArguments(builder, formattedValue, args);
	}

	void AppendCharsForSignedInt(FStringBuilder& builder, const int64 value, const FToCharsArgs& args)
	{
		TStaticArray<char, 32> formatBuffer;
		const FStringView formattedValue = SafeFormatIntegerValue(formatBuffer, value, args);
		AppendStringWithArguments(builder, formattedValue, args);
	}

	void AppendCharsForUnsignedInt(FStringBuilder& builder, const uint64 value, const FToCharsArgs& args)
	{
		TStaticArray<char, 32> formatBuffer;
		const FStringView formattedValue = SafeFormatIntegerValue(formatBuffer, value, args);
		AppendStringWithArguments(builder, formattedValue, args);
	}

	void AppendStringWithArguments(FStringBuilder& builder, FStringView value, const FToCharsArgs& args)
	{
		// Check if we need to pad the value on the left
		if (args.PadLeftAmount.HasValue())
		{
			const int32 numCharsToPadLeft = args.PadLeftAmount.GetValue() - value.Length();
			builder.Append(args.PadLeftChar, numCharsToPadLeft);
		}

		// Append the value
		builder.Append(value);

		// Check if we need to pad the value on the left
		if (args.PadRightAmount.HasValue())
		{
			const int32 numCharsToPadRight = args.PadRightAmount.GetValue() - value.Length();
			builder.Append(args.PadRightChar, numCharsToPadRight);
		}
	}

	void AppendFormattedString(FStringBuilder& builder, FStringView format, const TSpan<Private::FStringFormatArgument> args)
	{
		// TODO Change this implementation to use IndexOf to search for the next { in `format'

		int32 currentArgIndex = 0;
		for (int32 idx = 0; idx < format.Length(); ++idx)
		{
			char currentChar = format[idx];

			// Check if we're escaping '{'
			if (currentChar == '\\' && idx + 1 < format.Length() && format[idx + 1] == '{')
			{
				++idx;
				builder.Append('{', 1);
				continue;
			}

			// Not a format param start? Record the character and move on
			if (currentChar != '{')
			{
				builder.Append(currentChar, 1);
				continue;
			}

			// Move to the next character
			++idx;
			if (idx == format.Length())
			{
				builder.Append("{"_sv);
				break;
			}

			currentChar = format[idx];

			// Double { is the "escape sequence" for a single {
			if (currentChar == '{')
			{
				builder.Append("{"_sv);
				continue;
			}

			// Parse the format string
			int32 formatStringStart = idx;
			while (idx < format.Length() && format[idx] != '}')
			{
				++idx;
			}

			UM_ASSERT(idx < format.Length(), "Encountered unterminated string format argument");
			UM_ASSERT(args.IsValidIndex(currentArgIndex), "Too few string format argument values provided");

			FStringView argumentFormat;
			if (idx != formatStringStart)
			{
				argumentFormat = format.Substring(formatStringStart, idx - formatStringStart);
			}

			args[currentArgIndex].BuildString(argumentFormat, builder);
			++currentArgIndex;
		}
	}
}