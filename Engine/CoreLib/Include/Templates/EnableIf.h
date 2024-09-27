#pragma once

// https://en.cppreference.com/w/cpp/types/enable_if

template<bool Condition, typename T = void>
struct TEnableIf {};

template<typename T>
struct TEnableIf<true, T> { using Type = T; };

template<bool Condition, typename T>
using EnableIf = typename TEnableIf<Condition, T>::Type;