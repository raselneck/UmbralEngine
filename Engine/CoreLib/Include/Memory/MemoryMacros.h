#pragma once

#include <new> /* For std::align_val_t, std::nothrow_t */

/**
 * @brief Declares all standard-supported `new' operators. It is then up to the user to define any they wish to actually support.
 */
#define DECLARE_NEW_OPERATORS() \
	static void* operator new  (size_t); \
	static void* operator new[](size_t); \
	static void* operator new  (size_t, std::align_val_t); \
	static void* operator new[](size_t, std::align_val_t); \
	static void* operator new  (size_t, const std::nothrow_t&); \
	static void* operator new[](size_t, const std::nothrow_t&); \
	static void* operator new  (size_t, void*); \
	static void* operator new[](size_t, void*)

/**
 * @brief Declares all C++20 compatible, standard-supported `delete' operators. It is then up to the user to define any they wish to actually support.
 */
#define DECLARE_DELETE_OPERATORS() \
	static void operator delete  (void*) noexcept; \
	static void operator delete[](void*) noexcept; \
	static void operator delete  (void*, std::align_val_t) noexcept; \
	static void operator delete[](void*, std::align_val_t) noexcept; \
	static void operator delete  (void*, size_t) noexcept; \
	static void operator delete[](void*, size_t) noexcept; \
	static void operator delete  (void*, size_t, std::align_val_t) noexcept; \
	static void operator delete[](void*, size_t, std::align_val_t) noexcept; \
	static void operator delete  (void*, const std::nothrow_t&) noexcept; \
	static void operator delete[](void*, const std::nothrow_t&) noexcept; \
	static void operator delete  (void*, void*) noexcept; \
	static void operator delete[](void*, void*) noexcept

// NOTE This comment is necessary to appease the "backslash-newline at end of file" erroneous error with GCC