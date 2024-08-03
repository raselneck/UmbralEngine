#pragma once

#include "Engine/Hashing.h"
#include "Templates/IsCallable.h"
#include "Templates/IsHashable.h"
#include "Templates/IsZeroConstructible.h"
#include "Templates/Move.h"
#include "Templates/TypeTraits.h"

/**
 * @brief Defines a tuple visitor that can be constructed from several visit functors at once.
 *
 * @tparam Types The visit functor types.
 */
template<typename... Types>
struct FTupleVisitor : Types...
{
	using Types::operator() ...;
};

// Explicit deduction guide for the tuple visitor
template<typename... Types> FTupleVisitor(Types...) -> FTupleVisitor<Types...>;

/**
 * @brief Defines a container for a set of unnamed values.
 *
 * @tparam FirstType The first type in the tuple.
 * @tparam OtherTypes The remaining types in the tuple.
 */
template<typename FirstType, typename... OtherTypes>
class TTuple : public TTuple<OtherTypes...>
{
	static_assert(TNot<TIsVoid<FirstType>>::Value, "Tuples cannot store void");
	static_assert(TNot<TIsReference<FirstType>>::Value, "Tuples cannot store reference types");

	using Super = TTuple<OtherTypes...>;

public:

	/**
	 * @brief The number of types in this tuple.
	 */
	static constexpr int32 NumTypes = 1 + sizeof...(OtherTypes);

	/**
	 * @brief Sets default values for this tuple's properties.
	 */
	TTuple()
		requires TAnd<TIsDefaultConstructible<FirstType>, TIsDefaultConstructible<OtherTypes>...>::Value
		: Super { }
		, m_Value { }
	{
	}

	/**
	 * @brief Sets default values for this tuple's properties.
	 *
	 * @param value The first value.
	 * @param otherValues The remaining values.
	 */
	explicit TTuple(FirstType value, OtherTypes... otherValues)
		: Super(MoveTemp(otherValues)...)
		, m_Value { MoveTemp(value) }
	{
	}

	/**
	 * @brief Gets this tuple's hash code.
	 *
	 * @return This tuple's hash code.
	 */
	[[nodiscard]] uint64 GetHashCode() const
		requires TIsHashable<FirstType>::Value
	{
		return Private::HashCombine(::GetHashCode(m_Value), Super::GetHashCode());
	}

	/**
	 * @brief Gets the underlying value.
	 *
	 * @return The underlying value.
	 */
	[[nodiscard]] const FirstType& GetValue() const
	{
		return m_Value;
	}

	/**
	 * @brief Gets the underlying value.
	 *
	 * @return The underlying value.
	 */
	[[nodiscard]] FirstType& GetValue()
	{
		return m_Value;
	}

	/**
	 * @brief Visits all values in this tuple.
	 *
	 * @tparam VisitorType The type of the visitor.
	 * @param visitor The visitor.
	 */
	template<typename VisitorType>
	void Visit(VisitorType visitor) const
		requires TIsCallable<void, VisitorType, FirstType>::Value
	{
		visitor(m_Value);
		Super::Visit(MoveTemp(visitor));
	}

	/**
	 * @brief Visits all values in this tuple.
	 *
	 * @tparam VisitorType The type of the visitor.
	 * @param visitor The visitor.
	 */
	template<typename VisitorType>
	void Visit(VisitorType visitor)
		requires TIsCallable<void, VisitorType, FirstType>::Value
	{
		visitor(m_Value);
		Super::Visit(MoveTemp(visitor));
	}

private:

	FirstType m_Value;
};

/**
 * @brief Defines a container for a set of unnamed values.
 *
 * @tparam FirstType The first type in the tuple.
 * @tparam OtherTypes The remaining types in the tuple.
 */
template<typename FirstType>
class TTuple<FirstType>
{
	static_assert(TNot<TIsVoid<FirstType>>::Value, "Tuples cannot store void");
	static_assert(TNot<TIsReference<FirstType>>::Value, "Tuples cannot store reference types");

public:

	/**
	 * @brief The number of types in this tuple.
	 */
	static constexpr int32 NumTypes = 1;

	/**
	 * @brief Sets default values for this tuple's properties.
	 */
	TTuple()
		requires TIsDefaultConstructible<FirstType>::Value
		: m_Value { }
	{
	}

	/**
	 * @brief Sets default values for this tuple's properties.
	 *
	 * @param value The value.
	 */
	explicit TTuple(FirstType value)
		: m_Value { MoveTemp(value) }
	{
	}

	/**
	 * @brief Gets this tuple's hash code.
	 *
	 * @return This tuple's hash code.
	 */
	[[nodiscard]] uint64 GetHashCode() const
		requires TIsHashable<FirstType>::Value
	{
		return ::GetHashCode(m_Value);
	}

	/**
	 * @brief Gets the underlying value.
	 *
	 * @return The underlying value.
	 */
	[[nodiscard]] const FirstType& GetValue() const
	{
		return m_Value;
	}

	/**
	 * @brief Gets the underlying value.
	 *
	 * @return The underlying value.
	 */
	[[nodiscard]] FirstType& GetValue()
	{
		return m_Value;
	}

	/**
	 * @brief Visits all values in this tuple.
	 *
	 * @tparam VisitorType The type of the visitor.
	 * @param visitor The visitor.
	 */
	template<typename VisitorType>
	void Visit(VisitorType visitor) const
		requires TIsCallable<void, VisitorType, FirstType>::Value
	{
		visitor(m_Value);
	}

	/**
	 * @brief Visits all values in this tuple.
	 *
	 * @tparam VisitorType The type of the visitor.
	 * @param visitor The visitor.
	 */
	template<typename VisitorType>
	void Visit(VisitorType visitor)
		requires TIsCallable<void, VisitorType, FirstType>::Value
	{
		visitor(m_Value);
	}

private:

	FirstType m_Value;
};

template<typename FirstType, typename... OtherTypes>
struct TIsHashable<TTuple<FirstType, OtherTypes...>> : TAnd<TIsHashable<FirstType>, TIsHashable<OtherTypes>...>
{
};

template<typename FirstType, typename... OtherTypes>
struct TIsZeroConstructible<TTuple<FirstType, OtherTypes...>> : TAnd<TIsZeroConstructible<FirstType>, TIsZeroConstructible<OtherTypes>...>
{
};

/**
 * @brief Gets the hash code for the given tuple.
 *
 * @tparam FirstType The first type in the tuple.
 * @tparam OtherTypes The other types in the tuple.
 * @param value The tuple value.
 * @return The hash code.
 */
template<typename FirstType, typename... OtherTypes>
inline uint64 GetHashCode(const TTuple<FirstType, OtherTypes...>& value)
	requires TIsHashable<TTuple<FirstType, OtherTypes...>>::Value
{
	return value.GetHashCode();
}

namespace Private
{
	template<int32 Index, typename FirstType, typename... OtherTypes>
	struct TTupleValueGetter
	{
		static_assert(Index >= 0, "Invalid tuple index given");

		static auto Get(const TTuple<FirstType, OtherTypes...>* tuple)
			-> decltype(TTupleValueGetter<Index - 1, OtherTypes...>::Get(tuple))
		{
			return TTupleValueGetter<Index - 1, OtherTypes...>::Get(tuple);
		}

		static auto Get(TTuple<FirstType, OtherTypes...>* tuple)
			-> decltype(TTupleValueGetter<Index - 1, OtherTypes...>::Get(tuple))
		{
			return TTupleValueGetter<Index - 1, OtherTypes...>::Get(tuple);
		}
	};

	template<typename FirstType, typename... OtherTypes>
	struct TTupleValueGetter<0, FirstType, OtherTypes...>
	{
		static const FirstType& Get(const TTuple<FirstType, OtherTypes...>* tuple)
		{
			return tuple->GetValue();
		}

		static FirstType& Get(TTuple<FirstType, OtherTypes...>* tuple)
		{
			return tuple->GetValue();
		}
	};

	template<int32 Index, typename FirstType, typename... OtherTypes>
	struct TTupleTypeAtIndex
	{
		using Type = typename TVariadicTypeAtIndex<static_cast<usize>(Index), FirstType, OtherTypes...>::Type;
		using ReferenceType = typename TAddLValueReference<Type>::Type;
		using ConstReferenceType = typename TAddConst<ReferenceType>::Type;
	};
}

/**
 * @brief Gets the value of a tuple at the given index.
 *
 * @tparam Index The index.
 * @tparam FirstType The first type in the tuple.
 * @tparam OtherTypes The other types in the tuple.
 * @param tuple The tuple.
 * @return The value at \p Index.
 */
template<int32 Index, typename FirstType, typename... OtherTypes>
inline auto Get(const TTuple<FirstType, OtherTypes...>& tuple)
	-> typename Private::TTupleTypeAtIndex<Index, FirstType, OtherTypes...>::ConstReferenceType
{
	return Private::TTupleValueGetter<Index, FirstType, OtherTypes...>::Get(&tuple);
}

/**
 * @brief Gets the value of a tuple at the given index.
 *
 * @tparam Index The index.
 * @tparam FirstType The first type in the tuple.
 * @tparam OtherTypes The other types in the tuple.
 * @param tuple The tuple.
 * @return The value at \p Index.
 */
template<int32 Index, typename FirstType, typename... OtherTypes>
inline auto Get(TTuple<FirstType, OtherTypes...>& tuple)
	-> typename Private::TTupleTypeAtIndex<Index, FirstType, OtherTypes...>::ReferenceType
{
	return Private::TTupleValueGetter<Index, FirstType, OtherTypes...>::Get(&tuple);
}