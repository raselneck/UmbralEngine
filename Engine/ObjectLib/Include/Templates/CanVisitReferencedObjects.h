#pragma once

#include "Templates/IntegralConstant.h"
#include "Templates/IsSame.h"

class FObjectHeapVisitor;

namespace Private
{
	FObjectHeapVisitor& GetNonModifyingHeapVisitor();
}

template<typename T>
concept CanVisitReferencedObjects = requires(T value)
{
	{ value.VisitReferencedObjects(Private::GetNonModifyingHeapVisitor()) } -> SameAs<void>;
};

template<typename T>
struct TCanVisitReferencedObjects
{
	static constexpr bool Value = CanVisitReferencedObjects<T>;
};