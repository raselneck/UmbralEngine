#include "Containers/String.h"
#include "Containers/Variant.h"
#include "Math/Math.h"
#include <cstdio>
#include <gtest/gtest.h>

TEST(VariantTests, VariadicIndexOfType)
{
	static_assert(TVariadicIndexOfType<int, int, float, FString>::Value == 0, "Int should be index 0");
	static_assert(TVariadicIndexOfType<float, int, float, FString>::Value == 1, "Float should be index 1");
	static_assert(TVariadicIndexOfType<FString, int, float, FString>::Value == 2, "FString should be index 2");
	static_assert(TVariadicIndexOfType<TArray<int>, int, float, FString>::Value == INDEX_NONE, "TArray<int> should have an invalid index");
}

TEST(VariantTests, DefaultConstruct)
{
	TVariant<int, float, FString> value;
	EXPECT_TRUE(value.Is<int>());
	EXPECT_FALSE(value.Is<float>());
	EXPECT_FALSE(value.Is<FString>());
	//EXPECT_FALSE(value.Is<TArray<uint8>>()); // Compile error

	// Default value will be the first type
	EXPECT_NE(value.GetValuePointer<int>(), nullptr);
	EXPECT_EQ(value.GetValuePointer<float>(), nullptr);
	EXPECT_EQ(value.GetValuePointer<FString>(), nullptr);
	//EXPECT_EQ(value.GetValuePointer<TArray<uint8>>(), nullptr); // Compile error

	EXPECT_EQ(value.GetValue<int>(), 0);
}

TEST(VariantTests, CopyConstruct)
{
	TVariant<int> firstValue { 42 };
	TVariant<int> secondValue = firstValue;

	EXPECT_TRUE(firstValue.Is<int>());
	EXPECT_TRUE(secondValue.Is<int>());

	EXPECT_EQ(firstValue.GetValue<int>(), secondValue.GetValue<int>());
}

// TODO MoveConstruct

TEST(VariantTests, ConstructFromString)
{
#if 0
	const FString stringValue = "hello, world!"_s;

	TVariant<int, float, FString> value { stringValue };

	EXPECT_TRUE(value.HasValue());
	EXPECT_FALSE(value.Is<int>());
	EXPECT_FALSE(value.Is<float>());
	EXPECT_TRUE(value.Is<FString>());
	EXPECT_FALSE(value.Is<TArray<uint8>>());

	EXPECT_EQ(value.GetValuePointer<int>(), nullptr);
	EXPECT_EQ(value.GetValuePointer<float>(), nullptr);
	EXPECT_NE(value.GetValuePointer<FString>(), nullptr);
	EXPECT_EQ(value.GetValuePointer<TArray<uint8>>(), nullptr);

	EXPECT_EQ(value.GetValue<FString>(), stringValue);
#endif
}

TEST(VariantTests, Visit)
{
	enum class EVariantType
	{
		Int,
		Double,
		Array
	};

	TVariant<int, double, TArray<char>> variant { TMathConstants<double>::Pi };

	const EVariantType variantType = variant.Visit<EVariantType>(FVariantVisitor
	{
		[](int) { return EVariantType::Int; },
		[](double) { return EVariantType::Double; },
		[](const TArray<char>&) { return EVariantType::Array; }
	});
	EXPECT_EQ(variantType, EVariantType::Double);

	bool visitedAsInt = false;
	bool visitedAsDouble = false;
	bool visitedAsArray = false;
	variant.Visit(FVariantVisitor
	{
		[&visitedAsInt](int) { visitedAsInt = true; },
		[&visitedAsDouble](double) { visitedAsDouble = true; },
		[&visitedAsArray](const TArray<char>&) { visitedAsArray = true; }
	});
	EXPECT_FALSE(visitedAsInt);
	EXPECT_TRUE(visitedAsDouble);
	EXPECT_FALSE(visitedAsArray);

	bool visitedAsGeneric = false;
	variant.Visit([&visitedAsGeneric](auto) { visitedAsGeneric = true; });
	EXPECT_TRUE(visitedAsGeneric);
}