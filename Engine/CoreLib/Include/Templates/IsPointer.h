#pragma once

#include "Templates/IntegralConstant.h"
#include "Templates/IsReference.h"
#include "Templates/TypeIdentity.h"

// https://en.cppreference.com/w/cpp/types/is_pointer

template<typename T> struct TIsPointer     : FFalseType { };
template<typename T> struct TIsPointer<T*> : FTrueType  { };
template<typename T> struct TIsPointer<T* const> : FTrueType  { };
template<typename T> struct TIsPointer<T* volatile> : FTrueType  { };
template<typename T> struct TIsPointer<T* const volatile> : FTrueType  { };

template<typename T> inline constexpr bool IsPointer = TIsPointer<T>::Value;

// https://en.cppreference.com/w/cpp/types/add_pointer

namespace Private
{
	template<typename T> auto TryAddPointer(int) -> TTypeIdentity<typename TRemoveReference<T>::Type*> { };
	template<typename T> auto TryAddPointer(...) -> TTypeIdentity<T> { };
}

template<typename T> using TAddPointer = decltype(Private::TryAddPointer<T>(0));

template<typename T> using AddPointer = typename TAddPointer<T>::Type;

// https://en.cppreference.com/w/cpp/types/remove_pointer

template<typename T> struct TRemovePointer     { using Type = T; };
template<typename T> struct TRemovePointer<T*> { using Type = T; };

template<typename T> using RemovePointer = typename TRemovePointer<T>::Type;