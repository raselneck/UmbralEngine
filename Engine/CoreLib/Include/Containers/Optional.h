#pragma once

#include "Engine/IntTypes.h"
#include "Engine/Assert.h"
#include "Engine/Hashing.h"
#include "Memory/AlignedStorage.h"
#include "Memory/Memory.h"
#include "Templates/BulkOperations.h"
#include "Templates/TypeTraits.h"

namespace Private
{
	struct FNullOptional
	{
	};
}

/**
 * @brief Similar to nullptr, used to construct a "null" optional value.
 */
[[maybe_unused]] inline constexpr Private::FNullOptional nullopt;

/**
 * @brief Defines a way to optionally store a value.
 *
 * @tparam T The optional element type.
 */
template<typename T>
class TOptional final
{
public:

	using ElementType = RemoveCV<T>;
	using StorageType = TAlignedStorage<sizeof(ElementType), alignof(ElementType)>;

	/**
	 * @brief Sets default values for this optional's properties.
	 */
	constexpr TOptional() = default;

	/**
	 * @brief Sets default values for this optional's properties.
	 */
	constexpr TOptional(decltype(nullopt)) { }

	/**
	 * @brief Copies another optional.
	 *
	 * @param other The optional to copy.
	 */
	TOptional(const TOptional& other)
	{
		CopyFrom(other);
	}

	/**
	 * @brief Assumes ownership of another optional's value.
	 *
	 * @param other The optional to own.
	 */
	TOptional(TOptional&& other)
	{
		MoveFrom(MoveTemp(other));
	}

	/**
	 * @brief Sets this optional's underlying value.
	 *
	 * @param value The value to copy.
	 */
	TOptional(const ElementType& value)
	{
		SetValue(value);
	}

	/**
	 * @brief Sets this optional's underlying value.
	 *
	 * @param value The value to own.
	 */
	TOptional(ElementType&& value)
	{
		SetValue(MoveTemp(value));
	}

	/**
	 * @brief Destroys this optional.
	 */
	~TOptional()
	{
		Reset();
	}

	/**
	 * @brief In-place constructs the underlying value.
	 *
	 * @tparam ArgTypes The types of the arguments to pass along to the constructor.
	 * @param args The arguments to pass along to the constructor.
	 * @return The underlying value that was constructed.
	 */
	template<typename... ArgTypes>
	[[nodiscard]] ElementType& EmplaceValue(ArgTypes&&... args)
	{
		Reset();

		new (m_Storage.GetData()) ElementType(Forward<ArgTypes>(args)...);
		m_HasValue = true;

		return GetValue();
	}

	/**
	 * @brief Gets a pointer to this optional's value.
	 *
	 * @return A pointer to this optional's value. Will be nullptr if this optional does not have a value.
	 */
	[[nodiscard]] const ElementType* GetValueAsPointer() const
	{
		return const_cast<TOptional*>(this)->GetValueAsPointer();
	}

	/**
	 * @brief Gets a pointer to this optional's value.
	 *
	 * @return A pointer to this optional's value. Will be nullptr if this optional does not have a value.
	 */
	[[nodiscard]] ElementType* GetValueAsPointer()
	{
		return m_HasValue ? m_Storage.template GetTypedData<ElementType>() : nullptr;
	}

	/**
	 * @brief Gets this optional's value. Will assert if this optional does not have a value.
	 *
	 * @return This optional's value.
	 */
	[[nodiscard]] const ElementType& GetValue() const
	{
		return const_cast<TOptional*>(this)->GetValue();
	}

	/**
	 * @brief Gets this optional's value. Will assert if this optional does not have a value.
	 *
	 * @return This optional's value.
	 */
	[[nodiscard]] ElementType& GetValue()
	{
		UM_ASSERT(HasValue(), "Attempting to retrieve value from unset optional");
		return *m_Storage.template GetTypedData<ElementType>();
	}

	/**
	 * @brief Gets this optional's value, or the given default value if this optional does not have a value.
	 *
	 * @param defaultValue The default value to return if this optional does not have a value.
	 * @return This optional's value, or the specified default value if this optional does not have a value.
	 */
	[[nodiscard]] ElementType GetValueOr(ElementType defaultValue) const
	{
		return m_HasValue ? GetValue() : MoveTemp(defaultValue);
	}

	/**
	 * @brief Gets a value indicating whether or not this optional has a value.
	 *
	 * @return True if this optional has a value, otherwise false.
	 */
	[[nodiscard]] bool HasValue() const
	{
		return m_HasValue;
	}

	/**
	 * @brief Gets a value indicating whether or not this optional is empty.
	 *
	 * @return True if this optional is empty, otherwise false.
	 */
	[[nodiscard]] bool IsEmpty() const
	{
		return m_HasValue == false;
	}

	/**
	 * @brief Releases ownership of this optional's value, clearing it in the process.
	 *
	 * @remarks Will throw if this optional does not have a value.
	 *
	 * @return This optional's value.
	 */
	[[nodiscard]] T ReleaseValue()
	{
		UM_ASSERT(HasValue(), "Cannot release an optional value if it does not have one");

		T value = MoveTemp(GetValue());
		Reset();

		return MoveTemp(value);
	}

	/**
	 * @brief Resets this optional, clearing any value it might store.
	 */
	void Reset()
	{
		if (m_HasValue == false)
		{
			return;
		}

		DestructElements(GetValueAsPointer(), 1);

		//FMemory::ZeroOut(m_Storage.GetData(), m_Storage.Size);
		m_HasValue = false;
	}

	/**
	 * @brief Sets the underlying value.
	 *
	 * @param value The value to copy.
	 */
	void SetValue(const ElementType& value)
		requires TIsCopyConstructible<ElementType>::Value
	{
		Reset();

		new (m_Storage.GetData()) ElementType(value);
		m_HasValue = true;
	}

	/**
	 * @brief Sets the underlying value.
	 *
	 * @param value The value to own.
	 */
	void SetValue(ElementType&& value)
		requires TIsMoveConstructible<ElementType>::Value
	{
		Reset();

		new (m_Storage.GetData()) ElementType(MoveTemp(value));
		m_HasValue = true;
	}

	/**
	 * @brief Copies another optional.
	 *
	 * @param other The other optional.
	 * @return This optional.
	 */
	TOptional& operator=(const TOptional& other)
	{
		if (&other != this)
		{
			CopyFrom(other);
		}

		return *this;
	}

	/**
	 * @brief Assumes ownership of another optional's value.
	 *
	 * @param other The other optional.
	 * @return This optional.
	 */
	TOptional& operator=(TOptional&& other) noexcept
	{
		if (&other != this)
		{
			MoveFrom(MoveTemp(other));
		}

		return *this;
	}

	/**
	 * @brief Sets the underlying value.
	 *
	 * @param value The new value.
	 * @return This optional.
	 */
	TOptional& operator=(const ElementType& value)
	{
		SetValue(value);
		return *this;
	}

	/**
	 * @brief Sets the underlying value.
	 *
	 * @param value The new value.
	 * @return This optional.
	 */
	TOptional& operator=(ElementType&& value)
	{
		SetValue(MoveTemp(value));
		return *this;
	}

	/**
	 * @brief Clears the underlying value.
	 *
	 * @return This optional.
	 */
	TOptional& operator=(decltype(nullopt))
	{
		Reset();
		return *this;
	}

private:

	/**
	 * @brief Copies the value from the given optional value if it contains one.
	 *
	 * @param other The optional value to copy.
	 */
	void CopyFrom(const TOptional& other)
	{
		Reset();

		if (other.HasValue())
		{
			SetValue(other.GetValue());
		}
	}

	/**
	 * @brief Moves the value from the given optional value if it contains one.
	 *
	 * @param other The optional value to move.
	 */
	void MoveFrom(TOptional&& other)
	{
		Reset();

		if (other.HasValue())
		{
			T otherValue = other.ReleaseValue();
			new (m_Storage.GetData()) ElementType(MoveTemp(otherValue));
			m_HasValue = true;
		}
	}

	StorageType m_Storage;
	bool m_HasValue = false;
};

template<typename T>
struct TIsZeroConstructible<TOptional<T>> : TIsZeroConstructible<typename TOptional<T>::StorageType>
{
};

/**
 * @brief Gets the hash code for the given optional value.
 *
 * @tparam T The underlying value type.
 * @param value The optional value.
 * @return The hash code.
 */
template<typename T>
inline uint64 GetHashCode(const TOptional<T>& value)
{
	return value.HasValue() ? GetHashCode(value.GetValue()) : 0;
}

/**
 * @brief Makes an optional value.
 *
 * @tparam T The optional type.
 * @tparam ArgTypes The types of the arguments to supply to the type's constructor.
 * @param args The arguments to supply to the type's constructor.
 * @return The optional value.
 */
template<typename T, typename... ArgTypes>
inline TOptional<T> MakeOptional(ArgTypes&&... args)
{
	TOptional<T> value;
	(void)value.template EmplaceValue<ArgTypes...>(Forward<ArgTypes>(args)...);
	return value;
}