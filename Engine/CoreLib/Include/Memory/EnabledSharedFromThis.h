#pragma once

#include "Memory/WeakPtr.h"

/**
 * @brief Defines a base for classes that wish to enable retrieving a shared pointer to itself.
 *
 * @remarks Objects that inherit from this should only be created from MakeShared.
 *
 * @tparam T The type of the object to enable retrieving a shared pointer to itself.
 */
template<typename T>
class TEnableSharedFromThis
{
	template<typename U>
	friend class TSharedPtr;

public:

	UM_DISABLE_COPY(TEnableSharedFromThis);
	UM_DEFAULT_MOVE(TEnableSharedFromThis);

	/**
	 * @brief Gets a shared pointer to this object.
	 *
	 * @return A shared pointer to this object.
	 */
	TSharedPtr<const T> AsShared() const
	{
		return m_WeakThis.Pin();
	}

	/**
	 * @brief Gets a shared pointer to this object.
	 *
	 * @return A shared pointer to this object.
	 */
	TSharedPtr<T> AsShared()
	{
		return m_WeakThis.Pin();
	}

	/**
	 * @brief Gets a weak pointer to this object.
	 *
	 * @return A weak pointer to this object.
	 */
	TWeakPtr<const T> AsWeak() const
	{
		return m_WeakThis;
	}

	/**
	 * @brief Gets a weak pointer to this object.
	 *
	 * @return A weak pointer to this object.
	 */
	TWeakPtr<T> AsWeak()
	{
		return m_WeakThis;
	}

protected:

	/**
	 * @brief Sets default values for this object's properties.
	 */
	TEnableSharedFromThis() noexcept = default;

private:

	TWeakPtr<T> m_WeakThis;
};