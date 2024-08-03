#include "Containers/StringReference.h"

FStringReference FStringReference::FromString(const FString& value)
{
	// TODO Maybe passing a const string reference should copy the string?

	FStringReference result;
	result.m_Value.ResetToType<FStringView>(value.AsStringView());
	return result;
}

FStringReference FStringReference::FromString(FString&& value)
{
	FStringReference result;
	result.m_Value.ResetToType<FString>(MoveTemp(value));
	return result;
}

FStringReference FStringReference::FromStringView(FStringView value)
{
	FStringReference result;
	result.m_Value.ResetToType<FStringView>(MoveTemp(value));
	return result;
}

const FStringReference::CharType* FStringReference::GetChars() const
{
	return m_Value.Visit<const CharType*>(FVariantVisitor
	{
		[](const auto& value) { return value.GetChars(); }
	});
}

FStringReference::SizeType FStringReference::Length() const
{
	return m_Value.Visit<SizeType>(FVariantVisitor
	{
		[](const auto& value) { return value.Length(); }
	});
}

FString FStringReference::ToString() const
{
	return FString { GetChars(), Length() };
}

FStringReference::operator FStringView() const
{
	return FStringView { GetChars(), Length() };
}

FStringReference::operator SpanType() const
{
	return SpanType { GetChars(), Length() };
}