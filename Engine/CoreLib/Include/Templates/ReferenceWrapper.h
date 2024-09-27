#pragma once

#include "Templates/AddressOf.h"
#include "Templates/Declval.h"
#include "Templates/EnableIf.h"
#include "Templates/Forward.h"
#include "Templates/IsConstVolatile.h"
#include "Templates/IsSame.h"

namespace Private
{
	template<class T> constexpr T& RefWrapTest(T& t) noexcept { return t; }
	template<class T> void RefWrapTest(T&&) = delete;
}

/**
 * @brief Wraps a reference in a copyable, assignable object.
 *
 * @tparam T The type of the underlying reference.
 */
template<typename T>
class TReferenceWrapper
{
public:

	using Type = T;

	/**
	 * @brief Creates a new reference wrapper for the given value.
	 *
	 * @tparam U The value's type.
	 * @param value The value.
	 */
	template<class U, class = decltype(
		Private::RefWrapTest<T>(declval<U>()),
		EnableIf<IsSame<TReferenceWrapper, RemoveCVRef<U>> == false, void>()
	)>
	constexpr TReferenceWrapper(U&& value)
		noexcept(noexcept(Private::RefWrapTest<T>(Forward<U>(value))))
		: m_Value(AddressOf(Private::RefWrapTest<T>(Forward<U>(value))))
	{
	}

	TReferenceWrapper(const TReferenceWrapper&) noexcept = default;
	TReferenceWrapper& operator=(const TReferenceWrapper&) noexcept = default;

	/**
	 * @brief Gets the underlying reference.
	 *
	 * @return The underlying reference.
	 */
	constexpr T& Get() const noexcept
	{
		return *m_Value;
	}

	/**
	 * @brief Gets the underlying reference.
	 *
	 * @return The underlying reference.
	 */
	constexpr operator T&() const noexcept
	{
		return Get();
	}

private:

	T* m_Value = nullptr;
};

// Deduction guide
template<class T>
TReferenceWrapper(T&) -> TReferenceWrapper<T>;