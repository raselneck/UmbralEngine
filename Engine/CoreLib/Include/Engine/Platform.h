#pragma once

#include "Engine/MiscMacros.h"
#include "Templates/IsSame.h"

#if !defined(UMBRAL_RELEASE) && !defined(UMBRAL_DEBUG)
#	error Either UMBRAL_DEBUG or UMBRAL_RELEASE must be defined.
#endif

#if defined(UMBRAL_RELEASE) && !defined(UMBRAL_DEBUG)
#	define UMBRAL_DEBUG (!UMBRAL_RELEASE)
#elif defined(UMBRAL_DEBUG) && !defined(UMBRAL_RELEASE)
#	define UMBRAL_RELEASE (!UMBRAL_DEBUG)
#endif

#define UMBRAL_PLATFORM_WINDOWS                 0
#define UMBRAL_PLATFORM_LINUX                   1
#define UMBRAL_PLATFORM_MAC                     2
#define UMBRAL_PLATFORM_IOS                     3
#define UMBRAL_PLATFORM_ANDROID                 4
#define UMBRAL_PLATFORM_EMSCRIPTEN              5

#define UMBRAL_ARCH_X86                         0
#define UMBRAL_ARCH_AMD64                       1
#define UMBRAL_ARCH_ARM32                       2
#define UMBRAL_ARCH_ARM64                       3

#define UMBRAL_COMPILER_MSVC                    0
#define UMBRAL_COMPILER_GCC                     1
#define UMBRAL_COMPILER_CLANG                   2
#define UMBRAL_COMPILER_APPLE_CLANG             3

// Determine current platform
#if _WIN32 || _WIN64
#	define UMBRAL_PLATFORM UMBRAL_PLATFORM_WINDOWS
#elif __APPLE__
#	include <TargetConditionals.h>
#	if TARGET_OS_IOS
#		define UMBRAL_PLATFORM UMBRAL_PLATFORM_IOS
#	elif TARGET_OS_MAC
#		define UMBRAL_PLATFORM UMBRAL_PLATFORM_MAC
#	else
#		error Cannot build for the target platform.
#	endif
#elif __linux__
#	define UMBRAL_PLATFORM UMBRAL_PLATFORM_LINUX
#elif defined(__ANDROID__)
#	define UMBRAL_PLATFORM UMBRAL_PLATFORM_ANDROID
#elif defined(__EMSCRIPTEN__)
#	define UMBRAL_PLATFORM UMBRAL_PLATFORM_EMSCRIPTEN
#else
#	error Unknown platform, cannot build.
#endif

// Determine current architecture
#if defined(__x86_64__) || defined(_M_AMD64)
#	define UMBRAL_ARCH UMBRAL_ARCH_AMD64
#elif defined(__arm64__) || defined(_M_ARM64)
#	define UMBRAL_ARCH UMBRAL_ARCH_ARM64
#elif defined(__arm__) || defined(_M_ARM)
#	define UMBRAL_ARCH UMBRAL_ARCH_ARM32
#elif defined(__i386__) || defined(_M_IX86)
#	define UMBRAL_ARCH UMBRAL_ARCH_X86
#else
#	error Unknown architecture, cannot build.
#endif

#define UMBRAL_ARCH_IS_X86                      (UMBRAL_ARCH == UMBRAL_ARCH_X86)
#define UMBRAL_ARCH_IS_AMD64                    (UMBRAL_ARCH == UMBRAL_ARCH_AMD64)
#define UMBRAL_ARCH_IS_ARM32                    (UMBRAL_ARCH == UMBRAL_ARCH_ARM32)
#define UMBRAL_ARCH_IS_ARM64                    (UMBRAL_ARCH == UMBRAL_ARCH_ARM64)
#define UMBRAL_ARCH_IS_32BIT                    (UMBRAL_ARCH_IS_X86 || UMBRAL_ARCH_IS_ARM32)
#define UMBRAL_ARCH_IS_64BIT                    (UMBRAL_ARCH_IS_AMD64 || UMBRAL_ARCH_IS_ARM64)
#define UMBRAL_ARCH_IS_ARM                      (UMBRAL_ARCH_IS_ARM32 || UMBRAL_ARCH_IS_ARM64)

#define UMBRAL_PLATFORM_IS_WINDOWS              (UMBRAL_PLATFORM == UMBRAL_PLATFORM_WINDOWS)
#define UMBRAL_PLATFORM_IS_LINUX                (UMBRAL_PLATFORM == UMBRAL_PLATFORM_LINUX)
#define UMBRAL_PLATFORM_IS_MAC                  (UMBRAL_PLATFORM == UMBRAL_PLATFORM_MAC)
#define UMBRAL_PLATFORM_IS_IOS                  (UMBRAL_PLATFORM == UMBRAL_PLATFORM_IOS)
#define UMBRAL_PLATFORM_IS_ANDROID              (UMBRAL_PLATFORM == UMBRAL_PLATFORM_ANDROID)
#define UMBRAL_PLATFORM_IS_EMSCRIPTEN           (UMBRAL_PLATFORM == UMBRAL_PLATFORM_EMSCRIPTEN)

#define UMBRAL_PLATFORM_IS_APPLE                (UMBRAL_PLATFORM_IS_MAC || UMBRAL_PLATFORM_IS_IOS)
#define UMBRAL_PLATFORM_IS_DESKTOP              (UMBRAL_PLATFORM_IS_WINDOWS || UMBRAL_PLATFORM_IS_LINUX || UMBRAL_PLATFORM_IS_MAC)
#define UMBRAL_PLATFORM_IS_MOBILE               (UMBRAL_PLATFORM_IS_ANDROID || UMBRAL_PLATFORM_IS_IOS)
#define UMBRAL_PLATFORM_IS_WEB                  (UMBRAL_PLATFORM_IS_EMSCRIPTEN)

// TODO Make this a test for a define at CMake configure time
#if UMBRAL_PLATFORM_IS_APPLE || UMBRAL_PLATFORM_IS_WINDOWS
#	define UMBRAL_PLATFORM_LONG_IS_UNIQUE 1
	static_assert(IsSame<signed long, int32> == false);
	static_assert(IsSame<signed long, int64> == false);
	static_assert(IsSame<unsigned long, uint32> == false);
	static_assert(IsSame<unsigned long, uint64> == false);
#else
#	define UMBRAL_PLATFORM_LONG_IS_UNIQUE 0
#	if UMBRAL_ARCH_IS_64BIT
		static_assert(IsSame<signed long, int64>);
		static_assert(IsSame<unsigned long, uint64>);
#	else
		static_assert(IsSame<signed long, int32>);
		static_assert(IsSame<unsigned long, uint32>);
#	endif
#endif

// TODO Make this a test for a define at CMake configure time
#if UMBRAL_PLATFORM_IS_LINUX
#	define UMBRAL_PLATFORM_LONG_LONG_IS_UNIQUE 1
	static_assert(IsSame<signed long long, int64> == false);
	static_assert(IsSame<unsigned long long, uint64> == false);
#else
#	define UMBRAL_PLATFORM_LONG_LONG_IS_UNIQUE 0
	static_assert(IsSame<signed long long, int64>);
	static_assert(IsSame<unsigned long long, uint64>);
#endif

// Determine current endianness
#if defined(__BYTE_ORDER__)
#	define UMBRAL_ENDIANNESS_LITTLE             __ORDER_LITTLE_ENDIAN__
#	define UMBRAL_ENDIANNESS_BIG                __ORDER_BIG_ENDIAN__
#	define UMBRAL_ENDIANNESS                    __BYTE_ORDER__
#else
#	define UMBRAL_ENDIANNESS_LITTLE             0
#	define UMBRAL_ENDIANNESS_BIG                1
#	if defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
#		define UMBRAL_ENDIANNESS                UMBRAL_ENDIANNESS_BIG
#	elif UMBRAL_PLATFORM_IS_WINDOWS || defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
#		define UMBRAL_ENDIANNESS                UMBRAL_ENDIANNESS_LITTLE
#	else
#		error Unable to determine platform endianness.
#	endif
#endif

#define UMBRAL_FILE __FILE__
#define UMBRAL_LINE __LINE__

#define UMBRAL_FILE_AS_VIEW UM_JOIN(UMBRAL_FILE, _sv)

// Determine the current compiler
#if defined(_MSC_VER)
#	define UMBRAL_COMPILER                      UMBRAL_COMPILER_MSVC
#	define UMBRAL_EXPORT                        __declspec(dllexport)
#	define UMBRAL_IMPORT                        __declspec(dllimport)
#	define UMBRAL_FUNCTION                      __FUNCSIG__
#	define LIKELY(Condition)                    Condition
#	define UNLIKELY(Condition)                  Condition
#	define FORCEINLINE                          __forceinline
#	define FORCENOINLINE                        __declspec(noinline)
// TODO FORCEINLINE_DEBUGGABLE
#	define PRAGMA_WARNING_PUSH                  __pragma(warning(push))
#	define PRAGMA_WARNING_DISABLE_MSVC(id)      __pragma(warning(disable: id))
#	define PRAGMA_WARNING_DISABLE_CLANG(id)
#	define PRAGMA_WARNING_DISABLE_GCC(id)
#	define PRAGMA_WARNING_POP                   __pragma(warning(pop))
#elif defined(__clang__)
#	if UMBRAL_PLATFORM == UMBRAL_PLATFORM_MAC || UMBRAL_PLATFORM == UMBRAL_PLATFORM_IOS
#		define UMBRAL_COMPILER                  UMBRAL_COMPILER_APPLE_CLANG
#	else
#		define UMBRAL_COMPILER                  UMBRAL_COMPILER_CLANG
#	endif
#	define UMBRAL_EXPORT                        __attribute__((visibility("default")))
#	define UMBRAL_IMPORT
#	define UMBRAL_FUNCTION                      __PRETTY_FUNCTION__
#	define LIKELY(Condition)                    __builtin_expect(!!(Condition), 1)
#	define UNLIKELY(Condition)                  __builtin_expect(!!(Condition), 0)
#	define FORCEINLINE                          __attribute__((always_inline))
#	define FORCENOINLINE                        __attribute__((noinline))
// TODO FORCEINLINE_DEBUGGABLE
#	define PRAGMA_WARNING_PUSH                  _Pragma("clang diagnostic push")
#	define PRAGMA_WARNING_DISABLE_MSVC(id)
#	define PRAGMA_WARNING_DISABLE_CLANG(id)     _Pragma(UM_STRINGIFY(clang diagnostic ignored id))
#	define PRAGMA_WARNING_DISABLE_GCC(id)
#	define PRAGMA_WARNING_POP                   _Pragma("clang diagnostic pop")
#elif defined(__GNUC__) || defined(__GNUG__)
#	define UMBRAL_COMPILER                      UMBRAL_COMPILER_GCC
#	define UMBRAL_EXPORT                        __attribute__((visibility("default")))
#	define UMBRAL_IMPORT
#	define UMBRAL_FUNCTION                      __FUNCTION__
#	define LIKELY(Condition)                    __builtin_expect(!!(Condition), 1)
#	define UNLIKELY(Condition)                  __builtin_expect(!!(Condition), 0)
#	define FORCEINLINE                          __attribute__((always_inline))
#	define FORCENOINLINE                        __attribute__((noinline))
// TODO FORCEINLINE_DEBUGGABLE
#	define PRAGMA_WARNING_PUSH                  _Pragma("GCC diagnostic push")
#	define PRAGMA_WARNING_DISABLE_MSVC(id)
#	define PRAGMA_WARNING_DISABLE_CLANG(id)
#	define PRAGMA_WARNING_DISABLE_GCC(id)       _Pragma(UM_STRINGIFY(GCC diagnostic ignored id))
#	define PRAGMA_WARNING_POP                   _Pragma("GCC diagnostic pop")
#else
#	error Unknown compiler, cannot build.
#endif

/**
 * @brief An enumeration of system platforms.
 */
enum class EPlatform
{
	/** @brief Windows. */
	Windows = UMBRAL_PLATFORM_WINDOWS,
	/** @brief Linux. */
	Linux = UMBRAL_PLATFORM_LINUX,
	/** @brief Mac OS. */
	Mac = UMBRAL_PLATFORM_MAC,
	/** @brief iOS (iPhone and iPad). */
	iOS = UMBRAL_PLATFORM_IOS,
	/** @brief Android. */
	Android = UMBRAL_PLATFORM_ANDROID,
	/** @brief Emscripten aka Web. */
	Emscripten = UMBRAL_PLATFORM_EMSCRIPTEN
};

/**
 * @brief An enumeration of system architectures.
 */
enum class EArchitecture
{
	/** @brief 32-bit (not officially supported). */
	x86 = UMBRAL_ARCH_X86,
	/** @brief 64-bit. */
	Amd64 = UMBRAL_ARCH_AMD64,
	/** @brief 32-bit ARM (not officially supported). */
	Arm32 = UMBRAL_ARCH_ARM32,
	/** @brief 64-bit ARM. */
	Arm64 = UMBRAL_ARCH_ARM64
};

/**
 * @brief An enumeration of system endianness.
 */
enum class EEndianness
{
	/** @brief Little endian. */
	Little = UMBRAL_ENDIANNESS_LITTLE,
	/** @brief Big endian. */
	Big = UMBRAL_ENDIANNESS_BIG
};

/**
 * @brief An enumeration of system compilers.
 */
enum class ECompiler
{
	/** @brief Microsoft's Visual C++ compiler. */
	MSVC = UMBRAL_COMPILER_MSVC,
	/** @brief The GNU C++ compiler. */
	GCC = UMBRAL_COMPILER_GCC,
	/** @brief LLVM's C++ compiler. */
	Clang = UMBRAL_COMPILER_CLANG,
	/** @brief Apple's fork of LLVM's C++ compiler. */
	AppleClang = UMBRAL_COMPILER_APPLE_CLANG
};

/**
 * @brief Gets the current system platform.
 *
 * @return The current system platform.
 */
inline constexpr EPlatform GetSystemPlatform()
{
	return static_cast<EPlatform>(UMBRAL_PLATFORM);
}

/**
 * @brief Gets the current system architecture.
 *
 * @return The current system architecture.
 */
inline constexpr EArchitecture GetSystemArchitecture()
{
	return static_cast<EArchitecture>(UMBRAL_ARCH);
}

/**
 * @brief Gets the current system endianness.
 *
 * @return The current system endianness.
 */
inline constexpr EEndianness GetSystemEndianness()
{
	return static_cast<EEndianness>(UMBRAL_ENDIANNESS);
}

/**
 * @brief Gets the system compiler we were built with.
 *
 * @return The system compiler we were built with.
 */
inline constexpr ECompiler GetSystemCompiler()
{
	return static_cast<ECompiler>(UMBRAL_COMPILER);
}