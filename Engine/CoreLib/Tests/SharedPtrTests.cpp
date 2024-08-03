#include "Memory/SharedPtr.h"
#include <gtest/gtest.h>

class FLambdaBase
{
public:

	virtual ~FLambdaBase() = default;
	virtual void Evaluate() { }
};

template<typename LambdaType>
class TLambdaOnDestruct : public FLambdaBase
{
public:

	TLambdaOnDestruct(LambdaType lambda)
		: m_Lambda { MoveTemp(lambda) }
	{
	}

	~TLambdaOnDestruct() override
	{
		m_Lambda();
	}

private:

	LambdaType m_Lambda;
};

template<typename LambdaType>
class TLambdaOnEvaluate : public FLambdaBase
{
public:

	TLambdaOnEvaluate(LambdaType lambda)
		: m_Lambda { MoveTemp(lambda) }
	{
	}

	~TLambdaOnEvaluate() override = default;

	void Evaluate() override
	{
		m_Lambda();
	}

private:

	LambdaType m_Lambda;
};

TEST(SharedPtrTests, DefaultConstruct)
{
	TSharedPtr<int32> ptr;
	EXPECT_FALSE(ptr.IsValid());
	EXPECT_TRUE(ptr.IsNull());
	EXPECT_EQ(ptr.Get(), nullptr);
}

TEST(SharedPtrTests, CopyConstruct)
{
	const TSharedPtr<int32> value1 = MakeShared<int32>(42);
	const TSharedPtr<int32> value2 = value1;

	EXPECT_TRUE(value1.IsValid());
	EXPECT_TRUE(value2.IsValid());

	EXPECT_FALSE(value1.IsNull());
	EXPECT_FALSE(value2.IsNull());

	EXPECT_EQ(*value1, *value2);

	EXPECT_EQ(value1.GetStrongRefCount(), 2);
	EXPECT_EQ(value2.GetStrongRefCount(), 2);
}

TEST(SharedPtrTests, MakeSharedInt)
{
	TSharedPtr<int32> ptr = MakeShared<int32>(42);
	EXPECT_TRUE(ptr.IsValid());
	EXPECT_FALSE(ptr.IsNull());
	EXPECT_NE(ptr.Get(), nullptr);
	EXPECT_EQ(*ptr, 42);
}

TEST(SharedPtrTests, MakeSharedClassAccess)
{
	bool methodWasCalled = false;

	auto SetMethodWasCalledFlag = [&] { methodWasCalled = true; };

	using FLambdaClass = TLambdaOnEvaluate<decltype(SetMethodWasCalledFlag)>;
	TSharedPtr<FLambdaBase> ptr = MakeShared<FLambdaClass>(MoveTemp(SetMethodWasCalledFlag));

	EXPECT_TRUE(ptr.IsValid());
	EXPECT_FALSE(ptr.IsNull());
	EXPECT_NE(ptr.Get(), nullptr);

	ptr->Evaluate();
	EXPECT_TRUE(methodWasCalled);
}

TEST(SharedPtrTests, MakeSharedClassDestruct)
{
	bool classWasDestroyed = false;

	{
		auto SetClassWasDestroyedFlag = [&] { classWasDestroyed = true; };

		using FLambdaClass = TLambdaOnDestruct<decltype(SetClassWasDestroyedFlag)>;
		TSharedPtr<FLambdaBase> ptr = MakeShared<FLambdaClass>(MoveTemp(SetClassWasDestroyedFlag));

		EXPECT_TRUE(ptr.IsValid());
		EXPECT_FALSE(ptr.IsNull());
		EXPECT_NE(ptr.Get(), nullptr);
	}

	EXPECT_TRUE(classWasDestroyed);
}

TEST(SharedPtrTests, Reset)
{
	bool classWasDestroyed = false;

	auto SetClassWasDestroyedFlag = [&] { classWasDestroyed = true; };

	using FLambdaClass = TLambdaOnDestruct<decltype(SetClassWasDestroyedFlag)>;
	TSharedPtr<FLambdaClass> ptr = MakeShared<FLambdaClass>(MoveTemp(SetClassWasDestroyedFlag));

	EXPECT_TRUE(ptr.IsValid());
	EXPECT_FALSE(ptr.IsNull());
	EXPECT_NE(ptr.Get(), nullptr);

	ptr.Reset();
	EXPECT_TRUE(classWasDestroyed);
}