#include "Containers/StringOrStringView.h"

FStringOrStringView::FStringOrStringView(const CharType* value)
	: FStringOrStringView(FStringView { value, FStringView::CharTraitsType::GetNullTerminatedLength(value) })
{
}

FStringOrStringView::FStringOrStringView(const CharType* value, SizeType length)
	: FStringOrStringView(FStringView { value, length })
{
}

FStringOrStringView::FStringOrStringView(SpanType value)
	: FStringOrStringView(FStringView { value })
{
}

FStringOrStringView::FStringOrStringView(const FString& value)
	: m_Value { value }
{
}

FStringOrStringView::FStringOrStringView(FString&& value)
	: m_Value { MoveTemp(value) }
{
}

FStringOrStringView::FStringOrStringView(const FStringView value)
	: m_Value { value }
{
}

FString FStringOrStringView::AsString() const
{
	return FString { GetChars(), Length() };
}

FStringView FStringOrStringView::AsStringView() const
{
	return FStringView { GetChars(), Length() };
}

TSpan<const FStringOrStringView::CharType> FStringOrStringView::AsSpan() const
{
	return SpanType { GetChars(), Length() };
}

const FStringOrStringView::CharType* FStringOrStringView::GetChars() const
{
	return m_Value.Visit<const CharType*>(FVariantVisitor
	{
		[](const auto& value) { return value.GetChars(); }
	});
}

FStringOrStringView::SizeType FStringOrStringView::Length() const
{
	return m_Value.Visit<SizeType>(FVariantVisitor
	{
		[](const auto& value) { return value.Length(); }
	});
}