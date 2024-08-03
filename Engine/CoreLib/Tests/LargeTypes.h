#pragma once

#include "Engine/Hashing.h"
#include "Meta/TypeInfo.h"

struct FLargeType
{
	static int32 NumDestroyed;

	double Data[32];

	~FLargeType();
};

DECLARE_PRIMITIVE_TYPE_DEFINITION(FLargeType);

inline uint64 GetHashCode(const FLargeType& value)
{
	return HashItems(value.Data);
}

struct FLargerType
{
	static int32 NumDestroyed;

	double Data[64];

	~FLargerType();
};

DECLARE_PRIMITIVE_TYPE_DEFINITION(FLargerType);

inline uint64 GetHashCode(const FLargerType& value)
{
	return HashItems(value.Data);
}

struct FLargestType
{
	static int32 NumDestroyed;

	double Data[128];

	~FLargestType();
};

DECLARE_PRIMITIVE_TYPE_DEFINITION(FLargestType);

inline uint64 GetHashCode(const FLargestType& value)
{
	return HashItems(value.Data);
}