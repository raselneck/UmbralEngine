#pragma once

#include "Templates/EnableIf.h"
#include "Templates/IsObject.h"

// https://en.cppreference.com/w/cpp/memory/addressof

template<class T>
inline EnableIf<IsObject<T>, T*> AddressOf(T& arg) noexcept
{
	return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(arg)));
}

template<class T>
inline EnableIf<IsObject<T> == false, T*> AddressOf(T& arg) noexcept
{
	return &arg;
}