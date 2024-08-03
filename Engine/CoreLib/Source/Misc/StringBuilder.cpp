#include "Misc/StringBuilder.h"
#include "Containers/InternalString.h"

FStringBuilder::CharType* FStringBuilder::AddZeroed(const SizeType numChars)
{
	if (numChars <= 0)
	{
		return nullptr;
	}

	const SizeType index = m_Chars.AddZeroed(numChars);
	return m_Chars.GetData() + index;
}

FStringBuilder& FStringBuilder::Append(const FString& string)
{
	m_Chars.Append(string.AsSpan());
	return *this;
}

FStringBuilder& FStringBuilder::Append(const FStringView stringView)
{
	m_Chars.Append(stringView);
	return *this;
}

FStringBuilder& FStringBuilder::Append(const CharType* chars, const SizeType numChars)
{
	m_Chars.Append(chars, numChars);
	return *this;
}

FStringBuilder& FStringBuilder::Append(const CharType ch, SizeType numChars)
{
	// TODO Add an overload to TArray to do this in bulk

	int32 index = m_Chars.AddZeroed(numChars);
	while (numChars > 0)
	{
		m_Chars[index] = ch;
		++index;
		--numChars;
	}

	return *this;
}

FStringBuilder& FStringBuilder::Append(const float value, TOptional<int32> numDecimals)
{
	Private::AppendCharsForFloat(*this, value, FToCharsArgs::WithDecimals(MoveTemp(numDecimals)));
	return *this;
}

FStringBuilder& FStringBuilder::Append(const double value, TOptional<int32> numDecimals)
{
	Private::AppendCharsForDouble(*this, value, FToCharsArgs::WithDecimals(MoveTemp(numDecimals)));
	return *this;
}

FStringBuilder& FStringBuilder::Append(const int64 value, const ENumericBase base)
{
	Private::AppendCharsForSignedInt(*this, value, FToCharsArgs::WithNumericBase(base));
	return *this;
}

FStringBuilder& FStringBuilder::Append(const uint64 value, const ENumericBase base)
{
	Private::AppendCharsForUnsignedInt(*this, value, FToCharsArgs::WithNumericBase(base));
	return *this;
}

FStringView FStringBuilder::AsStringView() const
{
	if (m_Chars.IsEmpty())
	{
		return {};
	}

	return FStringView { m_Chars.GetData(), m_Chars.Num() };
}

FString FStringBuilder::ReleaseString()
{
	if (m_Chars.IsEmpty() == false && m_Chars.Last() != TCharTraits<CharType>::NullChar)
	{
		m_Chars.Add(TCharTraits<CharType>::NullChar);
	}

	return FString { {}, MoveTemp(m_Chars) };
}

FStringBuilder& FStringBuilder::Reserve(const SizeType reserveAmount)
{
	m_Chars.Reserve(reserveAmount);
	return *this;
}

FStringBuilder& FStringBuilder::AppendFormattedString(const FStringView formatString, const TSpan<Private::FStringFormatArgument> formatArgs)
{
	Private::AppendFormattedString(*this, formatString, formatArgs);
	return *this;
}