#pragma once

#include "Engine/IntTypes.h"
#include "Templates/IsInt.h"
#include <functional>
#include <type_traits>

template<typename IntegerType, IntegerType... Values>
struct TIntegerSequence
{
	static_assert(TIsInt<IntegerType>::Value, "TIntegerSequence must be used with integer types");

	using ValueType = IntegerType;

	static constexpr int32 Num = static_cast<int32>(sizeof...(Values));
};

template<int32... Values>
using TIndexSequence = TIntegerSequence<int32, Values...>;

namespace Private
{
	// TODO Would be neat if we could figure out how to use recursive templates to define these

	template<typename IntegerType, int32 Num>
	struct TMakeIntegerSequence;

	// Zero is a special case
	template<typename IntegerType>
	struct TMakeIntegerSequence<IntegerType, 0>
	{
		using Type = TIntegerSequence<IntegerType>;
	};

#define DEFINE_INTEGER_SEQUENCE(Count, ...)                             \
	template<typename IntegerType>                                      \
	struct TMakeIntegerSequence<IntegerType, Count>                     \
	{                                                                   \
		using Type = TIntegerSequence<IntegerType, __VA_ARGS__>;        \
	}

	DEFINE_INTEGER_SEQUENCE( 1, 0);
	DEFINE_INTEGER_SEQUENCE( 2, 0, 1);
	DEFINE_INTEGER_SEQUENCE( 3, 0, 1, 2);
	DEFINE_INTEGER_SEQUENCE( 4, 0, 1, 2, 3);
	DEFINE_INTEGER_SEQUENCE( 5, 0, 1, 2, 3, 4);
	DEFINE_INTEGER_SEQUENCE( 6, 0, 1, 2, 3, 4, 5);
	DEFINE_INTEGER_SEQUENCE( 7, 0, 1, 2, 3, 4, 5, 6);
	DEFINE_INTEGER_SEQUENCE( 8, 0, 1, 2, 3, 4, 5, 6, 7);
	DEFINE_INTEGER_SEQUENCE( 9, 0, 1, 2, 3, 4, 5, 6, 7, 8);
	DEFINE_INTEGER_SEQUENCE(10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
	DEFINE_INTEGER_SEQUENCE(11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	DEFINE_INTEGER_SEQUENCE(12, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
	DEFINE_INTEGER_SEQUENCE(13, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
	DEFINE_INTEGER_SEQUENCE(14, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
	DEFINE_INTEGER_SEQUENCE(15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
	DEFINE_INTEGER_SEQUENCE(16, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

#undef DEFINE_INTEGER_SEQUENCE
}

template<typename IntegerType, int32 Count>
using TMakeIntegerSequence = Private::TMakeIntegerSequence<IntegerType, Count>;

template<int32 Num>
using TMakeIndexSequence = TMakeIntegerSequence<int32, Num>;

template<typename... Types>
using TIndexSequenceFor = TMakeIndexSequence<static_cast<int32>(sizeof...(Types))>;