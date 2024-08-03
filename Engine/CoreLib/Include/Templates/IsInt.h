#pragma once

#include "Engine/IntTypes.h"
#include "Templates/IntegralConstant.h"
#include "Templates/IsConstVolatile.h"
#include "Templates/Select.h"

// https://en.cppreference.com/w/cpp/types/is_integral

namespace Private
{
	template<typename T> struct TIsRawInt : FFalseType { };
	template<> struct TIsRawInt<signed char> : FTrueType { };
	template<> struct TIsRawInt<signed short int> : FTrueType { };
	template<> struct TIsRawInt<signed int> : FTrueType { };
	template<> struct TIsRawInt<signed long int> : FTrueType { };
	template<> struct TIsRawInt<signed long long> : FTrueType { };
	template<> struct TIsRawInt<unsigned char> : FTrueType { };
	template<> struct TIsRawInt<unsigned short int> : FTrueType { };
	template<> struct TIsRawInt<unsigned int> : FTrueType { };
	template<> struct TIsRawInt<unsigned long int> : FTrueType { };
	template<> struct TIsRawInt<unsigned long long> : FTrueType { };

	template<typename T> struct TIsRawSigned : FFalseType { };
	template<> struct TIsRawSigned<signed char> : FTrueType { };
	template<> struct TIsRawSigned<signed short int> : FTrueType { };
	template<> struct TIsRawSigned<signed int> : FTrueType { };
	template<> struct TIsRawSigned<signed long int> : FTrueType { };
	template<> struct TIsRawSigned<signed long long> : FTrueType { };
	template<> struct TIsRawSigned<float> : FTrueType { };
	template<> struct TIsRawSigned<double> : FTrueType { };
	template<> struct TIsRawSigned<long double> : FTrueType { };

	template<typename T> struct TIsRawUnsigned : FFalseType { };
	template<> struct TIsRawUnsigned<unsigned char> : FTrueType { };
	template<> struct TIsRawUnsigned<unsigned short int> : FTrueType { };
	template<> struct TIsRawUnsigned<unsigned int> : FTrueType { };
	template<> struct TIsRawUnsigned<unsigned long int> : FTrueType { };
	template<> struct TIsRawUnsigned<unsigned long long> : FTrueType { };
}

template<typename T> using TIsInt = Private::TIsRawInt<typename TRemoveConstVolatile<T>::Type>;
template<typename T> using TIsSigned = Private::TIsRawSigned<typename TRemoveConstVolatile<T>::Type>;
template<typename T> using TIsUnsigned = Private::TIsRawUnsigned<typename TRemoveConstVolatile<T>::Type>;

template<typename T> inline constexpr bool IsInt = TIsInt<T>::Value;
template<typename T> inline constexpr bool IsSigned = TIsSigned<T>::Value;
template<typename T> inline constexpr bool IsUnsigned = TIsUnsigned<T>::Value;

// FIXME Neither MakeSigned nor MakeUnsigned work with CV

template<typename T> struct TMakeSigned;
template<> struct TMakeSigned<signed char>          { using Type = int8; };
template<> struct TMakeSigned<unsigned char>        { using Type = int8; };
template<> struct TMakeSigned<char>                 { using Type = int8; };
template<> struct TMakeSigned<signed short int>     { using Type = int16; };
template<> struct TMakeSigned<unsigned short int>   { using Type = int16; };
template<> struct TMakeSigned<signed int>           { using Type = int32; };
template<> struct TMakeSigned<unsigned int>         { using Type = int32; };
template<> struct TMakeSigned<signed long int>      { using Type = SelectType<sizeof(signed long int) == 8, int64, int32>; };
template<> struct TMakeSigned<unsigned long int>    { using Type = SelectType<sizeof(unsigned long int) == 8, int64, int32>; };
template<> struct TMakeSigned<signed long long>     { static_assert(sizeof(signed long long) == 8);  using Type = int64; };
template<> struct TMakeSigned<unsigned long long>   { static_assert(sizeof(signed long long) == 8);  using Type = int64; };

template<typename T>
using MakeSigned = typename TMakeSigned<T>::Type;

static_assert(sizeof(signed long int) == sizeof(MakeSigned<signed long int>));
static_assert(sizeof(unsigned long int) == sizeof(MakeSigned<unsigned long int>));

template<typename T> struct TMakeUnsigned;
template<> struct TMakeUnsigned<signed char>          { using Type = uint8; };
template<> struct TMakeUnsigned<unsigned char>        { using Type = uint8; };
template<> struct TMakeUnsigned<char>                 { using Type = uint8; };
template<> struct TMakeUnsigned<signed short int>     { using Type = uint16; };
template<> struct TMakeUnsigned<unsigned short int>   { using Type = uint16; };
template<> struct TMakeUnsigned<signed int>           { using Type = uint32; };
template<> struct TMakeUnsigned<unsigned int>         { using Type = uint32; };
template<> struct TMakeUnsigned<signed long int>      { using Type = SelectType<sizeof(signed long int) == 8, uint64, uint32>; };
template<> struct TMakeUnsigned<unsigned long int>    { using Type = SelectType<sizeof(unsigned long int) == 8, uint64, uint32>; };
template<> struct TMakeUnsigned<signed long long>     { static_assert(sizeof(signed long long) == 8);  using Type = uint64; };
template<> struct TMakeUnsigned<unsigned long long>   { static_assert(sizeof(signed long long) == 8);  using Type = uint64; };

template<typename T>
using MakeUnsigned = typename TMakeUnsigned<T>::Type;

static_assert(sizeof(signed long int) == sizeof(MakeUnsigned<signed long int>));
static_assert(sizeof(unsigned long int) == sizeof(MakeUnsigned<unsigned long int>));