#pragma once

#include "Memory/SharedPtr.h"
#include "Memory/SharedResourceBlock.h"
#include "Misc/Badge.h"
#include "Templates/IsPointer.h"
#include "Templates/IsReference.h"
#include "Templates/IsZeroConstructible.h"

template<typename T>
class TSharedPtr;

namespace Private
{
	class FWeakPtrHelper;
}

/**
 * @brief Defines a container for a weak reference to a heap-allocated object.
 *
 * @tparam T The object type.
 */
template<typename T>
class TWeakPtr
{
	static_assert(TNot<TIsReference<T>>::Value, "Cannot store references in weak pointers");
	static_assert(TNot<TIsArray<T>>::Value, "Arrays are not currently supported for weak pointers");

	friend class Private::FWeakPtrHelper;

	template<typename U>
	friend class TWeakPtr;

public:

	using ElementType = T;
	using ConstElementType = typename TAddConst<ElementType>::Type;
	using ReferenceType = typename TAddLValueReference<ElementType>::Type;
	using ConstReferenceType = typename TAddConst<ReferenceType>::Type;
	using PointerType = typename TAddPointer<ElementType>::Type;
	using ConstPointerType = typename TAddConst<PointerType>::Type;

	/**
	 * @brief Sets default values for this weak pointer's properties.
	 */
	TWeakPtr() = default;

	/**
	 * @brief Copies another weak pointer.
	 *
	 * @param other The other weak pointer.
	 */
	TWeakPtr(const TWeakPtr& other)
	{
		CopyFrom(other);
	}

	/**
	 * @brief Copies another weak pointer.
	 *
	 * @tparam U The type of the other weak pointer's underlying object.
	 * @param other The other weak pointer.
	 */
	template<typename U>
	TWeakPtr(const TWeakPtr<U>& other)
		requires TIsConvertible<U*, T*>::Value
	{
		CopyFrom(other);
	}

	/**
	 * @brief Assumes ownership of another weak pointer's resources.
	 *
	 * @param other The other weak pointer.
	 */
	TWeakPtr(TWeakPtr&& other) noexcept
	{
		MoveFrom(MoveTemp(other));
	}

	/**
	 * @brief Assumes ownership of another weak pointer's resources.
	 *
	 * @tparam U The type of the other weak pointer's underlying object.
	 * @param other The other weak pointer.
	 */
	template<typename U>
	TWeakPtr(TWeakPtr<U>&& other) noexcept
		requires TIsConvertible<U*, T*>::Value
	{
		MoveFrom(MoveTemp(other));
	}

	/**
	 * @brief Creates a weak pointer from a shared pointer.
	 *
	 * @tparam U The type of the shared pointer's underlying object.
	 * @param sharedPtr The shared pointer.
	 */
	template<typename U>
	TWeakPtr(const TSharedPtr<U> sharedPtr)
		requires TIsConvertible<U*, T*>::Value
	{
		constexpr TBadge<TWeakPtr<T>> badge;

		m_ResourceBlock = sharedPtr.GetResourceBlock(badge);
		if (m_ResourceBlock)
		{
			m_ResourceBlock->AddWeakRef();
		}
	}

	/**
	 * @brief Destroys this weak pointer.
	 */
	~TWeakPtr()
	{
		Reset();
	}

	/**
	 * @brief Assigns a shared resource block to this weak pointer.
	 *
	 * @param resourceBlock The new shared resource block.
	 */
	template<typename U>
	void AssignResourceBlock(TBadge<TSharedPtr<U>>, Private::ISharedResourceBlock* resourceBlock)
	{
		if (m_ResourceBlock)
		{
			m_ResourceBlock->RemoveWeakRef();
		}

		m_ResourceBlock = resourceBlock;

		if (m_ResourceBlock)
		{
			m_ResourceBlock->AddWeakRef();
		}
	}

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return A pointer to the underlying object.
	 */
	[[nodiscard]] ConstPointerType Get() const
	{
		return const_cast<TWeakPtr*>(this)->Get();
	}

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return A pointer to the underlying object.
	 */
	[[nodiscard]] PointerType Get()
	{
		return IsValid() ? m_ResourceBlock->GetTypedObject<ElementType>() : nullptr;
	}

	/**
	 * @brief Gets this weak pointer's resource block.
	 *
	 * @tparam U The shared pointer's type.
	 * @return This weak pointer's resource block.
	 */
	template<typename U>
	[[nodiscard]] Private::ISharedResourceBlock* GetResourceBlock(TBadge<TSharedPtr<U>>) const
	{
		return m_ResourceBlock;
	}

	/**
	 * @brief Checks to see if this weak pointer is null.
	 *
	 * @return True if the underlying object is null, otherwise false.
	 */
	[[nodiscard]] bool IsNull() const
	{
		return IsValid() == false;
	}

	/**
	 * @brief Checks to see if this weak pointer is valid.
	 *
	 * @return True if the underlying object is set, otherwise false.
	 */
	[[nodiscard]] bool IsValid() const
	{
		return m_ResourceBlock != nullptr && m_ResourceBlock->HasObject();
	}

	/**
	 * @brief Pins this weak pointer as a shared pointer.
	 *
	 * @return This weak pointer as a shared pointer.
	 */
	[[nodiscard]] TSharedPtr<ElementType> Pin() const
	{
		constexpr TBadge<TWeakPtr> badge;
		return TSharedPtr<ElementType> { badge, m_ResourceBlock };
	}

	/**
	 * @brief Resets this shared pointer, de-referencing the owned object if there is one.
	 */
	void Reset()
	{
		if (m_ResourceBlock == nullptr)
		{
			return;
		}

		m_ResourceBlock->RemoveWeakRef();
		m_ResourceBlock = nullptr;
	}

	/**
	 * @brief Accesses the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] ConstPointerType operator->() const
	{
		UM_ASSERT(IsValid(), "Attempting to reference invalid weak pointer");
		return m_ResourceBlock->GetTypedObject<ElementType>();
	}

	/**
	 * @brief Accesses the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] PointerType operator->()
	{
		UM_ASSERT(IsValid(), "Attempting to reference invalid weak pointer");
		return m_ResourceBlock->GetTypedObject<ElementType>();
	}

	/**
	 * @brief Copies another weak pointer.
	 *
	 * @param other The other weak pointer.
	 */
	TWeakPtr& operator=(const TWeakPtr& other)
	{
		if (&other != this)
		{
			CopyFrom(other);
		}

		return *this;
	}

	/**
	 * @brief Copies another weak pointer.
	 *
	 * @tparam U The type of the other weak pointer's underlying object.
	 * @param other The other weak pointer.
	 */
	template<typename U>
	TWeakPtr& operator=(const TWeakPtr<U>& other)
		requires TIsConvertible<U*, T*>::Value
	{
		if (reinterpret_cast<void*>(&other) != this)
		{
			CopyFrom(other);
		}

		return *this;
	}

	/**
	 * @brief Moves ownership of another weak pointer's object to this shared pointer.
	 *
	 * @param other The other weak pointer.
	 * @return This weak pointer.
	 */
	TWeakPtr& operator=(TWeakPtr&& other) noexcept
	{
		if (&other != this)
		{
			MoveFrom(MoveTemp(other));
		}

		return *this;
	}

	/**
	 * @brief Moves ownership of another weak pointer's object to this shared pointer.
	 *
	 * @tparam U The type of the other weak pointer's object.
	 * @param other The other weak pointer.
	 * @return This weak pointer.
	 */
	template<typename U>
	TWeakPtr& operator=(TWeakPtr<U>&& other) noexcept
		requires TIsConvertible<U*, T*>::Value
	{
		if (reinterpret_cast<void*>(&other) != this)
		{
			MoveFrom(MoveTemp(other));
		}

		return *this;
	}

	/**
	 * @brief Checks to see if this weak pointer is null.
	 *
	 * @return True if this weak pointer is null, otherwise false.
	 */
	[[nodiscard]] bool operator==(std::nullptr_t) const
	{
		return IsNull();
	}

	/**
	 * @brief Checks to see if this weak pointer is valid.
	 *
	 * @return True if this weak pointer is valid, otherwise false.
	 */
	[[nodiscard]] bool operator!=(std::nullptr_t) const
	{
		return IsValid();
	}

	/**
	 * @brief Checks to see if this weak pointer is valid.
	 *
	 * @return True if the underlying object is set, otherwise false.
	 */
	[[nodiscard]] explicit operator bool() const
	{
		return IsValid();
	}

#define IMPLEMENT_WEAK_PTR_OPERATOR(Operator)                   \
	template<typename U>                                        \
	bool operator Operator (const TWeakPtr<U>& other) const     \
	{                                                           \
		return m_ResourceBlock Operator other.m_ResourceBlock;  \
	}

	// TODO Should the less-than and greater-than operators consider actual underlying value?
	IMPLEMENT_WEAK_PTR_OPERATOR(==)
	IMPLEMENT_WEAK_PTR_OPERATOR(!=)
	IMPLEMENT_WEAK_PTR_OPERATOR(<=)
	IMPLEMENT_WEAK_PTR_OPERATOR(< )
	IMPLEMENT_WEAK_PTR_OPERATOR(>=)
	IMPLEMENT_WEAK_PTR_OPERATOR(> )

#undef IMPLEMENT_WEAK_PTR_OPERATOR

#if WITH_TESTING
	/**
	 * @brief Gets the number of strong references to this weak pointer.
	 *
	 * @return The number of strong references to this weak pointer.
	 */
	[[nodiscard]] int32 GetStrongRefCount() const
	{
		if (m_ResourceBlock)
		{
			return m_ResourceBlock->GetStrongRefCount();
		}
		return 0;
	}

	/**
	 * @brief Gets the number of weak references to this weak pointer.
	 *
	 * @return The number of weak references to this weak pointer.
	 */
	[[nodiscard]] int32 GetWeakRefCount() const
	{
		if (m_ResourceBlock)
		{
			return m_ResourceBlock->GetWeakRefCount();
		}
		return 0;
	}
#endif

private:

	/**
	 * @brief Sets default values for this weak pointer's properties.
	 *
	 * @param resourceBlock The resource block.
	 */
	explicit TWeakPtr(Private::ISharedResourceBlock* resourceBlock)
		: m_ResourceBlock { resourceBlock }
	{
	}

	/**
	 * @brief Copies another weak pointer.
	 *
	 * @tparam U The type of the other weak pointer's underlying object.
	 * @param other The other weak pointer.
	 */
	template<typename U>
	void CopyFrom(const TWeakPtr<U>& other)
	{
		Reset();

		m_ResourceBlock = other.m_ResourceBlock;

		if (m_ResourceBlock)
		{
			m_ResourceBlock->AddWeakRef();
		}
	}

	/**
	 * @brief Assumes ownership of another weak pointer's resources.
	 *
	 * @tparam U The type of the other weak pointer's underlying object.
	 * @param other The other weak pointer.
	 */
	template<typename U>
	void MoveFrom(TWeakPtr<U>&& other)
	{
		Reset();

		m_ResourceBlock = other.m_ResourceBlock;

		other.m_ResourceBlock = nullptr;
	}

	Private::ISharedResourceBlock* m_ResourceBlock = nullptr;
};

template<typename T>
struct TIsZeroConstructible<TWeakPtr<T>> : FTrueType
{
};

namespace Private
{
	/**
	 * @brief Defines a helper for weak pointers.
	 */
	class FWeakPtrHelper
	{
	public:

		/**
		 * @brief Casts a weak pointer from one type to another.
		 *
		 * @tparam ToType The type to cast to.
		 * @tparam FromType The type being cast from.
		 * @param value The weak pointer value to cast.
		 * @return The casted weak pointer. Will be null if the cast failed.
		 */
		template<typename ToType, typename FromType>
		[[nodiscard]] static TWeakPtr<ToType> Cast(TWeakPtr<FromType> value)
		{
			TWeakPtr<ToType> result;
			if (::Cast<ToType>(value.Get()) != nullptr)
			{
				result.m_ResourceBlock = value.m_ResourceBlock;
				result.m_ResourceBlock->AddWeakRef();
			}
			return result;
		}
	};
}

/**
 * @brief Attempts to cast a value from one type to another.
 *
 * @tparam ToType The destination type.
 * @tparam FromType The source type.
 * @param value The value to cast.
 * @return \p value as \p ToType, or nullptr if the cast was not possible.
 */
template<typename ToType, typename FromType>
TWeakPtr<ToType> Cast(TWeakPtr<FromType> value)
{
	return Private::FWeakPtrHelper::Cast<ToType>(MoveTemp(value));
}

/**
 * @brief Gets the hash code of the given weak pointer.
 *
 * @tparam T The type of the weak pointer's underlying object.
 * @param value The weak pointer value.
 * @return The weak pointer's hash code.
 */
template<typename T>
uint64 GetHashCode(TWeakPtr<T> value)
{
	return ::GetHashCode(static_cast<void*>(value.Get()));
}