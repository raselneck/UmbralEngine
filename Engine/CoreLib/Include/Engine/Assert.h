#pragma once

#include "Engine/Platform.h"
#include "Engine/MiscMacros.h"
#include "Templates/StringTraits.h"
#include <stdexcept>

#if UMBRAL_DEBUG
#	if UMBRAL_PLATFORM == UMBRAL_PLATFORM_WINDOWS
extern void __cdecl __debugbreak(void);
#		define UMBRAL_DEBUG_BREAK() __debugbreak()
#	else
#		if defined(__APPLE__)
#			include <TargetConditionals.h>
#		endif
#		if defined(__clang__) && !TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#			define UMBRAL_DEBUG_BREAK() __builtin_debugtrap()
#		elif defined(__GNUC__)
#			define UMBRAL_DEBUG_BREAK() __builtin_trap()
#		elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE__)
#			include <signal.h>
#			define UMBRAL_DEBUG_BREAK() raise(SIGTRAP)
#		endif
#	endif
#endif

#ifndef UMBRAL_DEBUG_BREAK
#	define UMBRAL_DEBUG_BREAK() ((void)0)
#endif

namespace Private
{
	// TODO Use FStringView instead of raw const char pointers here. Need to ensure FStringView does not use asserts, though

	/**
	 * @brief Logs information about an assertion that was triggered.
	 *
	 * @param condition The assertion condition.
	 * @param message The assertion message.
	 * @param file The file the assertion occurred in.
	 * @param line The line the assertion occurred at.
	 * @param function The function the assertion occurred in.
	 */
	void LogAssert(const char* condition, const char* message, const char* file, int line, const char* function);

	/**
	 * @brief Checks to see if a debug break should be triggered for a failed ensure.
	 *
	 * @param condition The condition.
	 * @param conditionText The condition as a string.
	 * @param file The current file.
	 * @param line The current line.
	 * @return True if the break should happen, otherwise false.
	 */
	[[nodiscard]] bool ShouldBreakForEnsure(bool condition, const char* conditionText, const char* file, int line);

	namespace SeriouslyDontTouch
	{
		/**
		 * @brief Purposefully crashes the current application.
		 */
		[[noreturn]] void CrashApplication();
	}
}

#define UM_ASSERT(Condition, Message)                                                                               \
	do                                                                                                              \
	{                                                                                                               \
		static_assert(TIsAnsiStringLiteral<decltype(Message)>::Value, "Please use ANSI string literals with UM_ASSERT messages"); \
		if (static_cast<bool>(Condition) == false)                                                                  \
		{                                                                                                           \
			Private::LogAssert(#Condition, Message, UMBRAL_FILE, UMBRAL_LINE, UMBRAL_FUNCTION);                     \
			UMBRAL_DEBUG_BREAK();                                                                                   \
			Private::SeriouslyDontTouch::CrashApplication();                                                        \
		}                                                                                                           \
	} while (0)

#define UM_CONSTEXPR_ASSERT(Condition, Message)                                                                     \
	do                                                                                                              \
	{                                                                                                               \
		if (std::is_constant_evaluated())                                                                           \
		{                                                                                                           \
			if ((Condition) == false)                                                                               \
			{                                                                                                       \
				throw std::runtime_error("Constexpr condition `" UM_STRINGIFY(Condition) "' is false");             \
			}                                                                                                       \
		}                                                                                                           \
		else                                                                                                        \
		{                                                                                                           \
			UM_ASSERT(Condition, Message);                                                                          \
		}                                                                                                           \
	} while (0)

#define UM_ASSERT_NOT_REACHED_MSG(Message)                                                                          \
	do                                                                                                              \
	{                                                                                                               \
		static_assert(TIsAnsiStringLiteral<decltype(Message)>::Value, "Please use ANSI string literals with UM_ASSERT messages"); \
		Private::LogAssert(nullptr, Message, UMBRAL_FILE, UMBRAL_LINE, UMBRAL_FUNCTION);                            \
		UMBRAL_DEBUG_BREAK();                                                                                       \
		Private::SeriouslyDontTouch::CrashApplication();                                                            \
	} while(0)

#define UM_ASSERT_NOT_REACHED()                                                                                     \
	UM_ASSERT_NOT_REACHED_MSG("Execution should not have reached this point")

#define UM_ENSURE_IMPL(Always, Condition) [&]                                                                       \
	{                                                                                                               \
		const bool ANONYMOUS_VAR = Condition;                                                                       \
		if (::Private::ShouldBreakForEnsure(ANONYMOUS_VAR, UM_STRINGIFY(Condition), UMBRAL_FILE, UMBRAL_LINE))      \
		{                                                                                                           \
			UMBRAL_DEBUG_BREAK();                                                                                   \
		}                                                                                                           \
		return ANONYMOUS_VAR;                                                                                       \
	}()

#define UM_ENSURE(Condition) UM_ENSURE_IMPL(false, Condition)