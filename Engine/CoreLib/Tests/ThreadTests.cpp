#include "Engine/Logging.h"
#include "HAL/TimePoint.h"
#include "HAL/TimeSpan.h"
#include "Threading/Thread.h"
#include <gtest/gtest.h>

/**
 * @brief Calculates the Nth Fibonacci number.
 *
 * @tparam N The sequence number in the Fibonacci sequence to calculate.
 * @param result The result value. Must be a pointer to an int32.
 */
template<int32 N>
static void CalculateNthFibonacciNumber(void* result)
{
	if constexpr (N == 0)
	{
		*reinterpret_cast<int32*>(result) = 0;
	}
	else if constexpr (N == 1)
	{
		*reinterpret_cast<int32*>(result) = 1;
	}
	else
	{
		int32 nMinusOne = 0;
		CalculateNthFibonacciNumber<N - 1>(&nMinusOne);

		int32 nMinusTwo = 0;
		CalculateNthFibonacciNumber<N - 2>(&nMinusTwo);

		*reinterpret_cast<int32*>(result) = nMinusOne + nMinusTwo;
	}
}

TEST(ThreadTests, ParameterizedThreadFunction)
{
	constexpr int32 N = 32;

	const FTimePoint calculationStart = FTimePoint::Now();

	int32 result = 0;
	FThread fibonacciThread = FThread::Create(CalculateNthFibonacciNumber<N>, &result);
	fibonacciThread.Join();

	const FTimeSpan calculationDuration = FTimePoint::Now() - calculationStart;
	UM_LOG(Info, "Fibonacci number N={} is {} (took {} ms to calculate)", N, result, calculationDuration.GetTotalMilliseconds());
}