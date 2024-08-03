#include "Containers/Array.h"
#include "Containers/InternalString.h"
#include "Containers/StringView.h"
#include "Engine/Internationalization.h"
#include "Engine/Logging.h"
#include "Misc/CString.h"

ECompareResult FStringView::Compare(const FStringView other, const EStringComparison comparison) const
{
	TErrorOr<ECompareResult> result = FInternationalization::CompareStrings(AsSpan(), other.AsSpan(), comparison);

	if (result.IsError())
	{
		UM_LOG(Error, "{}", result.ReleaseError());
		return ECompareResult::LessThan;
	}

	return result.ReleaseValue();
}

bool FStringView::EndsWith(const FStringView other, EIgnoreCase ignoreCase) const
{
	if (other.Length() > Length())
	{
		return false;
	}

	return Right(other.Length()).Equals(other, ignoreCase);
}

FStringView::SizeType FStringView::IndexOf(const CharType value, const SizeType startIndex, const SizeType count) const
{
	using UnsignedSizeType = MakeUnsigned<SizeType>;

	if (static_cast<UnsignedSizeType>(startIndex) > static_cast<UnsignedSizeType>(Length()) ||
	    static_cast<UnsignedSizeType>(count) > static_cast<UnsignedSizeType>(Length() - startIndex))
	{
		return INDEX_NONE;
	}

	for (SizeType idx = startIndex; idx < startIndex + count; ++idx)
	{
		if (At(idx) == value)
		{
			return idx;
		}
	}

	return INDEX_NONE;
}

FStringView::SizeType FStringView::IndexOf(const FStringView value, const SizeType startIndex, const SizeType count, const EStringComparison comparison) const
{
	using UnsignedSizeType = MakeUnsigned<SizeType>;

	// TODO Need to account for `value` length here too
	if (static_cast<UnsignedSizeType>(startIndex) > static_cast<UnsignedSizeType>(Length()) ||
	    static_cast<UnsignedSizeType>(count) > static_cast<UnsignedSizeType>(Length() - startIndex))
	{
		return INDEX_NONE;
	}

	switch (comparison)
	{
	case EStringComparison::CurrentCulture:
	case EStringComparison::CurrentCultureIgnoreCase:
		UM_ASSERT_NOT_REACHED_MSG("FStringView::IndexOf using current culture is not yet implemented");

	case EStringComparison::InvariantCulture:
	case EStringComparison::InvariantCultureIgnoreCase:
		UM_ASSERT_NOT_REACHED_MSG("FStringView::IndexOf using invariant culture is not yet implemented");

	case EStringComparison::Ordinal:
	case EStringComparison::OrdinalIgnoreCase:
	{
		const CharType* haystack = GetChars() + startIndex;
		const SizeType haystackLength = count;
		const CharType* needle = value.GetChars();
		const SizeType needleLength = value.Length();

		const SizeType foundIndex = comparison == EStringComparison::OrdinalIgnoreCase
		                          ? FCString::StrCaseStr(haystack, haystackLength, needle, needleLength)
		                          : FCString::StrStr(haystack, haystackLength, needle, needleLength);
		return foundIndex == INDEX_NONE ? INDEX_NONE : foundIndex + startIndex;
	}

	default:
		UM_ASSERT_NOT_REACHED();
	}
}

void FStringView::SplitByChars(const FStringView chars, const EStringSplitOptions options, TArray<FStringView>& result) const
{
	if (IsEmpty())
	{
		return;
	}

	if (chars.IsEmpty())
	{
		result.Add(*this);
	}

	Private::SplitString<FStringView>(*this, options, chars, result,
	                                  Private::GetNextIndexOfClosestCharToSplitOn<FStringView>);
}

TArray<FStringView> FStringView::SplitByChars(const FStringView chars, const EStringSplitOptions options) const
{
	TArray<FStringView> result;
	SplitByChars(chars, options, result);
	return result;
}

void FStringView::SplitByString(const FStringView substring, const EStringSplitOptions options, TArray<FStringView>& result) const
{
	if (IsEmpty())
	{
		return;
	}

	if (substring.IsEmpty())
	{
		result.Add(*this);
	}

	Private::SplitString<FStringView, FStringView>(*this, options, substring, result,
	                                               Private::GetNextIndexOfSubstringToSplitOn<FStringView>);
}

TArray<FStringView> FStringView::SplitByString(const FStringView substring, const EStringSplitOptions options) const
{
	TArray<FStringView> result;
	SplitByString(substring, options, result);
	return result;
}

bool FStringView::StartsWith(const FStringView other, const EIgnoreCase ignoreCase) const
{
	if (other.Length() > Length())
	{
		return false;
	}

	return Left(other.Length()).Equals(other, ignoreCase);
}