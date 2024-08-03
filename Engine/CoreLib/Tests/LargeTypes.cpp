#include "LargeTypes.h"

int32 FLargeType::NumDestroyed = 0;

FLargeType::~FLargeType()
{
	++NumDestroyed;
}

DEFINE_PRIMITIVE_TYPE_DEFINITION(FLargeType);

int32 FLargerType::NumDestroyed = 0;

FLargerType::~FLargerType()
{
	++NumDestroyed;
}

DEFINE_PRIMITIVE_TYPE_DEFINITION(FLargerType);

int32 FLargestType::NumDestroyed = 0;

FLargestType::~FLargestType()
{
	++NumDestroyed;
}

DEFINE_PRIMITIVE_TYPE_DEFINITION(FLargestType);