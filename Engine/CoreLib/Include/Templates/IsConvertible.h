#pragma once

#include "Templates/AndNotOr.h"
#include "Templates/Declval.h"
#include "Templates/IntegralConstant.h"
#include "Templates/IsSame.h"
#include "Templates/IsVoid.h"

// https://en.cppreference.com/w/cpp/types/is_convertible

namespace Private
{
	template<typename T>
	auto TestReturnable(int) -> decltype(
	    void(static_cast<T(*)()>(nullptr)), FTrueType { }
	);

	template<typename>
	auto TestReturnable(...) -> FFalseType;

	template<class FromType, class ToType>
	auto TestImplicitlyConvertible(int) -> decltype(
	    void(declval<void(&)(ToType)>()(declval<FromType>())), FTrueType { }
	);

	template<typename, typename>
	auto TestImplicitlyConvertible(...) -> FFalseType;
}

template<class FromType, class ToType>
using TIsConvertible = TOr<
	TAnd<
		decltype(Private::TestReturnable<ToType>(0)),
		decltype(Private::TestImplicitlyConvertible<FromType, ToType>(0))
	>,
    TIsSame<FromType, ToType>,
	TAnd<
		TIsVoid<FromType>,
		TIsVoid<ToType>
	>
>;

template<class FromType, class ToType>
inline constexpr bool IsConvertible = TIsConvertible<FromType, ToType>::Value;

// TODO Do we need TIsNothrowConvertible ?