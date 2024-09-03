#pragma once

#include "Containers/Variant.h"
#include "Engine/Assert.h"
#include "Memory/Memory.h"
#include "Memory/SharedPtr.h"
#include "Memory/SmallBufferStorage.h"
#include "Memory/UniquePtr.h"
#include "Templates/IsAssignable.h"
#include "Templates/IsCallable.h"
#include "Templates/IsSame.h"
#include "Templates/IsZeroConstructible.h"
#include "Templates/Select.h"

// TODO Make Invoke able to be `const'

template<typename>
class TFunction;

/**
 * @brief Defines an abstraction for functions and function-like objects.
 *
 * @tparam ReturnType The return type.
 * @tparam ArgTypes The argument types.
 */
template<typename ReturnType, typename... ArgTypes>
class TFunction<ReturnType(ArgTypes...)> final
{
	UM_DISABLE_COPY(TFunction);

public:

	template<typename ObjectType>
	using MemberFunctionSignature = ReturnType(ObjectType::*)(ArgTypes...);
	template<typename ObjectType>
	using ConstMemberFunctionSignature = ReturnType(ObjectType::*)(ArgTypes...) const;
	using StaticFunctionSignature = ReturnType(*)(ArgTypes...);

	/**
	 * @brief Sets default values for this function's properties.
	 */
	TFunction() = default;

	/**
	 * @brief Assumes ownership of another function's properties.
	 *
	 * @param other The other function.
	 */
	TFunction(TFunction&& other) noexcept
		: m_CallableStorage { MoveTemp(other.m_CallableStorage) }
	{
		// TODO Do we need to reset the other function?
	}

	/**
	 * @brief Implicitly constructs this function from a lambda or static function.
	 *
	 * @tparam CallableType The callable type.
	 * @param callable The callable.
	 */
	template<typename CallableType>
	TFunction(CallableType callable)
	{
		if constexpr (TIsAssignable<StaticFunctionSignature&, CallableType>::Value)
		{
			InitializeFromStatic(callable);
		}
		else
		{
			InitializeFromLambda(MoveTemp(callable));
		}
	}

	/**
	 * @brief Destroys this function.
	 */
	~TFunction()
	{
		Reset();
	}

	/**
	 * @brief Creates a function object for a lambda.
	 *
	 * @tparam LambdaType The type of the lambda.
	 * @param function The lambda to wrap.
	 * @return The function object.
	 */
	template<typename LambdaType>
	static TFunction CreateLambda(LambdaType function)
	{
		TFunction result;
		result.InitializeFromLambda(MoveTemp(function));
		return result;
	}

	/**
	 * @brief Creates a function object for a raw member function.
	 *
	 * @tparam ObjectType The object type.
	 * @param object The object.
	 * @param function The function.
	 * @return The function object.
	 */
	template<typename ObjectType>
	static TFunction CreateRaw(ObjectType& object, MemberFunctionSignature<ObjectType> function)
	{
		using CallableType = TMemberCallable<ObjectType, false, false>;

		TFunction result;
		if (function == nullptr)
		{
			return result;
		}

		result.m_CallableStorage.Allocate(sizeof(CallableType));
		FMemory::ConstructObjectAt<CallableType>(result.m_CallableStorage.GetData(), &object, function);

		return result;
	}

	/**
	 * @brief Creates a function object for a raw member function.
	 *
	 * @tparam ObjectType The object type.
	 * @param object The object.
	 * @param function The function.
	 * @return The function object.
	 */
	template<typename ObjectType>
	static TFunction CreateRaw(const ObjectType& object, ConstMemberFunctionSignature<ObjectType> function)
	{
		using CallableType = TMemberCallable<ObjectType, true, false>;

		TFunction result;
		if (function == nullptr)
		{
			return result;
		}

		result.m_CallableStorage.Allocate(sizeof(CallableType));
		FMemory::ConstructObjectAt<CallableType>(result.m_CallableStorage.GetData(), &object, function);

		return result;
	}

	/**
	 * @brief Creates a function object for a shared member function.
	 *
	 * @tparam ObjectType The object type.
	 * @param object The object.
	 * @param function The function.
	 * @return The function object.
	 */
	template<typename ObjectType>
	static TFunction CreateShared(TSharedPtr<ObjectType> object, MemberFunctionSignature<ObjectType> function)
	{
		using CallableType = TMemberCallable<ObjectType, false, true>;

		TFunction result;
		if (object == nullptr || function == nullptr)
		{
			return result;
		}

		result.m_CallableStorage.Allocate(sizeof(CallableType));
		FMemory::ConstructObjectAt<CallableType>(result.m_CallableStorage.GetData(), &object, function);

		return result;
	}

	/**
	 * @brief Creates a function object for a shared member function.
	 *
	 * @tparam ObjectType The object type.
	 * @param object The object.
	 * @param function The function.
	 * @return The function object.
	 */
	template<typename ObjectType>
	static TFunction CreateShared(TSharedPtr<const ObjectType> object, ConstMemberFunctionSignature<ObjectType> function)
	{
		using CallableType = TMemberCallable<ObjectType, true, true>;

		TFunction result;
		if (object == nullptr || function == nullptr)
		{
			return result;
		}

		result.m_CallableStorage.Allocate(sizeof(CallableType));
		FMemory::ConstructObjectAt<CallableType>(result.m_CallableStorage.GetData(), &object, function);

		return result;
	}

	/**
	 * @brief Creates a function object for a static function (or non-capturing lambda).
	 *
	 * @param function The static function (or non-capturing lambda) to wrap.
	 * @return The function object.
	 */
	static TFunction CreateStatic(StaticFunctionSignature function)
	{
		TFunction result;
		result.InitializeFromStatic(function);
		return result;
	}

	/**
	 * @brief Invokes this function. Will assert if this function is not valid.
	 *
	 * @param args The arguments to supply to the function.
	 * @return The result of invoking the function.
	 */
	[[nodiscard]] ReturnType Invoke(ArgTypes... args)
	{
		UM_ASSERT(IsValid(), "Attempting to invoke invalid function");

		ICallable* callable = m_CallableStorage.GetTypedData<ICallable>();
		return callable->Invoke(Forward<ArgTypes>(args)...);
	}

	/**
	 * @brief Checks to see if this function is valid. A valid function can be invoked.
	 *
	 * @return True if this function is valid, otherwise false.
	 */
	[[nodiscard]] bool IsValid() const
	{
		const ICallable* callable = m_CallableStorage.GetTypedData<ICallable>();
		return callable != nullptr && callable->IsValid();
	}

	/**
	 * @brief Resets this function, removing the wrapped function.
	 */
	void Reset()
	{
		if (ICallable* callable = m_CallableStorage.GetTypedData<ICallable>())
		{
			FMemory::DestructObject(callable);
		}

		m_CallableStorage.Free();
	}

	/**
	 * @brief Invokes this function. Shorthand for calling Invoke.
	 *
	 * @param args The arguments to supply to the function.
	 * @return The result of invoking the function.
	 */
	[[nodiscard]] ReturnType operator()(ArgTypes... args)
	{
		return Invoke(Forward<ArgTypes>(args)...);
	}

	/**
	 * @brief Assumes ownership of another function's properties.
	 *
	 * @param other The other function.
	 * @return This function.
	 */
	TFunction& operator=(TFunction&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}

		Reset();
		m_CallableStorage = MoveTemp(other.m_CallableStorage);

		// TODO Do we need to reset the other function?

		return *this;
	}

private:

	/**
	 * @brief Defines the base interface for callable implementations of this function type.
	 */
	class ICallable
	{
	public:

		virtual ~ICallable() = default;
		[[nodiscard]] virtual ReturnType Invoke(ArgTypes... args) = 0;
		[[nodiscard]] virtual bool IsValid() const = 0;
	};

	/**
	 * @brief Defines a callable for lambdas.
	 *
	 * @tparam LambdaType The type of the lambda.
	 */
	template<typename LambdaType>
	class TLambdaCallable final : public ICallable
	{
	public:

		/**
		 * @brief Sets default values for this lambda callable's properties.
		 *
		 * @param lambda The underlying lambda.
		 */
		explicit TLambdaCallable(LambdaType&& lambda)
			: m_Lambda { MoveTemp(lambda) }
		{
		}

		/**
		 * @brief Destroys this lambda callable.
		 */
		virtual ~TLambdaCallable() = default;

		/**
		 * @brief Invokes the underlying lambda.
		 *
		 * @param args The arguments.
		 * @return The result of invoking the lambda.
		 */
		virtual ReturnType Invoke(ArgTypes... args) override
		{
			return m_Lambda(Forward<ArgTypes>(args)...);
		}

		/**
		 * @brief Checks to see if this lambda callable is valid.
		 *
		 * @return True.
		 */
		virtual bool IsValid() const override
		{
			return true;
		}

	private:

		LambdaType m_Lambda;
	};

	/**
	 * @brief Defines a callable for member functions.
	 *
	 * @tparam ObjectType The object type.
	 * @tparam IsConst Whether or not the member function is constant.
	 */
	template<typename ObjectType, bool IsConst, bool IsShared>
	class TMemberCallable final : public ICallable
	{
	public:

		using FunctionSignature = SelectType<IsConst, ConstMemberFunctionSignature<ObjectType>, MemberFunctionSignature<ObjectType>>;
		using RawObjectPointerType = SelectType<IsConst, const ObjectType*, ObjectType*>;
		using SharedObjectPointerType = SelectType<IsConst, TSharedPtr<const ObjectType>, TSharedPtr<ObjectType>>;
		using ObjectRefType = SelectType<IsShared, SharedObjectPointerType, RawObjectPointerType>;

		/**
		 * @brief Sets default values for this raw member callable's properties.
		 *
		 * @param object The target object.
		 * @param function The target function.
		 */
		TMemberCallable(ObjectRefType object, FunctionSignature function)
			: m_Object { object }
			, m_Function { function }
		{
		}

		/**
		 * @brief Destroys this member callable.
		 */
		virtual ~TMemberCallable() = default;

		/**
		 * @brief Invokes the underlying member function.
		 *
		 * @param args The arguments.
		 * @return The result of invoking the member function.
		 */
		virtual ReturnType Invoke(ArgTypes... args) override
		{
			return (m_Object->*m_Function)(Forward<ArgTypes>(args)...);
		}

		/**
		 * @brief Checks to see if this member callable is valid.
		 *
		 * @return True if this member callable is valid, otherwise false.
		 */
		virtual bool IsValid() const override
		{
			return m_Object != nullptr;
		}

	private:

		ObjectRefType m_Object = nullptr;
		FunctionSignature m_Function = nullptr;
	};

	/**
	 * @brief Defines a callable for static functions.
	 */
	class FStaticCallable final : public ICallable
	{
	public:

		/**
		 * @brief Sets default values for this static callable's properties.
		 *
		 * @param function The underlying function.
		 */
		explicit FStaticCallable(StaticFunctionSignature function)
			: m_Function { function }
		{
		}

		/**
		 * @brief Destroys this static callable.
		 */
		virtual ~FStaticCallable() = default;

		/**
		 * @brief Invokes the underlying function.
		 *
		 * @param args The arguments.
		 * @return The result of invoking the function.
		 */
		virtual ReturnType Invoke(ArgTypes... args) override
		{
			return m_Function(Forward<ArgTypes>(args)...);
		}

		/**
		 * @brief Checks to see if this static callable is valid.
		 *
		 * @return True.
		 */
		virtual bool IsValid() const override
		{
			return true;
		}

	private:

		StaticFunctionSignature m_Function = nullptr;
	};

	/**
	 * @brief Initializes this function with a lambda.
	 *
	 * @tparam LambdaType The lambda's type.
	 * @param lambda The lambda.
	 */
	template<typename LambdaType>
	void InitializeFromLambda(LambdaType lambda)
	{
		static_assert(TIsCallable<ReturnType, LambdaType, ArgTypes...>::Value, "Given lambda does not match this function's signature");

		using CallableType = TLambdaCallable<LambdaType>;

		m_CallableStorage.Allocate(sizeof(CallableType));
		FMemory::ConstructObjectAt<CallableType>(m_CallableStorage.GetData(), MoveTemp(lambda));
	}

	/**
	 * @brief Initializes this function from a static function.
	 *
	 * @param function The static function.
	 */
	void InitializeFromStatic(StaticFunctionSignature function)
	{
		if (function == nullptr)
		{
			return;
		}

		m_CallableStorage.Allocate(sizeof(FStaticCallable));
		FMemory::ConstructObjectAt<FStaticCallable>(m_CallableStorage.GetData(), MoveTemp(function));
	}

	// TODO Somehow get this to work with a unique pointer?
	FPotentiallySmallBuffer m_CallableStorage;
};

template<typename ReturnType, typename... ArgTypes>
struct TIsZeroConstructible<TFunction<ReturnType(ArgTypes...)>> : TIsZeroConstructible<FPotentiallySmallBuffer>
{
};