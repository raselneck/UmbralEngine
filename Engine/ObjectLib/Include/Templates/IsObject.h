#pragma once

#include "Templates/IntegralConstant.h"
#include "Templates/IsBaseOf.h"

class FObjectPtr;
class FSoftObjectPtr;
template<typename T> class TObjectPtr;
template<typename T> class TSoftObjectPtr;
class UObject;

template<typename T>
struct TIsObject : TIsBaseOf<UObject, T>
{
};

template<> struct TIsObject<FObjectPtr> : FTrueType { };
template<> struct TIsObject<FSoftObjectPtr> : FTrueType { };
template<typename T> struct TIsObject<TObjectPtr<T>> : FTrueType { };
template<typename T> struct TIsObject<TSoftObjectPtr<T>> : FTrueType { };
template<> struct TIsObject<UObject> : FTrueType { };