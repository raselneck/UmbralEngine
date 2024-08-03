#include "Engine/Internationalization.h"
#include <gtest/gtest.h>

// Tests are based on examples in https://mcilloni.ovh/2023/07/23/unicode-is-hard/

static const FStringView CiaoLowercase = "ciao"_sv;
static const FStringView CiaoUppercase = "CIAO"_sv;
static const FStringView Tuck = "tuck"_sv;
static const FStringView Lowe = "löwe"_sv;
static const FStringView Luck = "luck"_sv;
static const FStringView Zebra = "zebra"_sv;

static bool StringsMatchWithCulture(const FStringView first, const FStringView second)
{
	return first.Compare(second, EStringComparison::CurrentCulture) == ECompareResult::Equals;
}

TEST(InternationalizationTests, EstonianSorting)
{
	TErrorOr<void> setLocaleResult = FInternationalization::SetLocale("et_EE"_sv);
	EXPECT_FALSE(setLocaleResult.IsError());

	TArray<FStringView> strings {{ Tuck, Lowe, Luck, Zebra }};
	strings.Sort();

	EXPECT_PRED2(StringsMatchWithCulture, strings[0], Luck);
	EXPECT_PRED2(StringsMatchWithCulture, strings[1], Lowe);
	EXPECT_PRED2(StringsMatchWithCulture, strings[2], Zebra);
	EXPECT_PRED2(StringsMatchWithCulture, strings[3], Tuck);
}

TEST(InternationalizationTests, GermanSorting)
{
	TErrorOr<void> setLocaleResult = FInternationalization::SetLocale("de_DE"_sv);
	EXPECT_FALSE(setLocaleResult.IsError());

	TArray<FStringView> strings {{ Tuck, Lowe, Luck, Zebra }};
	strings.Sort();

	EXPECT_PRED2(StringsMatchWithCulture, strings[0], Lowe);
	EXPECT_PRED2(StringsMatchWithCulture, strings[1], Luck);
	EXPECT_PRED2(StringsMatchWithCulture, strings[2], Tuck);
	EXPECT_PRED2(StringsMatchWithCulture, strings[3], Zebra);
}

TEST(InternationalizationTests, SwedishSorting)
{
	TErrorOr<void> setLocaleResult = FInternationalization::SetLocale("sv_SE"_sv);
	EXPECT_FALSE(setLocaleResult.IsError());

	TArray<FStringView> strings {{ Tuck, Lowe, Luck, Zebra }};
	strings.Sort();

	EXPECT_PRED2(StringsMatchWithCulture, strings[0], Luck);
	EXPECT_PRED2(StringsMatchWithCulture, strings[1], Lowe);
	EXPECT_PRED2(StringsMatchWithCulture, strings[2], Tuck);
	EXPECT_PRED2(StringsMatchWithCulture, strings[3], Zebra);
}

TEST(InternationalizationTests, TurkishToLower)
{
	TErrorOr<void> setLocaleResult = FInternationalization::SetLocale("tr_TR.UTF8"_sv);
	EXPECT_FALSE(setLocaleResult.IsError());

	const TArray<char> genericAsLowerChars = FInternationalization::ConvertStringToLower(CiaoUppercase);
	const FStringView genericAsLower { genericAsLowerChars.AsSpan() };
	EXPECT_PRED2(StringsMatchWithCulture, genericAsLower, "cıao"_sv);
}

TEST(InternationalizationTests, TurkishToUpper)
{
	TErrorOr<void> setLocaleResult = FInternationalization::SetLocale("tr_TR.UTF8"_sv);
	EXPECT_FALSE(setLocaleResult.IsError());

	const TArray<char> genericAsUpperChars = FInternationalization::ConvertStringToUpper(CiaoLowercase);
	const FStringView genericAsUpper { genericAsUpperChars.AsSpan() };
	EXPECT_PRED2(StringsMatchWithCulture, genericAsUpper, "CİAO"_sv);
}