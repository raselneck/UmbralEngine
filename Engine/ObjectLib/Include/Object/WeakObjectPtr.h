#pragma once

#include "Engine/Cast.h"
#include "Meta/ClassInfo.h"
#include "Misc/Badge.h"
#include "Object/ObjectPtr.h"
#include "Templates/IsBaseOf.h"
#include "Templates/IsConvertible.h"

class UObject;

namespace Private
{
	class FWeakObjectCastHelper;
}

/**
 * @brief Defines a weak pointer to an object.
 */
class FWeakObjectPtr
{
public:

	UM_DEFAULT_COPY(FWeakObjectPtr);
	UM_DEFAULT_MOVE(FWeakObjectPtr);

	/**
	 * @brief Sets default values for this weak object pointer's properties.
	 */
	FWeakObjectPtr() = default;

	/**
	 * @brief Sets default values for this weak object pointer's properties.
	 */
	FWeakObjectPtr(std::nullptr_t) { }

	/**
	 * @brief Creates a weak object pointer directly from an object.
	 *
	 * @param object The object pointer.
	 */
	FWeakObjectPtr(UObject* object);

	/**
	 * @brief Creates a weak object pointer from an object pointer.
	 *
	 * @param object The object pointer.
	 */
	FWeakObjectPtr(const FObjectPtr& object);

	/**
	 * @brief Destroys this weak object pointer.
	 */
	virtual ~FWeakObjectPtr() = default; // TODO Necessary to be virtual?

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return The underlying object;
	 */
	[[nodiscard]] UObject* GetObject() const;

	/**
	 * @brief Checks to see if the underlying object is of the given type.
	 *
	 * @param objectClass The type.
	 * @return True if the underlying object is a \p objectClass, otherwise false.
	 */
	[[nodiscard]] bool IsA(const FClassInfo* objectClass) const;

	/**
	 * @brief Checks to see if the underlying object is of the given type.
	 *
	 * @tparam T The type.
	 * @return True if the underlying object is a \p T, otherwise false.
	 */
	template<typename T>
	[[nodiscard]] bool IsA() const
	{
		return IsA(T::StaticType());
	}

	/**
	 * @brief Checks to see if this weak object pointer can be considered null.
	 *
	 * @return True if this weak object pointer can be considered null.
	 */
	[[nodiscard]] bool IsNull() const;

	/**
	 * @brief Checks to see if this weak object pointer is valid.
	 *
	 * @return True if this weak object pointer is valid.
	 */
	[[nodiscard]] bool IsValid() const;

	/**
	 * @brief Resets this weak object pointer, de-referencing the object if there is one.
	 */
	void Reset();

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return The underlying object;
	 */
	[[nodiscard]] UObject* operator->() const;

	/**
	 * @brief Checks to see if this weak object pointer is valid.
	 *
	 * @return True if this weak object pointer is valid, otherwise false.
	 */
	[[nodiscard]] operator bool() const
	{
		return IsValid();
	}

private:

	class FObjectHeader* m_ObjectHeader = nullptr;
	uint64 m_ObjectHash = INVALID_HASH;
};

/**
 * @brief Defines a templated version of FWeakObjectPtr.
 *
 * @tparam T The object type.
 */
template<typename T>
class TWeakObjectPtr : public FWeakObjectPtr
{
	static_assert(Or<TIsTypeIncomplete<T>, TIsBaseOf<UObject, T>>, "Can only use TWeakObjectPtr with UObject-based types");

	using Super = FWeakObjectPtr;

public:

	UM_DEFAULT_COPY(TWeakObjectPtr);
	UM_DEFAULT_MOVE(TWeakObjectPtr);

	using ObjectType = T;

	/**
	 * @brief Sets default values for this weak object pointer's properties.
	 */
	TWeakObjectPtr() = default;

	/**
	 * @brief Sets default values for this weak object pointer's properties.
	 */
	TWeakObjectPtr(std::nullptr_t) { }

	/**
	 * @brief Creates a weak object pointer directly from an object.
	 *
	 * @param object The object pointer.
	 */
	template<typename OtherObjectType>
	TWeakObjectPtr(OtherObjectType* object)
		requires(IsConvertible<OtherObjectType*, ObjectType*>)
		: Super(object)
	{
	}

	/**
	 * @brief Creates a weak object pointer from an object pointer.
	 *
	 * @param object The object pointer.
	 */
	template<typename OtherObjectType>
	TWeakObjectPtr(const TObjectPtr<OtherObjectType>& object)
		requires(IsConvertible<OtherObjectType*, ObjectType*>)
		: Super(object)
	{
	}

	/**
	 * @brief Copies another weak object pointer.
	 *
	 * @tparam U The type of the other underlying object.
	 * @param other The other weak object pointer.
	 */
	template<typename U>
	TWeakObjectPtr(const TWeakObjectPtr<U>& other)
		requires(IsConvertible<U*, T*>)
		: Super(other.m_Object)
	{
	}

	/**
	 * @brief Assumes ownership of another weak object pointer's resources.
	 *
	 * @tparam U The type of the other underlying object.
	 * @param other The other weak object pointer.
	 */
	template<typename U>
	TWeakObjectPtr(TWeakObjectPtr<U>&& other)
		requires(IsConvertible<U*, T*>)
		: Super(MoveTemp(other.m_Object))
	{
	}

	/**
	 * @brief Creates a typed weak object pointer from an untyped weak object pointer.
	 *
	 * @param other The other, untyped object pointer.
	 */
	TWeakObjectPtr(TBadge<Private::FWeakObjectCastHelper>, const FObjectPtr& other)
		: Super(other)
	{
	}

	/**
	 * @brief Destroys this weak object pointer.
	 */
	virtual ~TWeakObjectPtr() = default; // TODO Necessary to be virtual?

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return The underlying object;
	 */
	[[nodiscard]] T* GetObject() const
	{
		return static_cast<T*>(Super::GetObject());
	}

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return The underlying object;
	 */
	[[nodiscard]] T* operator->() const
	{
		return GetObject();
	}

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return The underlying object;
	 */
	[[nodiscard]] T* operator*() const
	{
		return GetObject();
	}

	/**
	 * @brief Copies another shared object pointer.
	 *
	 * @tparam U The type of the other underlying object.
	 * @param other The other shared object pointer.
	 */
	template<typename U>
	TWeakObjectPtr& operator=(const TWeakObjectPtr<U>& other)
		requires(IsConvertible<U*, T*>)
	{
		if (reinterpret_cast<const void*>(&other) != this)
		{
			Super::operator=(other);
		}
		return *this;
	}

	/**
	 * @brief Copies another shared object pointer.
	 *
	 * @tparam U The type of the other underlying object.
	 * @param other The other shared object pointer.
	 */
	template<typename U>
	TWeakObjectPtr& operator=(TWeakObjectPtr<U>&& other)
		requires(IsConvertible<U*, T*>)
	{
		if (reinterpret_cast<const void*>(&other) != this)
		{
			Super::operator=(MoveTemp(other));
		}
		return *this;
	}
};

DECLARE_PRIMITIVE_TYPE_DEFINITION(FWeakObjectPtr);

namespace Private
{
	template<typename T>
	struct TTypeDefinition<::TWeakObjectPtr<T>> : public ::Private::TTypeDefinition<FWeakObjectPtr>
	{
	};

	/**
	 * @brief Defines a helper for weak object pointers.
	 */
	class FWeakObjectCastHelper
	{
	public:

		/**
		 * @brief Casts an object pointer to another object pointer.
		 *
		 * @tparam ToType The type to cast to.
		 * @param object The object pointer.
		 * @return The casted object pointer. Will be null if the cast was not successful.
		 */
		template<typename ToType>
		static TWeakObjectPtr<ToType> Cast(const FWeakObjectPtr& object)
		{
			if (object.IsA<ToType>() == false)
			{
				return nullptr;
			}

			constexpr TBadge<FWeakObjectCastHelper> badge;
			return TWeakObjectPtr<ToType> { badge, object };
		}
	};
}

/**
 * @brief Attempts to cast the given weak object pointer to another weak object pointer.
 *
 * @tparam ToType The destination type.
 * @param object The source object.
 * @return The casted weak object pointer. Will be null if the cast was not successful.
 */
template<typename ToType>
TWeakObjectPtr<ToType> Cast(FWeakObjectPtr object)
{
	return Private::FWeakObjectCastHelper::Cast<ToType>(object);
}

/**
 * @brief Attempts to cast the given weak object pointer to another weak object pointer.
 *
 * @tparam ToType The type being casted to.
 * @tparam FromType The type being cast from.
 * @param object The source object.
 * @return The casted weak object pointer. Will be null if the cast was not successful.
 */
template<typename ToType, typename FromType>
TWeakObjectPtr<ToType> Cast(TWeakObjectPtr<FromType> value)
{
	return Private::FWeakObjectCastHelper::Cast<ToType>(value);
}

// TODO CastChecked