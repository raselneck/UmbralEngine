#pragma once

#include "Containers/Function.h"
#include "Engine/MiscMacros.h"
#include "Templates/IsCallable.h"

// Courtesy of https://www.youtube.com/watch?v=zGWj7Qo_POY&t=620

namespace Private
{
	/**
	 * @brief Defines a struct that will call a function on destruction.
	 */
	class FCallFunctionOnDestruct final
	{
		using FunctionType = TFunction<void()>;

	public:

		/**
		 * @brief Destroys this object, calling the specified "on destruct" function.
		 */
		~FCallFunctionOnDestruct()
		{
			if (m_FunctionToCall.IsValid())
			{
				m_FunctionToCall();
			}
		}

		/**
		 * @brief Binds a lambda to this object.
		 *
		 * @tparam LambdaType The type of the lambda.
		 * @param function The lambda function to call.
		 * @return This object.
		 */
		template<typename LambdaType>
		FCallFunctionOnDestruct& operator->*(LambdaType&& function)
			requires TIsCallable<void, LambdaType>::Value
		{
			m_FunctionToCall = FunctionType::CreateLambda(Forward<LambdaType>(function));
			return *this;
		}

	private:

		FunctionType m_FunctionToCall;
	};
}

/**
 * @brief Begins defining a function to call when this scope exits.
 */
#define ON_EXIT_SCOPE() ::Private::FCallFunctionOnDestruct ANONYMOUS_VAR; ANONYMOUS_VAR ->* [&]()