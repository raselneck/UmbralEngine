#pragma once

#include "Templates/AndNotOr.h"
#include "Templates/TypeIdentity.h"

// https://en.cppreference.com/w/cpp/types/is_reference

template<typename T> struct TIsLValueReference     : FFalseType { };
template<typename T> struct TIsLValueReference<T&> : FTrueType  { };

template<typename T> inline constexpr bool IsLValueReference = TIsLValueReference<T>::Value;

template<typename T> struct TIsRValueReference      : FFalseType { };
template<typename T> struct TIsRValueReference<T&&> : FTrueType  { };

template<typename T> inline constexpr bool IsRValueReference = TIsRValueReference<T>::Value;

template<typename T>
using TIsReference = TOr<TIsLValueReference<T>, TIsRValueReference<T>>;

template<typename T> inline constexpr bool IsReference = TIsReference<T>::Value;

// https://en.cppreference.com/w/cpp/types/add_reference

namespace Private
{
	template<class T> auto TryAddLValueReference(int) -> TTypeIdentity<T&>;
	template<class T> auto TryAddLValueReference(...) -> TTypeIdentity<T>;

	template<class T> auto TryAddRValueReference(int) -> TTypeIdentity<T&&>;
	template<class T> auto TryAddRValueReference(...) -> TTypeIdentity<T>;
}

template<typename T> struct TAddLValueReference : decltype(Private::TryAddLValueReference<T>(0)) { };
template<typename T> struct TAddRValueReference : decltype(Private::TryAddRValueReference<T>(0)) { };

template<typename T> using AddLValueReference = typename TAddLValueReference<T>::Type;
template<typename T> using AddRValueReference = typename TAddRValueReference<T>::Type;

// https://en.cppreference.com/w/cpp/types/remove_reference

template<typename T> struct TRemoveReference      { using Type = T; };
template<typename T> struct TRemoveReference<T&>  { using Type = T; };
template<typename T> struct TRemoveReference<T&&> { using Type = T; };

template<typename T> using RemoveReference = typename TRemoveReference<T>::Type;

template<typename T> struct TRemoveConstReference            { using Type = T; };
template<typename T> struct TRemoveConstReference<T&>        { using Type = T; };
template<typename T> struct TRemoveConstReference<T&&>       { using Type = T; };
template<typename T> struct TRemoveConstReference<const T&>  { using Type = T; };
template<typename T> struct TRemoveConstReference<const T&&> { using Type = T; };

template<typename T> using RemoveConstReference = typename TRemoveConstReference<T>::Type;