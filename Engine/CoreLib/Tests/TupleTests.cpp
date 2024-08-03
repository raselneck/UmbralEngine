#include "Containers/StringView.h"
#include "Containers/Tuple.h"
#include "Math/Math.h"
#include "Memory/UniquePtr.h"
#include <gtest/gtest.h>

struct FFoo
{
	static int32 Num;

	FFoo() { ++Num; }
	~FFoo() { --Num; }
};

struct FBar
{
	static int32 Num;

	FBar() { ++Num; }
	~FBar() { --Num; }
};

int32 FFoo::Num = 0;
int32 FBar::Num = 0;

TEST(TupleTests, DefaultConstruct)
{
	TTuple<int, float, int> tuple;
	EXPECT_EQ(Get<0>(tuple), 0);
	EXPECT_FLOAT_EQ(Get<1>(tuple), 0.0f);
	EXPECT_EQ(Get<2>(tuple), 0);
	EXPECT_EQ(sizeof(tuple), sizeof(int) * 2 + sizeof(float));
}

TEST(TupleTests, ValueConstruct)
{
	TTuple<int, FStringView, double> tuple { 42, "hello"_sv, 3.14 };
	EXPECT_EQ(Get<0>(tuple), 42);
	EXPECT_EQ(Get<1>(tuple), "hello"_sv);
	EXPECT_DOUBLE_EQ(Get<2>(tuple), 3.14);
}

TEST(TupleTests, DestructFromStack)
{
	EXPECT_EQ(FFoo::Num, 0);
	EXPECT_EQ(FBar::Num, 0);

	{
		TTuple<FFoo, FBar> tuple;
		EXPECT_EQ(FFoo::Num, 1);
		EXPECT_EQ(FBar::Num, 1);
	}

	EXPECT_EQ(FFoo::Num, 0);
	EXPECT_EQ(FBar::Num, 0);
}

TEST(TupleTests, DestructFromHeap)
{
	EXPECT_EQ(FFoo::Num, 0);
	EXPECT_EQ(FBar::Num, 0);

	{
		TUniquePtr<TTuple<FFoo, FBar>> tuple = MakeUnique<TTuple<FFoo, FBar>>();
		EXPECT_EQ(FFoo::Num, 1);
		EXPECT_EQ(FBar::Num, 1);
	}

	EXPECT_EQ(FFoo::Num, 0);
	EXPECT_EQ(FBar::Num, 0);
}

TEST(TupleTests, Visit)
{
	using FTupleVector3 = TTuple<float, float, float>;

	FTupleVector3 value { 1.0f, 2.0f, 3.0f };

	float lengthSquared = 0.0f;
	value.Visit(FTupleVisitor
	{
		[&lengthSquared](float value) { lengthSquared += value * value; }
	});

	EXPECT_FLOAT_EQ(lengthSquared, 14.0f);
}