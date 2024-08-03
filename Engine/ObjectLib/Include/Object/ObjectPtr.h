#pragma once

#include "Engine/Cast.h"
#include "Meta/ClassInfo.h"
#include "Misc/Badge.h"
#include "Templates/IsBaseOf.h"
#include "Templates/IsTypeComplete.h"

class FObjectHeap;
class UObject;

namespace Private
{
	class FObjectPtrCastHelper;
}

/**
 * @brief Defines a pointer to an object.
 */
class FObjectPtr
{
public:

	UM_DEFAULT_COPY(FObjectPtr);
	UM_DEFAULT_MOVE(FObjectPtr);

	/**
	 * @brief Sets default values for this object pointer's properties.
	 */
	FObjectPtr() = default;

	/**
	 * @brief Sets default values for this object pointer's properties.
	 */
	FObjectPtr(std::nullptr_t) { }

	/**
	 * @brief Creates an object pointer directly from an object.
	 *
	 * @param object The object pointer.
	 */
	FObjectPtr(UObject* object);

	/**
	 * @brief Destroys this object pointer.
	 */
	virtual ~FObjectPtr() = default; // TODO Necessary to be virtual?

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
	 * @brief Checks to see if this object pointer can be considered null.
	 *
	 * @return True if this object pointer can be considered null.
	 */
	[[nodiscard]] bool IsNull() const;

	/**
	 * @brief Checks to see if this object pointer is valid.
	 *
	 * @return True if this object pointer is valid.
	 */
	[[nodiscard]] bool IsValid() const;

	/**
	 * @brief Resets this object pointer, de-referencing the object if there is one.
	 */
	void Reset();

	/**
	 * @brief Gets the underlying object.
	 *
	 * @return The underlying object;
	 */
	[[nodiscard]] UObject* operator->() const;

	/**
	 * @brief Checks to see if this object pointer is the same as another.
	 *
	 * @param other The other object pointer.
	 * @return True if the two object pointers point to the same object, otherwise false.
	 */
	[[nodiscard]] bool operator==(const FObjectPtr& other) const
	{
		return GetObject() == other.GetObject();
	}

	/**
	 * @brief Checks to see if this object pointer is not the same as another.
	 *
	 * @param other The other object pointer.
	 * @return True if the two object pointers point to different objects, otherwise false.
	 */
	[[nodiscard]] bool operator!=(const FObjectPtr& other) const
	{
		return GetObject() != other.GetObject();
	}

private:

	class FObjectHeader* m_ObjectHeader = nullptr;
	uint64 m_ObjectHash = INVALID_HASH;
};

/**
 * @brief Defines a templated version of FSoftObjectPtr.
 *
 * @tparam T The object type.
 */
template<typename T>
class TObjectPtr : public FObjectPtr
{
	static_assert(Or<TIsTypeIncomplete<T>, TIsBaseOf<UObject, T>>, "Can only use TObjectPtr with UObject-based types");

	using Super = FObjectPtr;

public:

	UM_DEFAULT_COPY(TObjectPtr);
	UM_DEFAULT_MOVE(TObjectPtr);

	using ObjectType = T;

	/**
	 * @brief Sets default values for this object pointer's properties.
	 */
	TObjectPtr() = default;

	/**
	 * @brief Sets default values for this object pointer's properties.
	 */
	TObjectPtr(std::nullptr_t) { }

	/**
	 * @brief Creates an object pointer directly from an object.
	 *
	 * @param object The object.
	 */
	template<typename OtherObjectType>
	TObjectPtr(OtherObjectType* object)
		requires(IsConvertible<OtherObjectType*, ObjectType*>)
		: Super(const_cast<RemoveConst<OtherObjectType>*>(object))
	{
		if constexpr (IsConst<OtherObjectType>)
		{
			static_assert(IsConst<ObjectType>);
		}
	}

	/**
	 * @brief Copies another object pointer.
	 *
	 * @tparam U The type of the other underlying object.
	 * @param other The other object pointer.
	 */
	template<typename U>
	TObjectPtr(const TObjectPtr<U>& other)
		requires(IsConvertible<U*, T*>)
		: Super(other)
	{
	}

	/**
	 * @brief Assumes ownership of another object pointer's resources.
	 *
	 * @tparam U The type of the other underlying object.
	 * @param other The other object pointer.
	 */
	template<typename U>
	TObjectPtr(TObjectPtr<U>&& other)
		requires(IsConvertible<U*, T*>)
		: Super(MoveTemp(other))
	{
	}

	/**
	 * @brief Creates a typed object pointer from an untyped object pointer.
	 *
	 * @param other The other, untyped object pointer.
	 */
	TObjectPtr(TBadge<Private::FObjectPtrCastHelper>, const FObjectPtr& other)
		: Super(other)
	{
	}

	/**
	 * @brief Destroys this object pointer.
	 */
	virtual ~TObjectPtr() = default; // TODO Necessary to be virtual?

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
	TObjectPtr& operator=(const TObjectPtr<U>& other)
		requires(IsConvertible<U*, T*>)
	{
		if (reinterpret_cast<const void*>(&other) != this)
		{
			Super::operator=(other);
		}
		return *this;
	}

	/**
	 * @brief Assumes ownership of another object pointer's resources.
	 *
	 * @tparam U The type of the other underlying object.
	 * @param other The other shared object pointer.
	 */
	template<typename U>
	TObjectPtr& operator=(TObjectPtr<U>&& other)
		requires(IsConvertible<U*, T*>)
	{
		if (reinterpret_cast<const void*>(&other) != this)
		{
			Super::operator=(MoveTemp(other));
		}
		return *this;
	}

	/**
	 * @brief Checks to see if this object pointer is the same as another.
	 *
	 * @param other The other object pointer.
	 * @return True if the two object pointers point to the same object, otherwise false.
	 */
	template<typename U>
	[[nodiscard]] bool operator==(const TObjectPtr<U>& other) const
	{
		static_assert(IsConvertible<U*, T*> || IsConvertible<T*, U*>);
		return Super::operator==(other);
	}

	/**
	 * @brief Checks to see if this object pointer is not the same as another.
	 *
	 * @param other The other object pointer.
	 * @return True if the two object pointers point to different objects, otherwise false.
	 */
	template<typename U>
	[[nodiscard]] bool operator!=(const TObjectPtr<U>& other) const
	{
		static_assert(IsConvertible<U*, T*> || IsConvertible<T*, U*>);
		return Super::operator!=(other);
	}
};

DECLARE_PRIMITIVE_TYPE_DEFINITION(FObjectPtr);

namespace Private
{
	template<typename T>
	struct TTypeDefinition<::TObjectPtr<T>> : public ::Private::TTypeDefinition<FObjectPtr>
	{
	};

	/**
	 * @brief Defines a helper for object pointers.
	 */
	class FObjectPtrCastHelper
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
		static TObjectPtr<ToType> Cast(const FObjectPtr& object)
		{
			if (object.IsA<ToType>() == false)
			{
				return nullptr;
			}

			constexpr TBadge<FObjectPtrCastHelper> badge;
			return TObjectPtr<ToType> { badge, object };
		}
	};
}

/**
 * @brief Attempts to cast the given object pointer to another object pointer.
 *
 * @tparam ToType The destination type.
 * @param object The source object.
 * @return The casted object pointer. Will be null if the cast was not successful.
 */
template<typename ToType>
TObjectPtr<ToType> Cast(FObjectPtr object)
{
	return ::Private::FObjectPtrCastHelper::Cast<ToType>(object);
}

/**
 * @brief Attempts to cast the given object pointer to another object pointer.
 *
 * @tparam ToType The type being casted to.
 * @tparam FromType The type being cast from.
 * @param object The source object.
 * @return The casted object pointer. Will be null if the cast was not successful.
 */
template<typename ToType, typename FromType>
TObjectPtr<ToType> Cast(TObjectPtr<FromType> value)
{
	return ::Private::FObjectPtrCastHelper::Cast<ToType>(value);
}

/**
 * @brief Attempts to cast the given object pointer to another object pointer.
 *
 * @tparam ToType The destination type.
 * @param object The source object.
 * @return The casted object pointer. Will be null if the cast was not successful.
 */
template<typename ToType>
TObjectPtr<ToType> CastChecked(FObjectPtr object)
{
	TObjectPtr<ToType> result = ::Private::FObjectPtrCastHelper::Cast<ToType>(object);
	UM_ASSERT(result.IsValid(), "Failed to cast value to destination type");
	return result;
}

/**
 * @brief Attempts to cast the given object pointer to another object pointer.
 *
 * @tparam ToType The type being casted to.
 * @tparam FromType The type being cast from.
 * @param object The source object.
 * @return The casted object pointer. Will be null if the cast was not successful.
 */
template<typename ToType, typename FromType>
TObjectPtr<ToType> CastChecked(TObjectPtr<FromType> value)
{
	TObjectPtr<ToType> result = ::Private::FObjectPtrCastHelper::Cast<ToType>(value);
	UM_ASSERT(result.IsValid(), "Failed to cast value to destination type");
	return result;
}