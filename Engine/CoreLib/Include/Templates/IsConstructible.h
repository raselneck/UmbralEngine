#pragma once

#include "Templates/IsReference.h"

// https://en.cppreference.com/w/cpp/types/is_constructible

template<typename ConstructType, typename... ArgTypes>
using TIsConstructible = TBoolConstant<__is_constructible(ConstructType, ArgTypes...)>;

template<typename T>
using TIsDefaultConstructible = TIsConstructible<T>;

template<typename T>
using TIsCopyConstructible = TIsConstructible<T, typename TAddLValueReference<T>::Type>;

template<typename T>
using TIsMoveConstructible = TIsConstructible<T, typename TAddRValueReference<T>::Type>;

template<typename ConstructType, typename... ArgTypes>
using TIsTriviallyConstructible = TBoolConstant<__is_trivially_constructible(ConstructType, ArgTypes...)>;

template<typename T>
using TIsTriviallyDefaultConstructible = TIsTriviallyConstructible<T>;

template<typename T>
using TIsTriviallyCopyConstructible = TIsTriviallyConstructible<T, typename TAddLValueReference<T>::Type>;

template<typename T>
using TIsTriviallyMoveConstructible = TIsTriviallyConstructible<T, typename TAddRValueReference<T>::Type>;

template<typename ConstructType, typename... ArgTypes>
inline constexpr bool IsConstructible = TIsConstructible<ConstructType, ArgTypes...>::Value;

template<typename T>
inline constexpr bool IsDefaultConstructible = TIsDefaultConstructible<T>::Value;

template<typename T>
inline constexpr bool IsCopyConstructible = TIsCopyConstructible<T>::Value;

template<typename T>
inline constexpr bool IsMoveConstructible = TIsMoveConstructible<T>::Value;

template<typename ConstructType, typename... ArgTypes>
inline constexpr bool IsTriviallyConstructible = TIsTriviallyConstructible<ConstructType, ArgTypes...>::Value;

template<typename T>
inline constexpr bool IsTriviallyDefaultConstructible = TIsTriviallyDefaultConstructible<T>::Value;

template<typename T>
inline constexpr bool IsTriviallyCopyConstructible = TIsTriviallyCopyConstructible<T>::Value;

template<typename T>
inline constexpr bool IsTriviallyMoveConstructible = TIsTriviallyMoveConstructible<T>::Value;