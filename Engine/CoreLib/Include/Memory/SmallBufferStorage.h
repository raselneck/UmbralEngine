#pragma once

#include "Containers/Variant.h"
#include "Memory/AlignedStorage.h"
#include "Templates/IsZeroConstructible.h"

#ifndef UMBRAL_STACK_BUFFER_STORAGE_SIZE
#	define UMBRAL_STACK_BUFFER_STORAGE_SIZE 16
#endif

namespace Private
{
	/**
	 * @brief Defines empty buffer storage.
	 */
	class FNoBufferStorage
	{
	};

	/**
	 * @brief Defines stack-based buffer storage.
	 */
	using FStackBufferStorage = TAlignedStorage<UMBRAL_STACK_BUFFER_STORAGE_SIZE, sizeof(void*)>;

	/**
	 * @brief Defines heap-based buffer storage.
	 */
	class FHeapBufferStorage final
	{
		UM_DISABLE_COPY(FHeapBufferStorage);

	public:

		/**
		 * @brief Sets default values for this heap buffer storage's properties.
		 */
		FHeapBufferStorage() = default;

		/**
		 * @brief Assumes ownership of another heap buffer's data.
		 *
		 * @param other The other heap buffer.
		 */
		FHeapBufferStorage(FHeapBufferStorage&& other) noexcept;

		/**
		 * @brief Destroys this heap buffer storage, freeing any allocated memory.
		 */
		~FHeapBufferStorage();

		/**
		 * @brief Allocates memory for this heap buffer of the given size.
		 *
		 * @param size The size of memory to allocate.
		 */
		void Allocate(int32 size);

		/**
		 * @brief Frees any allocated memory.
		 */
		void Free();

		/**
		 * @brief Gets the allocated memory.
		 *
		 * @return The allocated memory.
		 */
		[[nodiscard]] const void* GetData() const
		{
			return m_Memory;
		}

		/**
		 * @brief Gets the allocated memory.
		 *
		 * @return The allocated memory.
		 */
		[[nodiscard]] void* GetData()
		{
			return m_Memory;
		}

		/**
		 * @brief Gets the size, in bytes, of the allocated memory.
		 *
		 * @return The size, in bytes, of the allocated memory.
		 */
		[[nodiscard]] int32 GetDataSize() const
		{
			return m_MemorySize;
		}

		/**
		 * @brief Gets the allocated memory cast to the given type.
		 *
		 * @tparam T The type to cast to.
		 * @return The allocated memory as the given type.
		 */
		template<typename T>
		[[nodiscard]] const T* GetTypedData() const
		{
			return static_cast<const T*>(GetData());
		}

		/**
		 * @brief Gets the allocated memory cast to the given type.
		 *
		 * @tparam T The type to cast to.
		 * @return The allocated memory as the given type.
		 */
		template<typename T>
		[[nodiscard]] T* GetTypedData()
		{
			return static_cast<T*>(GetData());
		}

		/**
		 * @brief Assumes ownership of another heap buffer's data.
		 *
		 * @param other The other heap buffer.
		 * @return This heap buffer.
		 */
		FHeapBufferStorage& operator=(FHeapBufferStorage&& other) noexcept;

	private:

		void* m_Memory = nullptr;
		int32 m_MemorySize = 0;
	};
}

template<>
struct TIsZeroConstructible<Private::FHeapBufferStorage> : FTrueType
{
};

/**
 * @brief Defines buffer storage that is potentially small-buffer optimized.
 */
class FPotentiallySmallBuffer final
{
public:

	UM_DEFAULT_COPY(FPotentiallySmallBuffer);
	UM_DEFAULT_MOVE(FPotentiallySmallBuffer);

	using StorageType = TVariant<Private::FNoBufferStorage, Private::FStackBufferStorage, Private::FHeapBufferStorage>;

	/**
	 * @brief Sets default values for this potentially small buffer's properties.
	 */
	FPotentiallySmallBuffer() = default;

	/**
	 * @brief Destroys this potentially small buffer.
	 */
	~FPotentiallySmallBuffer() = default;

	/**
	 * @brief Ensures this buffer's memory is at least the given size.
	 *
	 * @param size The requested buffer size, in bytes.
	 */
	void Allocate(int32 size);

	/**
	 * @brief Frees any memory this buffer may have allocated.
	 */
	void Free();

	/**
	 * @brief Gets the allocated memory.
	 *
	 * @return The allocated memory.
	 */
	[[nodiscard]] const void* GetData() const;

	/**
	 * @brief Gets the allocated memory.
	 *
	 * @return The allocated memory.
	 */
	[[nodiscard]] void* GetData();

	/**
	 * @brief Gets the size, in bytes, of the allocated memory.
	 *
	 * @return The size, in bytes, of the allocated memory.
	 */
	[[nodiscard]] int32 GetDataSize() const;

	/**
	 * @brief Gets the allocated memory cast to the given type.
	 *
	 * @tparam T The type to cast to.
	 * @return The allocated memory as the given type.
	 */
	template<typename T>
	[[nodiscard]] const T* GetTypedData() const
	{
		return reinterpret_cast<const T*>(GetData());
	}

	/**
	 * @brief Gets the allocated memory cast to the given type.
	 *
	 * @tparam T The type to cast to.
	 * @return The allocated memory as the given type.
	 */
	template<typename T>
	[[nodiscard]] T* GetTypedData()
	{
		return reinterpret_cast<T*>(GetData());
	}

	/**
	 * @brief Checks to see if this buffer storage has any "allocated" memory.
	 *
	 * @return True if this buffer storage has any "allocated" memory, otherwise false.
	 */
	[[nodiscard]] bool IsAllocated() const;

private:

	/**
	 * @brief Checks to see if this buffer storage is currently using heap memory.
	 *
	 * @return True if this buffer storage is currently using heap memory, otherwise false.
	 */
	[[nodiscard]] bool IsUsingHeapBuffer() const;

	/**
	 * @brief Checks to see if this buffer storage is currently using stack memory.
	 *
	 * @return True if this buffer storage is currently using stack memory, otherwise false.
	 */
	[[nodiscard]] bool IsUsingStackBuffer() const;

	StorageType m_Storage;
};

template<>
struct TIsZeroConstructible<FPotentiallySmallBuffer> : TIsZeroConstructible<FPotentiallySmallBuffer::StorageType>
{
};