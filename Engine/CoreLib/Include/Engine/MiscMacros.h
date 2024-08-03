#pragma once

#include "Engine/IntTypes.h"
#include "Templates/IsArray.h"
#include <cstddef>

#define UM_ARRAY_SIZE(Array)                        TArrayExtent<decltype(Array)>::Value

#define UM_JOIN_IMPL_2(First, Second)               First##Second
#define UM_JOIN_IMPL_1(First, Second)               UM_JOIN_IMPL_2(First, Second)
#define UM_JOIN(First, Second)                      UM_JOIN_IMPL_1(First, Second)

#define UM_STRINGIFY_IMPL_2(Thing)                  #Thing
#define UM_STRINGIFY_IMPL_1(Thing)                  UM_STRINGIFY_IMPL_2(Thing)
#define UM_STRINGIFY(Thing)                         UM_STRINGIFY_IMPL_1(Thing)
#define UM_STRINGIFY_AS_VIEW(Thing)                 UM_JOIN(UM_STRINGIFY(Thing), _sv)

#define UM_OFFSET_OF_CAST_TO_TYPE(Type, Value)      reinterpret_cast<const Type*>(Value)
#define UM_OFFSET_OF_BASE_ADDRESS                   UM_OFFSET_OF_CAST_TO_TYPE(uint8, 0xAABB)
#define UM_OFFSET_OF_MEMBER_POINTER(Type, Member)   UM_OFFSET_OF_CAST_TO_TYPE(uint8, &(UM_OFFSET_OF_CAST_TO_TYPE(Type, 0xAABB)->Member))
#define UM_OFFSET_OF(Type, Member)                  static_cast<int32>(UM_OFFSET_OF_MEMBER_POINTER(Type, Member) - UM_OFFSET_OF_BASE_ADDRESS)

#define UM_DEFAULT_COPY(TypeName)                       \
	TypeName(const TypeName&)                = default; \
	TypeName& operator=(const TypeName&)     = default

#define UM_DEFAULT_MOVE(TypeName)                       \
	TypeName(TypeName&&)            noexcept = default; \
	TypeName& operator=(TypeName&&) noexcept = default

#define UM_DISABLE_COPY(TypeName)                       \
	TypeName(const TypeName&)                = delete;  \
	TypeName& operator=(const TypeName&)     = delete

#define UM_DISABLE_MOVE(TypeName)                       \
	TypeName(TypeName&&)                     = delete;  \
	TypeName& operator=(TypeName&&)          = delete

#define nameof                                      UM_STRINGIFY_AS_VIEW

#define ANONYMOUS_VAR                               UM_JOIN(l_anonymous_var_, UMBRAL_LINE)