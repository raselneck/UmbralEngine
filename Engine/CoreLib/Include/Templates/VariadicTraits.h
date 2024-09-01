#pragma once

#include "Engine/CoreTypes.h"
#include "Engine/IntTypes.h"
#include "Templates/Forward.h"
#include "Templates/IsPointer.h"
#include "Templates/IsSame.h"
#include "Templates/Move.h"

///////////////////////////////////////////////////////////////////////////////
// Querying the index of a type in a variadic type list

namespace Private
{
	template<isize Index, typename TypeToFind, typename... Types>
	struct TGetVariadicIndexOfType;

	template<isize Index, typename TypeToFind>
	struct TGetVariadicIndexOfType<Index, TypeToFind>
	{
		static constexpr isize Value = INDEX_NONE;
	};

	template<isize Index, typename TypeToFind, typename CurrentType, typename... Types>
	struct TGetVariadicIndexOfType<Index, TypeToFind, CurrentType, Types...>
	{
		static constexpr isize Value = TIsSame<TypeToFind, CurrentType>::Value
		                             ? Index
		                             : TGetVariadicIndexOfType<Index + 1, TypeToFind, Types...>::Value;
	};
}

template<typename TypeToFind, typename... Types>
using TVariadicIndexOfType = Private::TGetVariadicIndexOfType<0, TypeToFind, Types...>;

///////////////////////////////////////////////////////////////////////////////
// Querying the type at a given index in a variadic type list

namespace Private
{
	template<isize Index, typename... Types>
	struct TGetVariadicTypeAtIndex;

	template<typename FirstType, typename... Types>
	struct TGetVariadicTypeAtIndex<0, FirstType, Types...>
	{
		using Type = FirstType;
	};

	template<isize Index, typename FirstType, typename... OtherTypes>
	struct TGetVariadicTypeAtIndex<Index, FirstType, OtherTypes...>
	{
		static_assert(Index >= 0, "Invalid index given for variadic type list");

		using Type = typename TGetVariadicTypeAtIndex<Index - 1, OtherTypes...>::Type;
	};
}

template<isize Index, typename... Types>
using TVariadicTypeAtIndex = Private::TGetVariadicTypeAtIndex<Index, Types...>;

///////////////////////////////////////////////////////////////////////////////
// Querying if a variadic type list contains a type

template<typename TypeToFind, typename... Types>
struct TVariadicContainsType
{
	static constexpr bool Value = TVariadicIndexOfType<TypeToFind, Types...>::Value != INDEX_NONE;
};

///////////////////////////////////////////////////////////////////////////////
// Querying if a variadic type list contains a pointer type

template<typename... Types>
using TVariadicContainsPointer = TOr<TIsPointer<Types>...>;

///////////////////////////////////////////////////////////////////////////////
// Querying if a variadic type list contains a reference type

template<typename... Types>
using TVariadicContainsLValueReference = TOr<TIsLValueReference<Types>...>;

template<typename... Types>
using TVariadicContainsRValueReference = TOr<TIsRValueReference<Types>...>;

template<typename... Types>
using TVariadicContainsReference = TOr<TIsReference<Types>...>;

///////////////////////////////////////////////////////////////////////////////
// Querying if a variadic type list contains duplicate types

template<typename... Types>
struct TVariadicContainsDuplicate;

template<typename T>
struct TVariadicContainsDuplicate<T>
{
	static constexpr bool Value = false;
};

template<typename Type, typename... OtherTypes>
struct TVariadicContainsDuplicate<Type, Type, OtherTypes...>
{
	static constexpr bool Value = true;
};

// FIXME: The below specialization may still allow variants such as `TVariant<int, const int, volatile int, const volatile int>'

template<typename FirstType, typename SecondType, typename... OtherTypes>
struct TVariadicContainsDuplicate<FirstType, SecondType, OtherTypes...>
{
	static constexpr bool Value = TOr<
		TVariadicContainsDuplicate<FirstType, OtherTypes...>,
		TVariadicContainsDuplicate<SecondType, OtherTypes...>
	>::Value;
};

///////////////////////////////////////////////////////////////////////////////
// Visiting each type in a variadic pack

template<typename... Types>
struct TVariadicForEach;

template<>
struct TVariadicForEach<>
{
	template<typename CallbackType>
	static void Visit(CallbackType callback)
	{
		(void)callback;
	}
};

template<typename FirstType, typename... OtherTypes>
struct TVariadicForEach<FirstType, OtherTypes...>
{
	template<typename CallbackType>
	static void Visit(CallbackType callback, FirstType firstValue, OtherTypes... otherValues)
	{
		using CallbackReturnType = decltype(callback(MoveTempIfPossible(firstValue)));
		static_assert(IsSame<CallbackReturnType, EIterationDecision>, "Callback must return EIteractionDecision");

		const EIterationDecision decision = callback(MoveTempIfPossible(firstValue));
		if (decision == EIterationDecision::Break)
		{
			return;
		}

		TVariadicForEach<OtherTypes...>::Visit(MoveTemp(callback), Forward<OtherTypes>(otherValues)...);
	}
};