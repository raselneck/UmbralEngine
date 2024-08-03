#include "Math/Math.h"
#include "Memory/Memory.h"
#include <gtest/gtest.h>

class FMemoryFriendClass
{
	friend FMemory;

public:

	double GetValue() const
	{
		return m_Value;
	}

private:

	FMemoryFriendClass() = default;
	~FMemoryFriendClass() = default;

	FMemoryFriendClass(const double value)
		: m_Value { value }
	{
	}

	double m_Value;
};

template<typename SizeType>
static bool IsMemoryZeroed(const void* memory, SizeType numBytes)
{
	const uint8* bytes = reinterpret_cast<const uint8*>(memory);
	while (numBytes > 0)
	{
		if (*bytes != 0)
		{
			return false;
		}

		++bytes;
		--numBytes;
	}
	return true;
}

TEST(MemoryTests, Allocate)
{
	constexpr int32 numBytes = 16;
	void* memory = FMemory::Allocate(numBytes);
	EXPECT_NE(memory, nullptr);
	EXPECT_TRUE(IsMemoryZeroed(memory, numBytes));
	FMemory::Free(memory);
}

TEST(MemoryTests, AllocateObjectNoParams)
{
	FMemoryFriendClass* value = FMemory::AllocateObject<FMemoryFriendClass>();
	EXPECT_NE(value, nullptr);
	EXPECT_TRUE(IsMemoryZeroed(value, sizeof(FMemoryFriendClass)));
	EXPECT_DOUBLE_EQ(value->GetValue(), 0.0);
	FMemory::FreeObject(value);
}

TEST(MemoryTests, AllocateObjectWithParams)
{
	FMemoryFriendClass* value = FMemory::AllocateObject<FMemoryFriendClass>(FMath::Pi);
	EXPECT_NE(value, nullptr);
	EXPECT_FALSE(IsMemoryZeroed(value, sizeof(FMemoryFriendClass)));
	EXPECT_DOUBLE_EQ(value->GetValue(), FMath::Pi);
	FMemory::FreeObject(value);
}