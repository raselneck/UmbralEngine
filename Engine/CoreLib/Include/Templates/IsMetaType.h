#pragma once

#include "Meta/ClassInfo.h"
#include "Meta/StructInfo.h"
#include "Templates/IsBaseOf.h"
#include "Templates/IsConstVolatile.h"
#include "Templates/IsPointer.h"

/**
 * @brief Checks to see if a type has meta information available.
 *
 * @tparam T The type to check.
 */
template<typename T, typename MetaType>
struct TIsMetaType
{
	static constexpr bool Value = requires()
	{
		T::StaticType();
		requires(IsPointer<decltype(T::StaticType())>);
		requires(IsConst<RemovePointer<decltype(T::StaticType())>>);
		requires(IsBaseOf<MetaType, RemoveConst<RemovePointer<decltype(T::StaticType())>>>);
	};
};

template<typename T>
using TIsMetaClass = TIsMetaType<T, FClassInfo>;

template<typename T>
using TIsMetaStruct = TIsMetaType<T, FStructInfo>;

template<typename T, typename MetaType>
inline constexpr bool IsMetaType = TIsMetaType<T, MetaType>::Value;

template<typename T>
inline constexpr bool IsMetaClass = TIsMetaClass<T>::Value;

template<typename T>
inline constexpr bool IsMetaStruct = TIsMetaStruct<T>::Value;