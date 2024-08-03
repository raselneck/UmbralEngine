#pragma once

#include "Templates/IntegralConstant.h"

template<typename T>
struct TIsChar : FFalseType
{
};

template<> struct TIsChar<char>     : FTrueType { };
template<> struct TIsChar<char8_t>  : FTrueType { };
template<> struct TIsChar<char16_t> : FTrueType { };
template<> struct TIsChar<char32_t> : FTrueType { };
template<> struct TIsChar<wchar_t>  : FTrueType { };