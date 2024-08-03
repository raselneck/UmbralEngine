#pragma once

#include "Engine/IntTypes.h"

template<usize FirstValue, usize... OtherValues>
struct TMinOf;

template<usize InValue>
struct TMinOf<InValue>
{
	static constexpr usize Value = InValue;
};

template<usize FirstValue, usize SecondValue, usize ... OtherValues>
struct TMinOf<FirstValue, SecondValue, OtherValues...>
{
	static constexpr usize Value = FirstValue < SecondValue
	                             ? TMinOf<FirstValue, OtherValues...>::Value
	                             : TMinOf<SecondValue, OtherValues...>::Value;
};

template<usize FirstValue, usize... OtherValues>
struct TMaxOf;

template<usize InValue>
struct TMaxOf<InValue>
{
	static constexpr usize Value = InValue;
};

template<usize FirstValue, usize SecondValue, usize ... OtherValues>
struct TMaxOf<FirstValue, SecondValue, OtherValues...>
{
	static constexpr usize Value = FirstValue >= SecondValue
	                             ? TMaxOf<FirstValue, OtherValues...>::Value
	                             : TMaxOf<SecondValue, OtherValues...>::Value;
};