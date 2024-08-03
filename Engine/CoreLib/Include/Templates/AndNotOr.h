#pragma once

#include "Templates/IntegralConstant.h"
#include <type_traits>

// TAnd is implemented just as std::conjunction is implemented in the Microsoft STL
// Search "struct _Conjunction" in https://github.com/microsoft/STL/blob/main/stl/inc/type_traits

namespace Private
{
	template<bool FirstValue, typename FirstCondition, typename... OtherConditions>
	struct TAndImpl
	{
		using Type = FirstCondition;
	};

	template<typename TrueCondition, typename NextCondition, typename... OtherConditions>
	struct TAndImpl<true, TrueCondition, NextCondition, OtherConditions...>
	{
		using Type = typename TAndImpl<NextCondition::Value, NextCondition, OtherConditions...>::Type;
	};
}

template<typename... ConditionTypes>
struct TAnd : FTrueType
{
};

template<typename FirstCondition, typename... OtherConditions>
struct TAnd<FirstCondition, OtherConditions...> : Private::TAndImpl<FirstCondition::Value, FirstCondition, OtherConditions...>::Type
{
};

template<typename... ConditionTypes>
inline constexpr bool And = TAnd<ConditionTypes...>::Value;

// TOr is implemented just as std::disjunction is implemented in the Microsoft STL
// Search "struct _Disjunction" in https://github.com/microsoft/STL/blob/main/stl/inc/xtr1common

namespace Private
{
	template<bool FirstValue, typename FirstCondition, typename... OtherConditions>
	struct TOrImpl
	{
		using Type = FirstCondition;
	};

	// First condition is false, try the next condition
	template<typename FalseCondition, typename NextCondition, typename... OtherConditions>
	struct TOrImpl<false, FalseCondition, NextCondition, OtherConditions...>
	{
		using Type = typename TOrImpl<NextCondition::Value, NextCondition, OtherConditions...>::Type;
	};
}

template<typename... ConditionTypes>
struct TOr : FFalseType
{
};

template<typename FirstCondition, typename... OtherConditions>
struct TOr<FirstCondition, OtherConditions...> : Private::TOrImpl<FirstCondition::Value, FirstCondition, OtherConditions...>::Type
{
};

template<typename... ConditionTypes>
inline constexpr bool Or = TOr<ConditionTypes...>::Value;

// TNot is implemented just as std::negation is implemented in the Microsoft STL
// Search "struct negation" in https://github.com/microsoft/STL/blob/main/stl/inc/xtr1common

template<typename ConditionType>
struct TNot : TBoolConstant<!static_cast<bool>(ConditionType::Value)>
{
};

template<typename ConditionType>
inline constexpr bool Not = TNot<ConditionType>::Value;