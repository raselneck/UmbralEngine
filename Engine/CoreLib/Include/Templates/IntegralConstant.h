#pragma once

template<typename T, T InValue>
struct TIntegralConstant
{
	using Type = T;
	static constexpr Type Value = InValue;

	[[nodiscard]] constexpr operator T() const
	{
		return Value;
	}
};

template<bool Condition>
using TBoolConstant = TIntegralConstant<bool, Condition>;

using FFalseType = TBoolConstant<false>;
using FTrueType = TBoolConstant<true>;