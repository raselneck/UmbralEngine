#include "Math/Math.h"
#include <gtest/gtest.h>
#include <cmath>

TEST(MathTests, CeilFloatPositive)
{
	const float value = 5.4f;
	const float expectedResult = ::ceilf(value);
	const float result = FMath::Ceil(value);

	EXPECT_FLOAT_EQ(result, expectedResult);
}

TEST(MathTests, CeilFloatNegative)
{
	const float value = -5.4f;
	const float expectedResult = ::ceilf(value);
	const float result = FMath::Ceil(value);

	EXPECT_FLOAT_EQ(result, expectedResult);
}

TEST(MathTests, CeilDoublePositive)
{
	const double value = 5.4;
	const double expectedResult = ::ceil(value);
	const double result = FMath::Ceil(value);

	EXPECT_DOUBLE_EQ(result, expectedResult);
}

TEST(MathTests, CeilDoubleNegative)
{
	const double value = -5.4;
	const double expectedResult = ::ceil(value);
	const double result = FMath::Ceil(value);

	EXPECT_DOUBLE_EQ(result, expectedResult);
}

TEST(MathTests, FloorFloatPositive)
{
	const float value = 5.4f;
	const float expectedResult = ::floorf(value);
	const float result = FMath::Floor(value);

	EXPECT_FLOAT_EQ(result, expectedResult);
}

TEST(MathTests, FloorFloatNegative)
{
	const float value = -5.4f;
	const float expectedResult = ::floorf(value);
	const float result = FMath::Floor(value);

	EXPECT_FLOAT_EQ(result, expectedResult);
}

TEST(MathTests, FloorDoublePositive)
{
	const double value = 5.4;
	const double expectedResult = ::floor(value);
	const double result = FMath::Floor(value);

	EXPECT_DOUBLE_EQ(result, expectedResult);
}

TEST(MathTests, FloorDoubleNegative)
{
	const double value = -5.4;
	const double expectedResult = ::floor(value);
	const double result = FMath::Floor(value);

	EXPECT_DOUBLE_EQ(result, expectedResult);
}

TEST(MathTests, RoundFloatDownPositive)
{
	const float value = 5.5f;
	const float expectedResult = ::roundf(value);
	const float result = FMath::Round(value);

	EXPECT_FLOAT_EQ(result, expectedResult);
}

TEST(MathTests, RoundFloatUpPositive)
{
	const float value = 5.4f;
	const float expectedResult = ::roundf(value);
	const float result = FMath::Round(value);

	EXPECT_FLOAT_EQ(result, expectedResult);
}

TEST(MathTests, RoundFloatDownNegative)
{
	const float value = -5.5f;
	const float expectedResult = ::roundf(value);
	const float result = FMath::Round(value);

	EXPECT_FLOAT_EQ(result, expectedResult);
}

TEST(MathTests, RoundFloatUpNegative)
{
	const float value = -5.4f;
	const float expectedResult = ::roundf(value);
	const float result = FMath::Round(value);

	EXPECT_FLOAT_EQ(result, expectedResult);
}

TEST(MathTests, RoundDoubleDownNegative)
{
	const double value = -5.5;
	const double expectedResult = ::round(value);
	const double result = FMath::Round(value);

	EXPECT_DOUBLE_EQ(result, expectedResult);
}

TEST(MathTests, RoundDoubleUpNegative)
{
	const double value = -5.4;
	const double expectedResult = ::round(value);
	const double result = FMath::Round(value);

	EXPECT_DOUBLE_EQ(result, expectedResult);
}

TEST(MathTests, SqrtFloat)
{
	const float value = 1.2345f;
	const float expectedResult = ::sqrtf(value);
	const float result = FMath::Sqrt(value);

	EXPECT_FLOAT_EQ(result, expectedResult);
}

TEST(MathTests, SqrtDouble)
{
	const double value = 5.4321;
	const double expectedResult = ::sqrt(value);
	const double result = FMath::Sqrt(value);

	EXPECT_DOUBLE_EQ(result, expectedResult);
}