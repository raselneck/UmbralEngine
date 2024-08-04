#pragma once

#include "Engine/Assert.h"
#include "Engine/Hashing.h"
#include "Memory/Memory.h"
#include "Templates/BulkOperations.h"
#include "Templates/IsArray.h"
#include "Templates/IsConstVolatile.h"
#include "Templates/IsPointer.h"
#include "Templates/IsReference.h"
#include "Templates/IsZeroConstructible.h"

// TODO Figure out how to support defining the delete type in the template, like the STL

namespace Private
{
	/**
	 * @brief Defines the default delete handler for unique pointers.
	 *
	 * @tparam T The type to be deleted.
	 */
	template<typename T>
	class TDefaultDeleter
	{
	public:

		UM_DEFAULT_COPY(TDefaultDeleter);
		UM_DEFAULT_MOVE(TDefaultDeleter);

		TDefaultDeleter() = default;
		~TDefaultDeleter() = default;

		template<typename U>
		TDefaultDeleter(const TDefaultDeleter<U>&) noexcept
			requires(IsConvertible<U*, T*>)
		{
		}

		template<typename U>
		TDefaultDeleter(TDefaultDeleter<U>&&) noexcept
			requires(IsConvertible<U*, T*>)
		{
		}

		void Delete(void* objectMemory) const
		{
			// HACK: This *should* throw a valid compiler error if the type remains complete, but without this check
			//       it will throw an invalid compiler error for now
			if constexpr (IsTypeComplete<T>)
			{
				FMemory::FreeObject(reinterpret_cast<T*>(objectMemory));
			}
		}
	};
}

/**
 * @brief Defines a managed wrapper for a uniquely-owned object.
 *
 * @tparam T The contained object's type.
 */
template<typename T, typename Deleter = Private::TDefaultDeleter<T>>
class TUniquePtr final : private Deleter
{
	static_assert(TNot<TIsReference<T>>::Value, "Cannot store references in unique pointers");
	static_assert(TNot<TIsArray<T>>::Value, "Arrays are not currently supported for unique pointers");

	template<typename U, typename OtherDeleter>
	friend class TUniquePtr;

public:

	using ElementType = T;
	using ConstElementType = typename TAddConst<ElementType>::Type;
	using PointerType = typename TAddPointer<ElementType>::Type;
	using ConstPointerType = typename TAddPointer<ConstElementType>::Type;
	using ReferenceType = typename TAddLValueReference<ElementType>::Type;
	using ConstReferenceType = typename TAddLValueReference<ConstElementType>::Type;
	using DeleterType = Deleter;

	UM_DISABLE_COPY(TUniquePtr);

	/**
	 * @brief Sets default values for this unique pointer's properties.
	 */
	TUniquePtr() = default;

	/**
	 * @brief Sets default values for this unique pointer's properties.
	 */
	TUniquePtr(std::nullptr_t)
	{
	}

	/**
	 * @brief Sets default values for this unique pointer's properties.
	 *
	 * @param object The initial object to own.
	 */
	template<typename U>
	explicit TUniquePtr(U* object)
		requires(IsConvertible<U*, T*>)
		: Deleter()
		, m_Object { object }
	{
	}

	/**
	 * @brief Moves ownership of another unique pointer's object to this unique pointer.
	 *
	 * @param other The other unique pointer.
	 */
	TUniquePtr(TUniquePtr&& other) noexcept
	{
		MoveFrom(MoveTemp(other));
	}

	/**
	 * @brief Moves ownership of another unique pointer's object to this unique pointer.
	 *
	 * @tparam U The other unique pointer's type.
	 * @tparam OtherDeleter The other unique pointer's deleter.
	 * @param other The other unique pointer.
	 */
	template<typename U, typename OtherDeleter>
	TUniquePtr(TUniquePtr<U, OtherDeleter>&& other) noexcept
		requires(IsConvertible<U*, T*>)
		: Deleter(MoveTemp(other.GetDeleter()))
		, m_Object { other.m_Object }
	{
		other.m_Object = nullptr;
	}

	/**
	 * @brief Destroys this unique pointer.
	 */
	~TUniquePtr()
	{
		Reset();
	}

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] constexpr ConstPointerType Get() const
	{
		return m_Object;
	}

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] constexpr PointerType Get()
	{
		return m_Object;
	}

	/**
	 * @brief Checks to see if this unique pointer is null.
	 *
	 * @return True if the underlying object is null, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsNull() const
	{
		return m_Object == nullptr;
	}

	/**
	 * @brief Checks to see if this unique pointer is valid.
	 *
	 * @return True if the underlying object is set, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsValid() const
	{
		return m_Object != nullptr;
	}

	/**
	 * @brief Makes a unique pointer.
	 *
	 * @tparam ArgTypes The types of the arguments to supply to the object's constructor.
	 * @param args The arguments to supply to the object's constructor.
	 * @return The unique pointer.
	 */
	template<typename... ArgTypes>
	[[nodiscard]] static TUniquePtr Make(ArgTypes... args)
	{
		TUniquePtr result;
		result.m_Object = FMemory::AllocateObject<ElementType>(Forward<ArgTypes>(args)...);
		return result;
	}

	// TODO MakeWithAllocator

	/**
	 * @brief Resets this unique pointer, freeing the owned object if there is one.
	 */
	void Reset()
	{
		if (m_Object == nullptr)
		{
			return;
		}

		GetDeleter().Delete(m_Object);

		m_Object = nullptr;
	}

	/**
	 * @brief Accesses the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] ConstPointerType operator->() const
	{
		UM_ASSERT(IsValid(), "Attempting to reference invalid unique pointer");
		return m_Object;
	}

	/**
	 * @brief Accesses the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] PointerType operator->()
	{
		UM_ASSERT(IsValid(), "Attempting to reference invalid unique pointer");
		return m_Object;
	}

	/**
	 * @brief Accesses the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] ConstReferenceType operator*() const
	{
		UM_ASSERT(IsValid(), "Attempting to dereference invalid unique pointer");
		return *m_Object;
	}

	/**
	 * @brief Accesses the underlying object.
	 *
	 * @return The underlying object.
	 */
	[[nodiscard]] ReferenceType operator*()
	{
		UM_ASSERT(IsValid(), "Attempting to dereference invalid unique pointer");
		return *m_Object;
	}

	/**
	 * @brief Moves ownership of another unique pointer's object to this unique pointer.
	 *
	 * @param other The other unique pointer.
	 * @return This unique pointer.
	 */
	TUniquePtr& operator=(TUniquePtr&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}

		MoveFrom(MoveTemp(other));
		return *this;
	}

	/**
	 * @brief Moves ownership of another unique pointer's object to this unique pointer.
	 *
	 * @tparam U The other unique pointer's type.
	 * @tparam OtherDeleter The other unique pointer's deleter.
	 * @param other The other unique pointer.
	 */
	template<typename U, typename OtherDeleter>
	TUniquePtr& operator=(TUniquePtr<U, OtherDeleter>&& other) noexcept
	{
		if (static_cast<void*>(&other) == static_cast<void*>(this))
		{
			return *this;
		}

		Reset();
		m_Object = other.m_Object;
		GetDeleter() = MoveTemp(other.GetDeleter());
		other.m_Object = nullptr;

		return *this;
	}

	/**
	 * @brief Checks to see if this unique pointer is valid.
	 *
	 * @return True if the underlying object is set, otherwise false.
	 */
	[[nodiscard]] constexpr explicit operator bool() const
	{
		return IsValid();
	}

private:

	/**
	 * @brief Gets this unique pointer's deleter.
	 *
	 * @return This unique pointer's deleter.
	 */
	[[nodiscard]] const Deleter& GetDeleter() const
	{
		return static_cast<const Deleter&>(*this);
	}

	/**
	 * @brief Gets this unique pointer's deleter.
	 *
	 * @return This unique pointer's deleter.
	 */
	[[nodiscard]] Deleter& GetDeleter()
	{
		return static_cast<Deleter&>(*this);
	}

	/**
	 * @brief Moves ownership of another unique pointer's object to this unique pointer.
	 *
	 * @param other The other unique pointer.
	 */
	constexpr void MoveFrom(TUniquePtr&& other)
	{
		Reset();

		m_Object = other.m_Object;

		other.m_Object = nullptr;
	}

	ElementType* m_Object = nullptr;
};

template<typename T>
struct TIsZeroConstructible<TUniquePtr<T>> : FTrueType
{
};

/**
 * @brief Gets the hash code of the given unique pointer.
 *
 * @tparam T The type of the unique pointer's underlying object.
 * @param value The unique pointer value.
 * @return The unique pointer's hash code.
 */
template<typename T>
uint64 GetHashCode(TUniquePtr<T>& value)
{
	return ::GetHashCode(static_cast<void*>(value.Get()));
}

/**
 * @brief Creates a new unique pointer for an object of type \tparam T.
 *
 * @tparam T The object type.
 * @tparam TArgs The types of the arguments to forward to the object's constructor.
 * @param args The arguments to forward for the object's constructor.
 * @return The unique pointer.
 */
template<typename ObjectType, typename... ArgTypes>
TUniquePtr<ObjectType> MakeUnique(ArgTypes... args)
{
	return TUniquePtr<ObjectType>::Make(Forward<ArgTypes>(args)...);
}