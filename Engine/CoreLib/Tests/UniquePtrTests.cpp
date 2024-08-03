#include "Memory/UniquePtr.h"
#include <gtest/gtest.h>

enum class EType
{
	Base,
	Derived
};

class FBase
{
public:
	virtual ~FBase() = default;

	[[nodiscard]] virtual EType GetType() const
	{
		return EType::Base;
	}
};

class FDerived : public FBase
{
public:
	virtual ~FDerived() override = default;

	[[nodiscard]] virtual EType GetType() const override
	{
		return EType::Derived;
	}
};

TEST(UniquePtrTests, DefaultConstruct)
{
	TUniquePtr<int> pointer;
	EXPECT_FALSE(pointer.IsValid());
	EXPECT_EQ(pointer.Get(), nullptr);
}

TEST(UniquePtrTests, MakeUnique)
{
	TUniquePtr<int> pointer = MakeUnique<int>(42);
	EXPECT_TRUE(pointer.IsValid());
	EXPECT_NE(pointer.Get(), nullptr);
	EXPECT_EQ(*pointer, 42);
}

TEST(UniquePtrTests, MoveConstruct)
{
	TUniquePtr<FDerived> derived = MakeUnique<FDerived>();
	TUniquePtr<FBase> base = MoveTemp(derived);

	EXPECT_FALSE(derived.IsValid());
	EXPECT_EQ(derived.Get(), nullptr);

	EXPECT_TRUE(base.IsValid());
	EXPECT_NE(base.Get(), nullptr);
	EXPECT_EQ(base->GetType(), EType::Derived);
}