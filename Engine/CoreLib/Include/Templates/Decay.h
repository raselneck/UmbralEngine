#pragma once

#include "Templates/IsArray.h"
#include "Templates/IsConstVolatile.h"
#include "Templates/IsFunction.h"
#include "Templates/IsReference.h"
#include "Templates/IsPointer.h"
#include "Templates/Select.h"

// https://en.cppreference.com/w/cpp/types/decay

template<typename T>
class TDecay
{
	using U = typename TRemoveReference<T>::Type;

public:

	using Type = typename TSelectType<
		TIsArray<U>::Value,
		typename TAddPointer<typename TRemoveExtent<U>::Type>::Type,
		typename TSelectType<
			TIsFunction<U>::Value,
			typename TAddPointer<U>::Type,
			typename TRemoveConstVolatile<U>::Type
		>::Type
	>::Type;
};