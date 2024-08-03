#pragma once

#include <cstdint>

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

static_assert(sizeof(int8)  == 1);
static_assert(sizeof(int16) == 2);
static_assert(sizeof(int32) == 4);
static_assert(sizeof(int64) == 8);

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

static_assert(sizeof(uint8)  == 1);
static_assert(sizeof(uint16) == 2);
static_assert(sizeof(uint32) == 4);
static_assert(sizeof(uint64) == 8);

#if UINTPTR_MAX == UINT32_MAX
	using isize = signed int;
	using usize = unsigned int;

	static_assert(sizeof(isize) == sizeof(int32));
	static_assert(sizeof(usize) == sizeof(uint32));
#else
	using isize = signed long long;
	using usize = unsigned long long;

	static_assert(sizeof(isize) == sizeof(int64));
	static_assert(sizeof(usize) == sizeof(uint64));
#endif

static_assert(sizeof(usize) == sizeof(isize));
static_assert(sizeof(usize) == sizeof(void*));

using intptr = isize;
using uintptr = usize;

enum : int32 { INDEX_NONE = -1 };