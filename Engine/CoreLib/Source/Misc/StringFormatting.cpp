#include "Misc/StringFormatting.h"
#include "Containers/InternalString.h"
#include "Containers/String.h"
#include "Misc/StringBuilder.h"

template<>
struct TFormatter<FString>
{
	void BuildString(const FString& value, FStringBuilder& builder)
	{
		builder.Append(value);
	}

	bool Parse(const FStringView /*formatString*/)
	{
		// TODO Parse default formatting arguments
		return true;
	}
};

namespace Private
{
	FStringFormatArgument::FStringFormatArgument() = default;

	FStringFormatArgument::~FStringFormatArgument() = default;

	FStringFormatArgument::FStringFormatArgument(const char value)
	{
		switch (value)
		{
		case '\0': m_Value.ResetToType<FStringView>("\\0"_sv); break;
		case '\a': m_Value.ResetToType<FStringView>("\\a"_sv); break;
		case '\b': m_Value.ResetToType<FStringView>("\\b"_sv); break;
		case '\f': m_Value.ResetToType<FStringView>("\\f"_sv); break;
		case '\n': m_Value.ResetToType<FStringView>("\\n"_sv); break;
		case '\r': m_Value.ResetToType<FStringView>("\\r"_sv); break;
		case '\t': m_Value.ResetToType<FStringView>("\\t"_sv); break;
		case '\v': m_Value.ResetToType<FStringView>("\\v"_sv); break;
		default:
			if (value < ' ')
			{
				FToCharsArgs args;
				args.NumericBase = ENumericBase::Hexadecimal;
				args.PadLeftChar = '0';
				args.PadLeftAmount = 2;

				FStringBuilder builder;
				builder.Append("0x"_sv);
				Private::AppendCharsForUnsignedInt(builder, static_cast<uint64>(value), args);

				m_Value.ResetToType<TUniquePtr<ITypeFormatter>>(MakeUnique<TTypeFormatter<FString>>(builder.ReleaseString()));
			}
			else
			{
				m_Value.ResetToType<char>(value);
			}
			break;
		}
	}

	FStringFormatArgument::FStringFormatArgument(const FString& value)
	{
		m_Value.ResetToType<FStringView>(value.AsStringView());
	}

	// For r-value string references, we need to have a "custom formatter" to hold the temporary value
	FStringFormatArgument::FStringFormatArgument(FString&& value)
	{
		m_Value.ResetToType<TUniquePtr<ITypeFormatter>>(MakeUnique<TTypeFormatter<FString>>(value));
	}

	FStringFormatArgument::FStringFormatArgument(const FStringView value)
	{
		m_Value.ResetToType<FStringView>(value);
	}

	void FStringFormatArgument::BuildString(FStringView formatString, FStringBuilder& builder)
	{
		m_Value.Visit(FVariantVisitor
		{
			[](FEmptyType) {},
			[&](const int64 value)
			{
				const FToCharsArgs args = FToCharsArgs::ParseFormat(formatString);
				Private::AppendCharsForSignedInt(builder, value, args);
			},
			[&](const char value)
			{
				builder.Append(value, 1);
			},
			[&](const uint64 value)
			{
				const FToCharsArgs args = FToCharsArgs::ParseFormat(formatString);
				Private::AppendCharsForUnsignedInt(builder, value, args);
			},
			[&](const float value)
			{
				const FToCharsArgs args = FToCharsArgs::ParseFormat(formatString);
				Private::AppendCharsForFloat(builder, value, args);
			},
			[&](const double value)
			{
				const FToCharsArgs args = FToCharsArgs::ParseFormat(formatString);
				Private::AppendCharsForDouble(builder, value, args);
			},
			[&](const FStringView value)
			{
				const FToCharsArgs args = FToCharsArgs::ParseFormat(formatString);
				Private::AppendStringWithArguments(builder, value, args);
			},
			[&](const void* value)
			{
				const FToCharsArgs args = FToCharsArgs::ParseFormat(formatString);
				Private::AppendCharsForPointer(builder, value, args);
			},
			[&](const bool value)
			{
				const FToCharsArgs args = FToCharsArgs::ParseFormat(formatString);
				Private::AppendStringWithArguments(builder, value ? "true"_sv : "false"_sv, args);
			},
			[&](TUniquePtr<ITypeFormatter>& value)
			{
				if (value->Parse(formatString))
				{
					value->BuildString(builder);
				}
			}
		});
	}
}