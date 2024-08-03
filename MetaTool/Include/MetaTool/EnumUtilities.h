#pragma once

#include "Containers/StringView.h"

namespace Private
{
	template<typename T>
	struct TEnumToString;

	template<typename T>
	struct TStringToEnum;
}

template<typename T>
constexpr FStringView EnumToString(T value)
{
	return Private::TEnumToString<T>::Get(value);
}

template<typename T>
constexpr T StringToEnum(const FStringView value)
{
	return Private::TStringToEnum<T>::Get(value);
}