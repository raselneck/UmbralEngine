#pragma once

#include "Engine/Assert.h"
#include "Engine/Hashing.h"
#include "Memory/AlignedStorage.h"
#include "Memory/Memory.h"
#include "Templates/BulkOperations.h"
#include "Templates/IsHashable.h"
#include "Templates/NumericLimits.h"
#include "Templates/TypeTraits.h"

// NOTE The implementation here is based off of this public domain gist:
//      https://gist.github.com/calebh/fd00632d9c616d4b0c14e7c2865f3085

namespace Private
{
	using FVariantSizeType = int8;

	template<typename ReturnType>
	struct TDefaultValueHelper
	{
		static ReturnType Get() { return {}; }
	};

	template<>
	struct TDefaultValueHelper<void>
	{
		static void Get() { }
	};

	template<FVariantSizeType N, typename ... OtherTypes>
	class TIndexedVariantHelper;

	/**
	 * @brief Defines an explicitly-indexed helper for variant operations.
	 *
	 * @tparam N The index of the type to act on.
	 * @tparam FirstType The first type.
	 * @tparam OtherTypes The other types.
	 */
	template<FVariantSizeType N, typename FirstType, typename ... OtherTypes>
	class TIndexedVariantHelper<N, FirstType, OtherTypes...>
	{
	public:

		/**
		 * @brief Copies variant data from one location to another.
		 *
		 * @param id The variant type ID.
		 * @param source The source variant data.
		 * @param destination The destination variant data.
		 */
		static void Copy(FVariantSizeType id, const void* source, void* destination)
		{
			if (N == id)
			{
				FMemory::ConstructObjectAt<FirstType>(destination, *reinterpret_cast<const FirstType*>(source));
			}
			else
			{
				TIndexedVariantHelper<N + 1, OtherTypes...>::Copy(id, source, destination);
			}
		}

		/**
		 * @brief Destroys variant data with the given ID.
		 *
		 * @param id The variant type ID.
		 * @param data The variant data.
		 */
		static void Destroy(FVariantSizeType id, void* data)
		{
			if (N == id)
			{
				DestructElements(reinterpret_cast<FirstType*>(data), 1);
				FMemory::ZeroOut(data, sizeof(FirstType));
			}
			else
			{
				TIndexedVariantHelper<N + 1, OtherTypes...>::Destroy(id, data);
			}
		}

		/**
		 * @brief Gets the hash code for the variant data with the given ID.
		 *
		 * @param id The variant type ID.
		 * @param data The variant data.
		 * @return The hash code.
		 */
		static uint64 GetHashCode(FVariantSizeType id, const void* data)
			requires TIsHashable<FirstType>::Value
		{
			if (N == id)
			{
				return GetHashCode(*reinterpret_cast<const FirstType*>(data));
			}
			else
			{
				return TIndexedVariantHelper<N + 1, OtherTypes...>::GetHashCode(id, data);
			}
		}

		/**
		 * @brief Moves variant data from one location to another.
		 *
		 * @param id The variant type ID.
		 * @param source The source variant data.
		 * @param destination The destination variant data.
		 */
		static void Move(FVariantSizeType id, void* source, void* destination)
		{
			if (N == id)
			{
				FMemory::ConstructObjectAt<FirstType>(destination, MoveTemp(*reinterpret_cast<FirstType*>(source)));
			}
			else
			{
				TIndexedVariantHelper<N + 1, OtherTypes...>::Move(id, source, destination);
			}
		}

		/**
		 * @brief Visits a variant's value.
		 *
		 * @tparam VisitorType The type of the visitor.
		 * @tparam ReturnType The type that the visitor returns.
		 * @param id The variant type ID.
		 * @param visitor The visitor.
		 * @param data The variant's value data.
		 * @return The result of the visitor.
		 */
		template<typename VisitorType, typename ReturnType>
		static ReturnType VisitImmutable(FVariantSizeType id, VisitorType visitor, const void* data)
		{
			if (N == id)
			{
				return visitor(*reinterpret_cast<const FirstType*>(data));
			}
			else
			{
				return TIndexedVariantHelper<N + 1, OtherTypes...>::template VisitImmutable<VisitorType, ReturnType>(id, MoveTemp(visitor), data);
			}
		}

		/**
		 * @brief Visits a variant's value.
		 *
		 * @tparam VisitorType The type of the visitor.
		 * @tparam ReturnType The type that the visitor returns.
		 * @param id The variant type ID.
		 * @param visitor The visitor.
		 * @param data The variant's value data.
		 * @return The result of the visitor.
		 */
		template<typename VisitorType, typename ReturnType>
		static ReturnType VisitMutable(FVariantSizeType id, VisitorType visitor, void* data)
		{
			if (N == id)
			{
				return visitor(*reinterpret_cast<FirstType*>(data));
			}
			else
			{
				return TIndexedVariantHelper<N + 1, OtherTypes...>::template VisitMutable<VisitorType, ReturnType>(id, MoveTemp(visitor), data);
			}
		}
	};

	template<FVariantSizeType N>
	class TIndexedVariantHelper<N>
	{
	public:

		static void Copy(FVariantSizeType, const void*, void*)
		{
			UM_ASSERT_NOT_REACHED();
		}

		static void Destroy(FVariantSizeType, void*)
		{
			UM_ASSERT_NOT_REACHED();
		}

		static uint64 GetHashCode(FVariantSizeType, const void*)
		{
			UM_ASSERT_NOT_REACHED();
			return 0;
		}

		static void Move(FVariantSizeType, void*, void*)
		{
			UM_ASSERT_NOT_REACHED();
		}

		template<typename VisitorType, typename ReturnType>
		static ReturnType VisitImmutable(FVariantSizeType, VisitorType, const void*)
		{
			UM_ASSERT_NOT_REACHED();
		}

		template<typename VisitorType, typename ReturnType>
		static ReturnType VisitMutable(FVariantSizeType, VisitorType, void*)
		{
			UM_ASSERT_NOT_REACHED();
		}
	};

	/**
	 * @brief Defines a helper for variants.
	 *
	 * @tparam Types The variant types.
	 */
	template<typename... Types>
	class TVariantHelper
	{
	public:

		/**
		 * @brief Copies variant data from one location to another.
		 *
		 * @param id The variant type ID.
		 * @param source The source variant data.
		 * @param destination The destination variant data.
		 */
		static void Copy(FVariantSizeType id, const void* source, void* destination)
		{
			TIndexedVariantHelper<0, Types...>::Copy(id, source, destination);
		}

		/**
		 * @brief Destroys variant data with the given ID.
		 *
		 * @param id The variant type ID.
		 * @param data The variant data.
		 */
		static void Destroy(FVariantSizeType id, void* data)
		{
			TIndexedVariantHelper<0, Types...>::Destroy(id, data);
		}

		/**
		 * @brief Gets the hash code for the variant data with the given ID.
		 *
		 * @param id The variant type ID.
		 * @param data The variant data.
		 * @return The hash code.
		 */
		static uint64 GetHashCode(FVariantSizeType id, const void* data)
		{
			return TIndexedVariantHelper<0, Types...>::GetHashCode(id, data);
		}

		/**
		 * @brief Moves variant data from one location to another.
		 *
		 * @param id The variant type ID.
		 * @param source The source variant data.
		 * @param destination The destination variant data.
		 */
		static void Move(FVariantSizeType id, void* source, void* destination)
		{
			TIndexedVariantHelper<0, Types...>::Move(id, source, destination);
		}

		/**
		 * @brief Visits a variant's value.
		 *
		 * @tparam VisitorType The type of the visitor.
		 * @tparam ReturnType The type that the visitor returns.
		 * @param id The variant type ID.
		 * @param visitor The visitor.
		 * @param data The variant's value data.
		 * @return The result of the visitor.
		 */
		template<typename VisitorType, typename ReturnType>
		static ReturnType VisitImmutable(FVariantSizeType id, VisitorType visitor, const void* data)
		{
			return TIndexedVariantHelper<0, Types...>::template VisitImmutable<VisitorType, ReturnType>(id, MoveTemp(visitor), data);
		}

		/**
		 * @brief Visits a variant's value.
		 *
		 * @tparam VisitorType The type of the visitor.
		 * @tparam ReturnType The type that the visitor returns.
		 * @param id The variant type ID.
		 * @param visitor The visitor.
		 * @param data The variant's value data.
		 * @return The result of the visitor.
		 */
		template<typename VisitorType, typename ReturnType>
		static ReturnType VisitMutable(FVariantSizeType id, VisitorType visitor, void* data)
		{
			return TIndexedVariantHelper<0, Types...>::template VisitMutable<VisitorType, ReturnType>(id, MoveTemp(visitor), data);
		}
	};
}

/**
 * @brief Defines a variant visitor that can be constructed from several visit functors at once.
 *
 * @tparam Types The visit functor types.
 */
template<typename... Types>
struct FVariantVisitor : Types...
{
	using Types::operator() ...;
};

// Explicit deduction guide for the variant visitor
template<typename... Types> FVariantVisitor(Types...) -> FVariantVisitor<Types...>;

template<typename FirstType, typename ... OtherTypes>
class TVariant;

template<typename T>
struct TIsVariant : FFalseType
{
};

template<typename... Types>
struct TIsVariant<TVariant<Types...>> : FTrueType
{
};

/**
 * @brief Defines a completely empty type that can be used as the first type in a variant list for "null" values.
 */
struct FEmptyType
{
};

/**
 * @brief Defines a type that can hold one of many different types.
 *
 * @tparam Types The possible types this variant can hold.
 */
template<typename FirstType, typename... OtherTypes>
class TVariant final
{
	static constexpr int32 TypeCount = 1 + sizeof...(OtherTypes);
	using IndexType = Private::FVariantSizeType;
	using HelperType = Private::TVariantHelper<FirstType, OtherTypes...>;

	static_assert(TypeCount <= TNumericLimits<IndexType>::MaxValue, "Too many types supplied to TVariant");
	static_assert(TypeCount > 0, "At least one type must be supplied to TVariant");
	static_assert(TVariadicContainsReference<FirstType, OtherTypes...>::Value == false, "TVariant cannot contain reference types");
	static_assert(TVariadicContainsDuplicate<FirstType, OtherTypes...>::Value == false, "TVariant cannot be defined with multiple of the same type");
	static_assert(TOr<TIsDefaultConstructible<FirstType>, TIsZeroConstructible<FirstType>>::Value, "The first type in a TVariant type list must be default or zero constructible");

public:

	static constexpr usize DataSize = TMaxOf<sizeof(FirstType), sizeof(OtherTypes)...>::Value;
	static constexpr usize DataAlignment = TMaxOf<alignof(FirstType), alignof(OtherTypes)...>::Value;
	using StorageType = TAlignedStorage<DataSize, DataAlignment>;

	/**
	 * @brief Sets default values for this variant's properties.
	 */
	TVariant()
		: m_ValueIndex { 0 }
	{
		if constexpr (TIsZeroConstructible<FirstType>::Value)
		{
			// Do nothing
		}
		else
		{
			FMemory::ConstructObjectAt<FirstType>(m_ValueStorage.GetData());
		}
	}

	/**
	 * @brief Copies another variant.
	 *
	 * @param other The other variant to copy.
	 */
	TVariant(const TVariant& other)
		: m_ValueIndex { other.m_ValueIndex }
	{
		if (m_ValueIndex != INDEX_NONE)
		{
			HelperType::Copy(m_ValueIndex, other.m_ValueStorage.GetData(), m_ValueStorage.GetData());
		}
	}

	/**
	 * @brief Moves another variant's value to this variant.
	 *
	 * @param other The other variant.
	 */
	TVariant(TVariant&& other) noexcept
		: m_ValueIndex { other.m_ValueIndex }
	{
		if (m_ValueIndex != INDEX_NONE)
		{
			HelperType::Move(m_ValueIndex, other.m_ValueStorage.GetData(), m_ValueStorage.GetData());
		}

		other.Reset();
	}

	/**
	 * @brief Sets default values for this variant's properties.
	 *
	 * @tparam T The initial value's type.
	 * @param value The initial value.
	 */
	template<typename T>
	explicit TVariant(const T& value)
		: m_ValueIndex { IndexOfType<T>() }
	{
		static_assert(IndexOfType<T>() != INDEX_NONE, "Variant cannot hold given type");
		static_assert(TIsVariant<T>::Value == false, "Cannot assign differing variant types to each other");

		HelperType::Copy(m_ValueIndex, &value, m_ValueStorage.GetData());
	}

	/**
	 * @brief Sets default values for this variant's properties.
	 *
	 * @tparam T The initial value's type.
	 * @param value The initial value.
	 */
	template<typename T>
	explicit TVariant(T&& value) noexcept
		: m_ValueIndex { IndexOfType<T>() }
	{
		static_assert(IndexOfType<T>() != INDEX_NONE, "Variant cannot hold given type");
		static_assert(TIsVariant<T>::Value == false, "Cannot assign differing variant types to each other");

		HelperType::Move(m_ValueIndex, &value, m_ValueStorage.GetData());
	}

	/**
	 * @brief Destroys this variant.
	 */
	~TVariant()
	{
		Reset();
	}

	/**
	 * @brief Checks to see if this variant could contain the given type.
	 *
	 * @tparam TypeToFind The type to check for.
	 * @return True if this variant could contain a value of type \p TypeToFind, otherwise false.
	 */
	template<typename TypeToFind>
	static constexpr bool CouldContainType()
	{
		return TVariadicContainsType<TypeToFind, FirstType, OtherTypes...>::Value;
	}

	/**
	 * @brief Gets this variant's hash code.
	 *
	 * @return The hash code.
	 */
	[[nodiscard]] uint64 GetHashCode() const
	{
		return HelperType::GetHashCode(m_ValueIndex, m_ValueStorage.GetData());
	}

	/**
	 * @brief Gets this variant's value as the given type.
	 *
	 * @remarks Will assert if this variant is not of the given type.
	 *
	 * @tparam T The value type.
	 * @return A copy of this variant's value.
	 */
	template<typename T>
	const T& GetValue() const
	{
		static_assert(CouldContainType<T>(), "This variant cannot possibly contain a value of the given type");

		UM_ASSERT(Is<T>(), "Can't get value of variant when it is not of type T");

		return *m_ValueStorage.template GetTypedData<T>();
	}

	/**
	 * @brief Gets this variant's value as the given type.
	 *
	 * @remarks Will assert if this variant is not of the given type.
	 *
	 * @tparam T The value type.
	 * @return A copy of this variant's value.
	 */
	template<typename T>
	T& GetValue()
	{
		static_assert(CouldContainType<T>(), "This variant cannot possibly contain a value of the given type");

		UM_ASSERT(Is<T>(), "Can't get value of variant when it is not of type T");

		return *m_ValueStorage.template GetTypedData<T>();
	}

	/**
	 * @brief Gets the type index of this variant's value.
	 *
	 * @return The type index of this variant's value.
	 */
	[[nodiscard]] constexpr IndexType GetValueIndex() const
	{
		return m_ValueIndex;
	}

	/**
	 * @brief Gets this variant's value as a given type, or an alternate value if this variant does not contain that type.
	 *
	 * @tparam T The type to try to get this variant's value as.
	 * @param alternateValue The alternate value.
	 * @return This variant's value if it is of type T, otherwise \p alternateValue.
	 */
	template<typename T>
	T GetValueOr(T alternateValue) const
	{
		static_assert(CouldContainType<T>(), "This variant cannot possibly contain a value of the given type");

		return Is<T>() ? GetValue<T>() : MoveTemp(alternateValue);
	}

	/**
	 * @brief Gets a pointer to this variant's value.
	 *
	 * @tparam T The type to get this variant's value as.
	 * @return A pointer to this variant's value if it is of type \p T, otherwise nullptr.
	 */
	template<typename T>
	const T* GetValuePointer() const
	{
		static_assert(CouldContainType<T>(), "This variant cannot possibly contain a value of the given type");
		return Is<T>() ? m_ValueStorage.template GetTypedData<T>() : nullptr;
	}

	/**
	 * @brief Gets a pointer to this variant's value.
	 *
	 * @tparam T The type to get this variant's value as.
	 * @return A pointer to this variant's value if it is of type \p T, otherwise nullptr.
	 */
	template<typename T>
	T* GetValuePointer()
	{
		static_assert(CouldContainType<T>(), "This variant cannot possibly contain a value of the given type");
		return Is<T>() ? m_ValueStorage.template GetTypedData<T>() : nullptr;
	}

	/**
	 * @brief Gets the index of the given type in this variant's type list.
	 *
	 * @tparam TypeToFind The type to find.
	 * @return The index of \p TypeToFind in this variant's type list.
	 */
	template<typename TypeToFind>
	static constexpr IndexType IndexOfType()
	{
		return TVariadicIndexOfType<TypeToFind, FirstType, OtherTypes...>::Value;
	}

	/**
	 * @brief Checks to see if this variant is of the given type.
	 *
	 * @tparam T The type to check for.
	 * @return True if this variant is of type \p T, otherwise false.
	 */
	template<typename T>
	[[nodiscard]] bool Is() const
	{
		static_assert(CouldContainType<T>(), "This variant cannot possibly contain a value of the given type");

		return IndexOfType<T>() == GetValueIndex();
	}

	/**
	 * @brief Resets this variant to have a value of the first type in its type list.
	 */
	void Reset()
	{
		HelperType::Destroy(m_ValueIndex, m_ValueStorage.GetData());
		FMemory::ConstructObjectAt<FirstType>(m_ValueStorage.GetData());
		m_ValueIndex = 0;
	}

	/**
	 * @brief Resets this variant to have a value of the type at the given index in its type list.
	 *
	 * @tparam Index The index of the desired type.
	 * @tparam ConstructTypes The types of the arguments to pass along to the constructor.
	 * @param args The arguments to pass along to the constructor.
	 */
	template<IndexType Index, typename... ConstructTypes>
	void ResetToIndex(ConstructTypes&& ... args)
	{
		// Destroy old value
		HelperType::Destroy(m_ValueIndex, m_ValueStorage.GetData());
		m_ValueIndex = INDEX_NONE;

		// Ensure we can construct the type at the given index with the given arguments
		using TypeToConstruct = typename TVariadicTypeAtIndex<Index, FirstType, OtherTypes...>::Type;
		static_assert(TIsConstructible<TypeToConstruct, ConstructTypes...>::Value, "Invalid constructor arguments passed for type");

		// Construct new value
		FMemory::ConstructObjectAt<TypeToConstruct>(m_ValueStorage.GetData(), Forward<ConstructTypes>(args)...);
		m_ValueIndex = Index;
	}

	/**
	 * @brief Resets this variant's value to be the given type.
	 *
	 * @tparam Type The type to reset the value to.
	 * @tparam ConstructTypes The types of the arguments to pass along to the constructor of \p Type.
	 * @param args The arguments to pass along to the constructor of \p Type.
	 */
	template<typename Type, typename... ConstructTypes>
	[[maybe_unused]] Type& ResetToType(ConstructTypes&& ... args)
	{
		static_assert(CouldContainType<Type>(), "This variant cannot possibly contain a value of the given type");

		ResetToIndex<IndexOfType<Type>()>(Forward<ConstructTypes>(args)...);

		return GetValue<Type>();
	}

	/**
	 * @brief Attempts to get this variant's value as the given type.
	 *
	 * @tparam T The type to try to get.
	 * @param result The result value.
	 * @return True if the value was retrieved, otherwise false.
	 */
	template<typename T>
	[[nodiscard]] bool TryGetValue(T& result) const
	{
		static_assert(CouldContainType<T>(), "This variant cannot possibly contain a value of the given type");

		constexpr IndexType typeIndex = IndexOfType<T>();
		if (typeIndex == INDEX_NONE)
		{
			return false;
		}

		HelperType::Copy(m_ValueIndex, m_ValueStorage.GetData(), &result);

		return true;
	}

	/**
	 * @brief Visits this variant's value.
	 *
	 * @tparam VisitorType The type of the visitor.
	 * @tparam ReturnType The return type of the visitors.
	 * @param visitor The visitor.
	 * @return The return value dictated by the visitor.
	 */
	template<typename ReturnType=void, typename VisitorType=void>
	ReturnType Visit(VisitorType visitor) const
	{
		return HelperType::template VisitImmutable<VisitorType, ReturnType>(m_ValueIndex, MoveTemp(visitor), m_ValueStorage.GetData());
	}

	/**
	 * @brief Visits this variant's value.
	 *
	 * @tparam VisitorType The type of the visitor.
	 * @tparam ReturnType The return type of the visitors.
	 * @param visitor The visitor.
	 * @return The return value dictated by the visitor.
	 */
	template<typename ReturnType=void, typename VisitorType=void>
	ReturnType Visit(VisitorType visitor)
	{
		return HelperType::template VisitMutable<VisitorType, ReturnType>(m_ValueIndex, MoveTemp(visitor), m_ValueStorage.GetData());
	}

	/**
	 * @brief Copies another variant.
	 *
	 * @param other The other variant to copy.
	 * @return This variant.
	 */
	TVariant& operator=(const TVariant& other)
	{
		if (&other == this)
		{
			return *this;
		}

		Reset();

		m_ValueIndex = other.m_ValueIndex;
		HelperType::Copy(m_ValueIndex, other.m_ValueStorage.GetData(), m_ValueStorage.GetData());

		return *this;
	}

	/**
	 * @brief Moves another variant's value to this variant.
	 *
	 * @param other The other variant.
	 * @return This variant.
	 */
	TVariant& operator=(TVariant&& other) noexcept
	{
		if (&other == this)
		{
			return *this;
		}

		Reset();

		m_ValueIndex = other.m_ValueIndex;
		HelperType::Move(m_ValueIndex, other.m_ValueStorage.GetData(), m_ValueStorage.GetData());

		other.Reset();

		return *this;
	}

	/**
	 * @brief Sets this variant's value.
	 *
	 * @tparam T The new value's type.
	 * @param value The new value.
	 */
	template<typename T>
	TVariant& operator=(const T& value)
	{
		static_assert(CouldContainType<T>(), "This variant cannot possibly contain a value of the given type");
		static_assert(TIsVariant<T>::Value == false, "Cannot assign differing variant types to each other");

		Reset();

		m_ValueIndex = IndexOfType<T>();
		HelperType::Copy(m_ValueIndex, &value, m_ValueStorage.GetData());

		return *this;
	}

	/**
	 * @brief Sets this variant's value.
	 *
	 * @tparam T The new value's type.
	 * @param value The new value.
	 */
	template<typename T>
	TVariant& operator=(T&& value)
	{
		static_assert(CouldContainType<T>(), "This variant cannot possibly contain a value of the given type");
		static_assert(TIsVariant<T>::Value == false, "Cannot assign differing variant types to each other");

		Reset();

		m_ValueIndex = IndexOfType<T>();
		HelperType::Move(m_ValueIndex, &value, m_ValueStorage.GetData());

		return *this;
	}

private:

	StorageType m_ValueStorage;
	IndexType m_ValueIndex = 0;
};

template<typename FirstType, typename... OtherTypes>
struct TIsZeroConstructible<TVariant<FirstType, OtherTypes...>> : TIsZeroConstructible<typename TVariant<FirstType, OtherTypes...>::StorageType>
{
};

template<typename FirstType, typename... OtherTypes>
struct TIsHashable<TVariant<FirstType, OtherTypes...>> : TAnd<TIsHashable<FirstType>, TIsHashable<OtherTypes>...>
{
};

/**
 * @brief Gets the hash code for the given variant value.
 *
 * @tparam FirstType The first type in the variant.
 * @tparam OtherTypes The other types in the variant.
 * @param value The variant value.
 * @return The hash code.
 */
template<typename FirstType, typename... OtherTypes>
inline uint64 GetHashCode(const TVariant<FirstType, OtherTypes...>& value)
	requires TIsHashable<TVariant<FirstType, OtherTypes...>>::Value
{
	return value.GetHashCode();
}