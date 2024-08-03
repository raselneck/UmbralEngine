#pragma once

#include "Containers/Variant.h"
#include "Engine/Hashing.h"
#include "Memory/Memory.h"
#include "Memory/SmallBufferStorage.h"
#include "Meta/PointerTypeInfo.h"
#include "Meta/TypeInfo.h"
#include "Templates/BulkOperations.h"
#include "Templates/Decay.h"
#include "Templates/IsHashable.h"
#include "Templates/TypeTraits.h"

class FAny;

template<typename T>
using TCanBeStoredInAny = TNot<TOr<
	TIsSame<T, FAny>, // FAny cannot store an FAny
	TIsReference<T>,  // FAny cannot store references, but it CAN store pointers
	TIsVoid<T>        // FAny cannot store void
>>;

template<typename T>
inline constexpr bool CanBeStoredInAny = TCanBeStoredInAny<T>::Value;

/**
 * @brief Defines a way to hold any type.
 */
class FAny final
{
public:

	/**
	 * @brief Constructs a new, empty "any" value.
	 */
	FAny() = default;

	/**
	 * @brief Copies another "any" value.
	 *
	 * @param other The other value to copy.
	 */
	FAny(const FAny& other);

	/**
	 * @brief Assumes ownership of another "any" value's data.
	 *
	 * @param other The other value to assume ownership of.
	 */
	FAny(FAny&& other) noexcept;

	/**
	 * @brief Creates an "any" value wrapping the given value.
	 *
	 * @tparam T The type of the value to wrap.
	 * @param value The value to wrap.
	 */
	template<typename T>
	requires(CanBeStoredInAny<T>)
	explicit FAny(const T& value)
	{
		SetValue<T>(value);
	}

	/**
	 * @brief Creates an "any" value wrapping the given value.
	 *
	 * @tparam T The type of the value to wrap.
	 * @param value The value to wrap.
	 */
	template<typename T>
	requires(CanBeStoredInAny<T>)
	explicit FAny(T&& value)
	{
		SetValue<T>(MoveTemp(value));
	}

	/**
	 * @brief Destroys this value.
	 */
	~FAny();

	/**
	 * @brief Constructs a value in-place in this FAny value.
	 *
	 * @tparam ValueType The value type.
	 * @tparam ConstructTypes The types of the arguments to pass along to the value's constructor.
	 * @param args The arguments to pass along to the value's constructor.
	 */
	template<typename ValueType, typename... ConstructTypes>
	requires(CanBeStoredInAny<ValueType>)
	void Emplace(ConstructTypes&& ... args)
	{
		Reset();

		using DecayedType = typename TDecay<ValueType>::Type;

		void* valueLocation = GetOrAllocateValuePointer(sizeof(DecayedType));
		FMemory::ConstructObjectAt<DecayedType>(valueLocation, Forward<ConstructTypes>(args)...);

		m_ValueTypeFunction = DefaultValueFunction<DecayedType>;
	}

	/**
	 * @brief Gets the hash code for the contained value.
	 *
	 * @return The hash code for the contained value.
	 */
	[[nodiscard]] uint64 GetHashCode() const
	{
		return IsEmpty() ? 0 : m_ValueTypeFunction(EAnyValueOperation::Hash, GetValuePointer(), nullptr);
	}

	/**
	 * @brief Gets a pointer to the underlying value.
	 *
	 * @tparam T The type to get the underlying value of.
	 * @return A pointer to the underlying value if it is of type T, otherwise nullptr.
	 */
	template<typename T>
	requires(CanBeStoredInAny<T>)
	[[nodiscard]] const T* GetValue() const
	{
		return Is<T>() ? static_cast<const T*>(GetValuePointer()) : nullptr;
	}

	/**
	 * @brief Gets a pointer to the underlying value.
	 *
	 * @tparam T The type to get the underlying value of.
	 * @return A pointer to the underlying value if it is of type T, otherwise nullptr.
	 */
	template<typename T>
	requires(CanBeStoredInAny<T>)
	[[nodiscard]] T* GetValue()
	{
		return Is<T>() ? static_cast<T*>(GetValuePointer()) : nullptr;
	}

	/**
	 * @brief Gets the underlying value's type.
	 *
	 * @return The underlying value's type, or nullptr if there is no value.
	 */
	[[nodiscard]] const FTypeInfo* GetValueType() const;

	/**
	 * @brief Checks to see if this "any" value has an underlying value.
	 *
	 * @return True if this "any" value has an underlying value, otherwise false.
	 */
	[[nodiscard]] constexpr bool HasValue() const
	{
		return m_ValueTypeFunction != nullptr;
	}

	/**
	 * @brief Checks to see if this "any" value is of the given underlying type.
	 *
	 * @tparam T The type to check for.
	 * @return True if the underlying type is of the given type, otherwise false.
	 */
	template<typename T>
	requires(CanBeStoredInAny<T>)
	[[nodiscard]] bool Is() const
	{
		using DecayedType = typename TDecay<T>::Type;

		return HasValue() && ::GetType<DecayedType>() == GetValueType();
	}

	/**
	 * @brief Checks to see if this "any" value is empty.
	 *
	 * @return True if this "any" value is empty, otherwise false.
	 */
	[[nodiscard]] bool IsEmpty() const
	{
		return m_ValueTypeFunction == nullptr;
	}

	/**
	 * @brief Releases the underlying value if it is the given type.
	 *
	 * @tparam T The type.
	 * @return The underlying value as the given type.
	 */
	template<typename T>
	requires(CanBeStoredInAny<T>)
	T ReleaseValue()
	{
		UM_ASSERT(Is<T>(), "Cannot release FAny value for mismatched type");

		T value = MoveTemp(*GetValue<T>());
		Reset();

		return value;
	}

	/**
	 * @brief Destroys the underlying value, if one exists.
	 */
	void Reset();

	/**
	 * @brief Sets the underlying value.
	 *
	 * @tparam T The type of the value.
	 * @param value The value.
	 */
	template<typename T>
	requires(CanBeStoredInAny<T>)
	void SetValue(const T& value)
	{
		using DecayedType = typename TDecay<T>::Type;
		SetValueImpl<DecayedType>(value);
	}

	/**
	 * @brief Sets the underlying value.
	 *
	 * @tparam T The type of the value.
	 * @param value The value.
	 */
	template<typename T>
	requires(CanBeStoredInAny<T>)
	void SetValue(T&& value)
	{
		using DecayedType = typename TDecay<T>::Type;
		SetValueImpl<DecayedType>(MoveTemp(value));
	}

	/**
	 * @brief Copies another "any" value's value.
	 *
	 * @param other The other "any" value.
	 * @return This "any" value.
	 */
	FAny& operator=(const FAny& other);

	/**
	 * @brief Assumes ownership of another "any" value's value.
	 *
	 * @param other The other "any" value.
	 * @return This "any" value.
	 */
	FAny& operator=(FAny&& other) noexcept;

private:

	/** An enumeration of possible value operations. */
	enum class EAnyValueOperation
	{
		GetType,
		GetTypeSize,
		Copy,
		Move,
		Destruct,
		Hash
	};

	/** The function signature for FAny value operations. */
	using FAnyValueFunction = usize(*)(EAnyValueOperation operation, const void* source, void* target);

	/**
	 * @brief The default value operation function for values.
	 *
	 * @tparam T The type to operate on.
	 * @param operation The operation.
	 * @param source The source value address.
	 * @param target The target value address.
	 * @return The operation-specific return value.
	 */
	template<typename T>
	static usize DefaultValueFunction(const EAnyValueOperation operation, const void* source, void* target)
	{
		switch (operation)
		{
		case EAnyValueOperation::GetType:
			*reinterpret_cast<const FTypeInfo**>(target) = ::GetType<T>();
			return 0;

		case EAnyValueOperation::GetTypeSize:
			return sizeof(T);

		case EAnyValueOperation::Copy:
			FMemory::ConstructObjectAt<T>(target, *static_cast<const T*>(source));
			return 0;

		case EAnyValueOperation::Move:
			FMemory::ConstructObjectAt<T>(target, MoveTemp(*static_cast<T*>(const_cast<void*>(source))));
			return 0;

		case EAnyValueOperation::Destruct:
			FMemory::DestructObject(static_cast<T*>(target));
			return 0;

		case EAnyValueOperation::Hash:
			return ::GetHashCode(*static_cast<const T*>(source));

		default:
			UM_ASSERT_NOT_REACHED_MSG("Unhandled FAny value operation");
		}
	}

	/**
	 * @brief Copies another FAny value.
	 *
	 * @param other The value to copy.
	 */
	void CopyFrom(const FAny& other);

	/**
	 * @brief Gets a pointer for a new underlying value.
	 *
	 * Will free the old value pointer if necessary, and will then allocate the value pointer if necessary.
	 *
	 * @param valueSize The size of a new value.
	 * @return The pointer to use for the new value.
	 */
	[[nodiscard]] void* GetOrAllocateValuePointer(int32 valueSize);

	/**
	 * @brief Gets a pointer to the underlying value.
	 *
	 * @return A pointer to the underlying value.
	 */
	[[nodiscard]] const void* GetValuePointer() const
	{
		return m_ValueStorage.GetData();
	}

	/**
	 * @brief Gets a pointer to the underlying value.
	 *
	 * @return A pointer to the underlying value.
	 */
	[[nodiscard]] void* GetValuePointer()
	{
		return m_ValueStorage.GetData();
	}

	/**
	 * @brief Gets the size, in bytes, of the underlying type.
	 *
	 * @return The size, in bytes, of the underlying type.
	 */
	[[nodiscard]] int32 GetValueTypeSize() const;

	/**
	 * @brief Moves ownership of another value's resources.
	 *
	 * @param other The value whose resources are being moved.
	 */
	void MoveFrom(FAny&& other);

	/**
	 * @brief Sets the underlying value.
	 *
	 * @tparam T The value's type.
	 * @param value The value.
	 */
	template<typename T>
	void SetValueImpl(T value)
	{
		Reset();

		void* valueLocation = GetOrAllocateValuePointer(sizeof(T));
		FMemory::ConstructObjectAt<T>(valueLocation, value);

		m_ValueTypeFunction = DefaultValueFunction<T>;
	}

	FPotentiallySmallBuffer m_ValueStorage;
	FAnyValueFunction m_ValueTypeFunction = nullptr;
};

template<>
struct TIsZeroConstructible<FAny> : FTrueType
{
};