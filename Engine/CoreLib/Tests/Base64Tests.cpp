#include "Engine/Logging.h"
#include "Misc/Base64.h"
#include <gtest/gtest.h>

TEST(Base64Tests, EncodeMinimumViableString)
{
	const uint8 byte = 0;
	const FString encodedByte = Base64::Encode({ &byte, 1 });
	EXPECT_EQ(encodedByte, "AA=="_sv);
}

TEST(Base64Tests, EncodeNoPadding)
{
	constexpr FStringView originalString = "Many hands make light work."_sv;

	const TSpan<const uint8> originalBytes = originalString.AsByteSpan();
	const FString encodedString = Base64::Encode(originalBytes);

	EXPECT_EQ(encodedString, "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"_sv);
}

TEST(Base64Tests, DecodeMinimumViableString)
{
	constexpr FStringView encodedString = "AA=="_sv;

	TErrorOr<TArray<uint8>> decodeResult = Base64::Decode(encodedString);
	if (decodeResult.IsError())
	{
		UM_LOG(Error, "[{}] Base64 decode error: {}", test_info_->name(), decodeResult.GetError());
	}
	ASSERT_FALSE(decodeResult.IsError());

	const TArray<uint8> originalBytes = decodeResult.ReleaseValue();
	ASSERT_EQ(originalBytes.Num(), 1);
	ASSERT_EQ(originalBytes[0], 0);
}

TEST(Base64Tests, DecodeNoPadding)
{
	constexpr FStringView encodedString = "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu"_sv;

	FString originalString;
	ASSERT_TRUE(Base64::Decode(encodedString, originalString));
	EXPECT_EQ(originalString, "Many hands make light work."_sv);
}
