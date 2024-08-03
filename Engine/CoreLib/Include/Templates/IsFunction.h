#pragma once

#include "Templates/IntegralConstant.h"

template<typename T>
struct TIsFunction : FFalseType { };

// TODO Investigate the implementation on https://en.cppreference.com/w/cpp/types/TIsFunction
// They basically say that we can do the following:
//
//     template<typename T>
//     using TIsFunction = TBoolConstant<
//         TAnd<
//             TNot<TIsConst<const T>>,
//             TNot<TIsReference<T>>
//         >::Value
//     >;

template<typename ReturnType, typename... ArgTypes>
struct TIsFunction<ReturnType(ArgTypes...)> : FTrueType { };

template<typename ReturnType, typename... ArgTypes>
struct TIsFunction<ReturnType(ArgTypes...) const> : FTrueType { };

// Below are specialization for variadic functions, such as std::printf

template<typename ReturnType, typename... ArgTypes>
struct TIsFunction<ReturnType(ArgTypes..., ...)> : FTrueType { };

template<typename ReturnType, typename... ArgTypes>
struct TIsFunction<ReturnType(ArgTypes..., ...) const> : FTrueType { };