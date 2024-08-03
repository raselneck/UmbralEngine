#pragma once

#include "Containers/Optional.h"
#include "Engine/MiscMacros.h"
#include "Templates/IsCallable.h"

namespace Private
{
	template<usize Line, usize Counter, typename ReturnType, typename CallbackType>
	struct TDoOnceResultContainer
	{
		static ReturnType& Evaluate(CallbackType callback)
		{
			static TOptional<ReturnType> result;

			if (result.HasValue())
			{
				return result.GetValue();
			}

			result = callback();
			return result.GetValue();
		}
	};

	template<usize Line, usize Counter, typename CallbackType>
	struct TDoOnceResultContainer<Line, Counter, void, CallbackType>
	{
		static void Evaluate(CallbackType callback)
		{
			static bool hasBeenEvaluated = false;

			if (hasBeenEvaluated)
			{
				return;
			}

			callback();
			hasBeenEvaluated = true;
		}
	};

	template<usize Line, usize Counter, typename CallbackType>
	auto DoOnceImpl(CallbackType callback) -> decltype(callback())
	{
		using ContainerType = TDoOnceResultContainer<Line, Counter, decltype(callback()), CallbackType>;
		return ContainerType::Evaluate(MoveTemp(callback));
	}
}

/**
 * @brief Allows a lambda or callback to be executed exactly once.
 *
 * Example usage:
 *     DO_ONCE([] {
 *         // ...something really expensive that only needs to be done once...
 *     });
 *
 * Or, if you want to do something once and get the same value every time thereafter:
 *     const int32 result = DO_ONCE([] {
 *         // Something like calculating the 20th Fibonacci number
 *         return result;
 *     }));
 */
#define DO_ONCE(Callback) ::Private::DoOnceImpl<UMBRAL_LINE, __COUNTER__>(Callback)