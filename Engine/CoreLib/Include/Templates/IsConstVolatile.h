#pragma once

#include "Templates/IntegralConstant.h"
#include "Templates/IsReference.h"
#include "Templates/Select.h"
#include "Templates/TypeIdentity.h"

// https://en.cppreference.com/w/cpp/types/is_const

template<typename T> struct TIsConst          : FFalseType { };
template<typename T> struct TIsConst<const T> : FTrueType  { };

template<typename T> inline constexpr bool IsConst = TIsConst<T>::Value;

// https://en.cppreference.com/w/cpp/types/is_volatile

template<typename T> struct TIsVolatile             : FFalseType { };
template<typename T> struct TIsVolatile<volatile T> : FTrueType  { };

template<typename T> inline constexpr bool IsVolatile = TIsVolatile<T>::Value;

// https://en.cppreference.com/w/cpp/types/add_cv

template<typename T> using TAddConst = TTypeIdentity<const T>;
template<typename T> using TAddVolatile = TTypeIdentity<volatile T>;
template<typename T> using TAddConstVolatile = TTypeIdentity<const volatile T>;

template<typename T> using AddConst = typename TAddConst<T>::Type;
template<typename T> using AddVolatile = typename TAddVolatile<T>::Type;
template<typename T> using AddConstVolatile = typename TAddConstVolatile<T>::Type;

// https://en.cppreference.com/w/cpp/types/remove_cv

template<typename T> struct TRemoveConst                { using Type = T; };
template<typename T> struct TRemoveConst<const T>       { using Type = T; };

template<typename T> using RemoveConst = typename TRemoveConst<T>::Type;

template<typename T> struct TRemoveVolatile             { using Type = T; };
template<typename T> struct TRemoveVolatile<volatile T> { using Type = T; };

template<typename T> using RemoveVolatile = typename TRemoveVolatile<T>::Type;

template<typename T> using TRemoveConstVolatile = TRemoveConst<typename TRemoveVolatile<T>::Type>;
template<typename T> using TRemoveCV = TRemoveConstVolatile<T>;

template<typename T> using RemoveConstVolatile = typename TRemoveConstVolatile<T>::Type;
template<typename T> using RemoveCV = typename TRemoveConstVolatile<T>::Type;

// Custom

template<bool AddConst, typename T> using TConditionalAddConst = TSelectType<AddConst, typename TAddConst<T>::Type, T>;
template<bool AddConst, typename T> using ConditionalAddConst = typename TConditionalAddConst<AddConst, T>::Type;

// https://en.cppreference.com/w/cpp/types/remove_cvref

template<typename T> using TRemoveConstVolatileReference = TRemoveConstVolatile<typename TRemoveReference<T>::Type>;
template<typename T> using TRemoveCVRef = TRemoveConstVolatileReference<T>;

template<typename T> using RemoveConstVolatileReference = typename TRemoveConstVolatileReference<T>::Type;
template<typename T> using RemoveCVRef = typename TRemoveConstVolatileReference<T>::Type;