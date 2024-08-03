#pragma once

// https://en.cppreference.com/w/cpp/types/conditional

template<bool Condition, typename TrueType, typename FalseType>
struct TSelectType
{
	using Type = TrueType;
};

template<typename TrueType, typename FalseType>
struct TSelectType<false, TrueType, FalseType>
{
	using Type = FalseType;
};

template<bool Condition, typename TrueType, typename FalseType>
using SelectType = typename TSelectType<Condition, TrueType, FalseType>::Type;

template<bool Condition, auto TrueValue, auto FalseValue>
struct TSelectValue
{
	static constexpr decltype(auto) Value = TrueValue;
};

template<auto TrueValue, auto FalseValue>
struct TSelectValue<false, TrueValue, FalseValue>
{
	static constexpr decltype(auto) Value = FalseValue;
};

template<bool Condition, auto TrueValue, auto FalseValue>
[[maybe_unused]] inline constexpr auto SelectValue = TSelectValue<Condition, TrueValue, FalseValue>::Value;