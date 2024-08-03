#pragma once

#include "Engine/Assert.h"
#include "Engine/Cast.h"
#include "Engine/Hashing.h"
#include "Memory/SharedResourceBlock.h"
#include "Misc/Badge.h"
#include "Templates/TypeTraits.h"

template<typename T>
class TEnableSharedFromThis;

template<typename T>
class TWeakPtr;

namespace Private
{
	class FSharedPtrHelper;
}

/**
 * @brief Defines a container for a heap-allocated object that has shared reference counting.
 *
 * A shared pointer is similar to a shared reference, with the exception of possibly being null.
 *
 * @tparam T The object type.
 */
template<typename T>
class TSharedPtr final
{
	static_assert(TNot<TIsReference<T>>::Value, "Cannot store references in shared pointers");
	static_assert(TNot<TIsArray<T>>::Value, "Arrays are not currently supported for shared pointers");

	friend class Private::FSharedPtrHelper;

	template<typename U>
	friend class TSharedPtr;

	template<typename U>
	friend class TWeakPtr;

public:

	using ElementType = T;
	using ConstElementType = AddConst<ElementType>;
	using ReferenceType = AddLValueReference<ElementType>;
	using ConstReferenceType = AddConst<ReferenceType>;
	using PointerType = AddPointer<ElementType>;
	using ConstPointerType = AddConst<PointerType>;

	/**
	 * @brief Sets default values for this shared pointer's properties.
	 */
	constexpr TSharedPtr() = default;

	/**
	 * @brief Sets default values for this shared pointer's properties.
	 */
	constexpr TSharedPtr(std::nullptr_t)
	{
	}

	/**
	 * @brief Sets default values for this shared pointer's properties.
	 *
	 * @param value The default value.
	 */
	explicit TSharedPtr(PointerType value)
		: m_ResourceBlock { Private::AllocResourceBlock<ElementType>(value) }
	{
		if (m_ResourceBlock)
		{
			EnableWeakThis(value, value);
		}
	}

	/**
	 * @brief Copies another shared pointer.
	 *
	 * @param other The other shared pointer.
	 */
	TSharedPtr(const TSharedPtr& other)
	{
		CopyFrom(other);
	}

	/**
	 * @brief Copies another shared pointer.
	 *
	 * @tparam U The type of the other shared pointer's underlying object.
	 * @param other The other shared pointer.
	 */
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& other)
		requires TIsConvertible<U*, T*>::Value
	{
		CopyFrom(other);
	}

	/**
	 * @brief Assumes ownership of another shared pointer's resources.
	 *
	 * @param other The other shared pointer.
	 */
	TSharedPtr(TSharedPtr&& other) noexcept
	{
		MoveFrom(MoveTemp(other));
	}

	/**
	 * @brief Assumes ownership of another shared pointer's resources.
	 *
	 * @tparam U The type of the other shared pointer's underlying object.
	 * @param other The other shared pointer.
	 */
	template<typename U>
	TSharedPtr(TSharedPtr<U>&& other) noexcept
		requires TIsConvertible<U*, T*>::Value
	{
		MoveFrom(MoveTemp(other));
	}

	/**
	 * @brief Allows a weak pointer to construct a shared pointer.
	 *
	 * @param resourceBlock The resource block.
	 */
	template<typename U>
	TSharedPtr(TBadge<TWeakPtr<U>>, Private::ISharedResourceBlock* resourceBlock)
		: TSharedPtr(resourceBlock)
	{
	}

	/**
	 * @brief Destroys this shared pointer.
	 */
	~TSharedPtr()
	{
		Reset();
	}

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return A pointer to the underlying object.
	 */
	[[nodiscard]] ConstPointerType Get() const
	{
		return const_cast<TSharedPtr*>(this)->Get();
	}

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return A pointer to the underlying object.
	 */
	[[nodiscard]] PointerType Get()
	{
		return m_ResourceBlock == nullptr ? nullptr : m_ResourceBlock->GetTypedObject<ElementType>();
	}

	/**
	 * @brief Gets this shared pointer's resource block.
	 *
	 * @tparam U The weak pointer's type.
	 * @return This shared pointer's resource block.
	 */
	template<typename U>
	[[nodiscard]] Private::ISharedResourceBlock* GetResourceBlock(TBadge<TWeakPtr<U>>) const
	{
		return m_ResourceBlock;
	}

	/**
	 * @brief Checks to see if this shared pointer is null.
	 *
	 * @return True if the underlying object is null, otherwise false.
	 */
	[[nodiscard]] bool IsNull() const
	{
		return IsValid() == false;
	}

	/**
	 * @brief Checks to see if this shared pointer is valid.
	 *
	 * @return True if the underlying object is set, otherwise false.
	 */
	[[nodiscard]] bool IsValid() const
	{
		return m_ResourceBlock != nullptr && m_ResourceBlock->HasObject();
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

		m_ResourceBlock->RemoveStrongRef();
		m_ResourceBlock = nullptr;
	}

	/**
	 * @brief Accesses the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] ConstPointerType operator->() const
	{
		UM_ASSERT(IsValid(), "Attempting to reference invalid shared pointer");
		return m_ResourceBlock->GetTypedObject<ElementType>();
	}

	/**
	 * @brief Accesses the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] PointerType operator->()
	{
		UM_ASSERT(IsValid(), "Attempting to reference invalid shared pointer");
		return m_ResourceBlock->GetTypedObject<ElementType>();
	}

	/**
	 * @brief Accesses the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] ConstReferenceType operator*() const
	{
		UM_ASSERT(IsValid(), "Attempting to dereference invalid shared pointer");
		return *Get();
	}

	/**
	 * @brief Accesses the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] ReferenceType operator*()
	{
		UM_ASSERT(IsValid(), "Attempting to dereference invalid shared pointer");
		return *Get();
	}

	/**
	 * @brief Copies another shared pointer.
	 *
	 * @param other The other shared pointer.
	 */
	TSharedPtr& operator=(const TSharedPtr& other)
	{
		if (&other != this)
		{
			CopyFrom(other);
		}

		return *this;
	}

	/**
	 * @brief Copies another shared pointer.
	 *
	 * @tparam U The type of the other shared pointer's underlying object.
	 * @param other The other shared pointer.
	 */
	template<typename U>
	TSharedPtr& operator=(const TSharedPtr<U>& other)
		requires TIsConvertible<U*, T*>::Value
	{
		if (reinterpret_cast<void*>(&other) != this)
		{
			CopyFrom(other);
		}

		return *this;
	}

	/**
	 * @brief Moves ownership of another shared pointer's object to this shared pointer.
	 *
	 * @param other The other shared pointer.
	 * @return This shared pointer.
	 */
	TSharedPtr& operator=(TSharedPtr&& other) noexcept
	{
		if (&other != this)
		{
			MoveFrom(MoveTemp(other));
		}

		return *this;
	}

	/**
	 * @brief Moves ownership of another shared pointer's object to this shared pointer.
	 *
	 * @tparam U The type of the other shared pointer's object.
	 * @param other The other shared pointer.
	 * @return This shared pointer.
	 */
	template<typename U>
	TSharedPtr& operator=(TSharedPtr<U>&& other) noexcept
		requires TIsConvertible<U*, T*>::Value
	{
		if (reinterpret_cast<void*>(&other) != this)
		{
			MoveFrom(MoveTemp(other));
		}

		return *this;
	}

	/**
	 * @brief Checks to see if this shared pointer is null.
	 *
	 * @return True if this shared pointer is null, otherwise false.
	 */
	[[nodiscard]] bool operator==(std::nullptr_t) const
	{
		return IsNull();
	}

	/**
	 * @brief Checks to see if this shared pointer is valid.
	 *
	 * @return True if this shared pointer is valid, otherwise false.
	 */
	[[nodiscard]] bool operator!=(std::nullptr_t) const
	{
		return IsValid();
	}

	/**
	 * @brief Checks to see if this shared pointer is valid.
	 *
	 * @return True if the underlying object is set, otherwise false.
	 */
	[[nodiscard]] explicit operator bool() const
	{
		return IsValid();
	}

#define IMPLEMENT_SHARED_PTR_OPERATOR(Operator)                 \
	template<typename U>                                        \
	bool operator Operator (const TSharedPtr<U>& other) const   \
	{                                                           \
		return m_ResourceBlock Operator other.m_ResourceBlock;  \
	}

	// TODO Should the less-than and greater-than operators consider actual underlying value?
	IMPLEMENT_SHARED_PTR_OPERATOR(==)
	IMPLEMENT_SHARED_PTR_OPERATOR(!=)
	IMPLEMENT_SHARED_PTR_OPERATOR(<=)
	IMPLEMENT_SHARED_PTR_OPERATOR(< )
	IMPLEMENT_SHARED_PTR_OPERATOR(>=)
	IMPLEMENT_SHARED_PTR_OPERATOR(> )

#undef IMPLEMENT_SHARED_PTR_OPERATOR

#if WITH_TESTING
	/**
	 * @brief Gets the number of strong references to this shared pointer.
	 *
	 * @return The number of strong references to this shared pointer.
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
	 * @brief Gets the number of weak references to this shared pointer.
	 *
	 * @return The number of weak references to this shared pointer.
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
	 * @brief Allows the shared pointer helper to construct a shared pointer.
	 *
	 * @param resourceBlock The resource block.
	 */
	explicit TSharedPtr(Private::ISharedResourceBlock* resourceBlock)
		: m_ResourceBlock { resourceBlock }
	{
		if (m_ResourceBlock == nullptr)
		{
			return;
		}

#if WITH_TESTING
		UM_ENSURE(m_ResourceBlock->GetStrongRefCount() > 0);
		UM_ENSURE(m_ResourceBlock->GetWeakRefCount() > 0);
#endif

		EnableWeakThis(Get(), Get());
	}

	/**
	 * @brief Copies another shared pointer.
	 *
	 * @tparam U The type of the other shared pointer's underlying object.
	 * @param other The other shared pointer.
	 */
	template<typename U>
	void CopyFrom(const TSharedPtr<U>& other)
	{
		Reset();

		m_ResourceBlock = other.m_ResourceBlock;

		if (m_ResourceBlock)
		{
			m_ResourceBlock->AddStrongRef();
		}
	}

	/**
	 * @brief Populates the self weak reference on a type that enables "shared from this."
	 *
	 * @tparam SharedType
	 * @tparam ObjectType
	 * @param sharedObject The "shared from this" object.
	 * @param object The object.
	 */
	template<typename SharedType, typename ObjectType>
	void EnableWeakThis(TEnableSharedFromThis<SharedType>* sharedObject, [[maybe_unused]] ObjectType* object)
		requires IsConvertible<ObjectType*, const TEnableSharedFromThis<SharedType>*>
	{
		constexpr TBadge<TSharedPtr<T>> badge;

		if (Private::ISharedResourceBlock* resourceBlock = sharedObject->m_WeakThis.GetResourceBlock(badge))
		{
			UM_ASSERT(resourceBlock == m_ResourceBlock, "Shared pointer attempting to own separately owned object");
		}
		else
		{
			sharedObject->m_WeakThis.AssignResourceBlock(badge, m_ResourceBlock);
		}
	}

	// SFINAE fallback for non-"enable shared from this" values
	void EnableWeakThis(...) { }

	/**
	 * @brief Assumes ownership of another shared pointer's resources.
	 *
	 * @tparam U The type of the other shared pointer's underlying object.
	 * @param other The other shared pointer.
	 */
	template<typename U>
	void MoveFrom(TSharedPtr<U>&& other)
	{
		Reset();

		m_ResourceBlock = other.m_ResourceBlock;

		other.m_ResourceBlock = nullptr;
	}

	Private::ISharedResourceBlock* m_ResourceBlock = nullptr;
};

namespace Private
{
	/**
	 * @brief Defines a helper for shared pointers.
	 */
	class FSharedPtrHelper
	{
	public:

		// TODO Move the casting directly into the shared pointer?

		/**
		 * @brief Casts a shared pointer from one type to another.
		 *
		 * @tparam ToType The type to cast to.
		 * @tparam FromType The type being cast from.
		 * @param value The shared pointer value to cast.
		 * @return The casted shared pointer. Will be null if the cast failed.
		 */
		template<typename ToType, typename FromType>
		[[nodiscard]] static TSharedPtr<ToType> Cast(TSharedPtr<FromType> value)
		{
			TSharedPtr<ToType> result;
			if (::Cast<ToType>(value.Get()) != nullptr)
			{
				result.m_ResourceBlock = value.m_ResourceBlock;
				result.m_ResourceBlock->AddStrongRef();
			}
			return result;
		}

		/**
		 * @brief Constructs a valid shared pointer by allocating its underlying value.
		 *
		 * @tparam ElementType The underlying value type.
		 * @tparam ConstructTypes  The types of the arguments to supply to the value's constructor.
		 * @param args The arguments to supply to the underlying value's constructor.
		 * @return The shared pointer.
		 */
		template<typename ElementType, typename... ConstructTypes>
		[[nodiscard]] static TSharedPtr<ElementType> MakeShared(ConstructTypes... args)
		{
			ISharedResourceBlock* resourceBlock = AllocCombinedResourceBlock<ElementType>(Forward<ConstructTypes>(args)...);
			return TSharedPtr<ElementType> { resourceBlock };
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
TSharedPtr<ToType> Cast(TSharedPtr<FromType> value)
{
	return Private::FSharedPtrHelper::Cast<ToType>(MoveTemp(value));
}

/**
 * @brief Gets the hash code of the given shared pointer.
 *
 * @tparam T The type of the shared pointer's underlying object.
 * @param value The shared pointer value.
 * @return The shared pointer's hash code.
 */
template<typename T>
uint64 GetHashCode(TSharedPtr<T> value)
{
	return ::GetHashCode(static_cast<void*>(value.Get()));
}

/**
 * @brief Constructs a shared pointer.
 *
 * @tparam ElementType The underlying value's type.
 * @tparam ConstructTypes The types of the arguments to supply to the value's constructor.
 * @param args The arguments to supply to the underlying value's constructor.
 * @return The shared pointer.
 */
template<typename ElementType, typename... ConstructTypes>
TSharedPtr<ElementType> MakeShared(ConstructTypes... args)
{
	return Private::FSharedPtrHelper::MakeShared<ElementType>(Forward<ConstructTypes>(args)...);
}