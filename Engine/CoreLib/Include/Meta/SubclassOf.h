#pragma once

#include "Meta/ClassInfo.h"
#include "Templates/IsMetaType.h"
#include "Templates/IsTypeComplete.h"

class UObject;

/**
 * @brief Defines a templated way to store a subclass.
 *
 * @tparam T The subclass.
 */
template<typename T>
class TSubclassOf
{
	static_assert(Or<TIsTypeIncomplete<T>, TIsMetaClass<T>>, "TSubclassOf can only be used with classes that have meta information available");

public:

	/**
	 * @brief Sets the underlying class to nullptr.
	 */
	TSubclassOf() = default;

	/**
	 * @brief Sets the underlying class.
	 *
	 * @param theClass The underlying class.
	 */
	TSubclassOf(const FClassInfo* classInfo)
		: m_ClassInfo { classInfo }
	{
	}

	/**
	 * @brief Gets the underlying class.
	 *
	 * @return The underlying class.
	 */
	[[nodiscard]] const FClassInfo* GetClass() const
	{
		return IsValid() ? m_ClassInfo : nullptr;
	}

	/**
	 * @brief Gets the name of the underlying class, whether it is valid or not.
	 *
	 * @return The name of the underlying class.
	 */
	[[nodiscard]] FStringView GetClassName() const
	{
		if (m_ClassInfo == nullptr)
		{
			return "<null>"_sv;
		}

		return m_ClassInfo->GetName();
	}

	/**
	 * @brief Checks to see if the underlying class is null or invalid
	 *
	 * @return True if the underlying class is null or invalid.
	 */
	[[nodiscard]] bool IsNull() const
	{
		return IsValid() == false;
	}

	/**
	 * @brief Checks to see if the underlying class is valid and is a \tparam T.
	 *
	 * @return True if the underlying class is valid and is a \tparam T, otherwise false.
	 */
	[[nodiscard]] bool IsValid() const
	{
		return m_ClassInfo != nullptr && m_ClassInfo->IsA(T::StaticType());
	}

	/**
	 * @brief Gets the underlying class.
	 *
	 * @return The underlying class.
	 */
	[[nodiscard]] operator const FClassInfo*() const
	{
		return GetClass();
	}

	/**
	 * @brief Gets the underlying class.
	 *
	 * @return The underlying class.
	 */
	[[nodiscard]] const FClassInfo* operator->() const
	{
		return GetClass();
	}

private:

	const FClassInfo* m_ClassInfo = nullptr;
};