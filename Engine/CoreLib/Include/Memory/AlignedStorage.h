#pragma once

#include "Engine/Assert.h"
#include "Engine/IntTypes.h"
#include "Templates/IsZeroConstructible.h"

/**
 * @brief Defines aligned storage.
 *
 * @tparam Size The storage size.
 * @tparam Alignment The storage alignment.
 */
template<int32 InSize, int32 InAlignment>
class TAlignedStorage final
{
	static_assert(InSize > 0, "Aligned storage size cannot be zero");
	static_assert(InAlignment > 0, "Aligned storage alignment cannot be zero");

	using ThisType = TAlignedStorage;

public:

	static constexpr int32 Size = InSize;
	static constexpr int32 Alignment = InAlignment;

	/**
	 * @brief Creates new aligned storage.
	 */
	constexpr TAlignedStorage() = default;

	/**
	 * @brief Gets a pointer to this aligned storage's data.
	 *
	 * @return A pointer to this aligned storage's data.
	 */
	[[nodiscard]] const void* GetData() const
	{
		return m_Data;
	}

	/**
	 * @brief Gets a pointer to this aligned storage's data.
	 *
	 * @return A pointer to this aligned storage's data.
	 */
	[[nodiscard]] void* GetData()
	{
		return m_Data;
	}

	/**
	 * @brief Gets a pointer to this aligned storage's data as the given type.
	 *
	 * @tparam T The type to cast to.
	 * @return This aligned storage's data as the given type.
	 */
	template<typename T>
	[[nodiscard]] const T* GetTypedData() const
		requires (sizeof(T) <= InSize)
	{
		return reinterpret_cast<const T*>(GetData());
	}

	/**
	 * @brief Gets a pointer to this aligned storage's data as the given type.
	 *
	 * @tparam T The type to cast to.
	 * @return This aligned storage's data as the given type.
	 */
	template<typename T>
	[[nodiscard]] T* GetTypedData()
		requires (sizeof(T) <= InSize)
	{
		return reinterpret_cast<T*>(GetData());
	}

private:

	alignas(Alignment) uint8 m_Data[Size] { };
};

template<int32 Size, int32 Alignment>
struct TIsZeroConstructible<TAlignedStorage<Size, Alignment>> : FTrueType
{
};