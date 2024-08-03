#pragma once

#include "Templates/IsInt.h"

template<typename ValueType, typename SizeType>
constexpr void Fill(ValueType* values, SizeType numValues, const ValueType& valueToFillWith)
	requires TIsInt<SizeType>::Value
{
	while (numValues > 0)
	{
		*values = valueToFillWith;
		++values;
		--numValues;
	}
}

template<typename IteratorType, typename ValueType>
constexpr void Fill(const IteratorType beginning, const IteratorType ending, const ValueType& valueToFillWith)
{
	IteratorType iter = beginning;
	while (iter != ending)
	{
		*iter = valueToFillWith;
		++iter;
	}
}