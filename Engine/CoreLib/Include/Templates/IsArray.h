#pragma once

#include "Engine/IntTypes.h"
#include "Templates/IntegralConstant.h"

// https://en.cppreference.com/w/cpp/types/is_array

template<typename T> struct TIsArray : FFalseType { };
template<typename T> struct TIsArray<T[]> : FTrueType { };
template<typename T, usize N> struct TIsArray<T[N]> : FTrueType { };

template<typename T>
inline constexpr bool IsArray = TIsArray<T>::Value;

// https://en.cppreference.com/w/cpp/types/rank

template<typename T> struct TArrayRank : TIntegralConstant<usize, 0> { };
template<typename T> struct TArrayRank<T[]> : TIntegralConstant<usize, TArrayRank<T>::Value + 1> { };
template<typename T, usize N> struct TArrayRank<T[N]> : TIntegralConstant<usize, TArrayRank<T>::Value + 1> { };

template<typename T>
inline constexpr usize ArrayRank = TArrayRank<T>::Value;

// https://en.cppreference.com/w/cpp/types/extent

template<typename T, usize N = 0>
struct TArrayExtent : TIntegralConstant<usize, 0> { };

template<typename T>
struct TArrayExtent<T[], 0> : TIntegralConstant<usize, 0> { };

template<typename T, usize N>
struct TArrayExtent<T[], N> : TArrayExtent<T, N - 1> { };

template<typename T, usize I>
struct TArrayExtent<T[I], 0> : TIntegralConstant<usize, I> { };

template<typename T, usize I, usize N>
struct TArrayExtent<T[I], N> : TArrayExtent<T, N - 1> { };

template<typename T>
inline constexpr usize ArrayExtent = TArrayExtent<T>::Value;

// https://en.cppreference.com/w/cpp/types/remove_extent

template<typename T> struct TRemoveExtent { using Type = T; };
template<typename T> struct TRemoveExtent<T[]> { using Type = T; };
template<typename T, usize N> struct TRemoveExtent<T[N]> { using Type = T; };

template<typename T>
using RemoveExtent = typename TRemoveExtent<T>::Type;