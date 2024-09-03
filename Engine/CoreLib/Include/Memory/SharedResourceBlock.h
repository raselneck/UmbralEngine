#pragma once

#include "Engine/IntTypes.h"
#include "Memory/Memory.h"
#include <atomic>

namespace Private
{
	/**
	 * @brief Defines the interface for shared resource blocks.
	 */
	class ISharedResourceBlock
	{
	public:

		/**
		 * @brief Destroys this shared resource block.
		 */
		virtual ~ISharedResourceBlock() = default;

		/**
		 * @brief Adds a strong reference to this shared resource block.
		 */
		virtual void AddStrongRef() = 0;

		/**
		 * @brief Adds a weak reference to this shared resource block.
		 */
		virtual void AddWeakRef() = 0;

		/**
		 * @brief Gets a pointer to the underlying object.
		 *
		 * @return A pointer to the underlying object.
		 */
		[[nodiscard]] virtual const void* GetObject() const
		{
			return const_cast<ISharedResourceBlock*>(this)->GetObject();
		}

		/**
		 * @brief Gets a pointer to the underlying object.
		 *
		 * @return A pointer to the underlying object.
		 */
		[[nodiscard]] virtual void* GetObject() = 0;

		/**
		 * @brief Gets a typed pointer to the underlying object.
		 *
		 * @tparam T The type to retrieve the object as.
		 * @return A typed pointer to the underlying object.
		 */
		template<typename T>
		[[nodiscard]] const T* GetTypedObject() const
		{
			return reinterpret_cast<const T*>(GetObject());
		}

		/**
		 * @brief Gets a typed pointer to the underlying object.
		 *
		 * @tparam T The type to retrieve the object as.
		 * @return A typed pointer to the underlying object.
		 */
		template<typename T>
		[[nodiscard]] T* GetTypedObject()
		{
			return reinterpret_cast<T*>(GetObject());
		}

		/**
		 * @brief Returns a value indicating whether or not this shared resource block has an object.
		 *
		 * @return True if this shared resource block has an object, otherwise false.
		 */
		[[nodiscard]] virtual bool HasObject() const = 0;

		/**
		 * @brief Removes a strong reference from this shared resource block.
		 */
		virtual void RemoveStrongRef() = 0;

		/**
		 * @brief Removes a weak reference from this shared resource block.
		 */
		virtual void RemoveWeakRef() = 0;

#if WITH_TESTING
		/**
		 * @brief Gets the number of strong references to this resource block.
		 *
		 * @return The number of strong references to this resource block.
		 */
		[[nodiscard]] virtual int32 GetStrongRefCount() const = 0;

		/**
		 * @brief Gets the number of weak references to this resource block.
		 *
		 * @return The number of weak references to this resource block.
		 */
		[[nodiscard]] virtual int32 GetWeakRefCount() const = 0;
#endif

	protected:

		ISharedResourceBlock() = default;
	};

	/**
	 * @brief An enumeration of shared resource allocation strategies.
	 */
	enum class ESharedResourceAllocStrategy
	{
		/**
		 * @brief Only the object was allocated.
		 */
		Object,
		/**
		 * @brief The object and resource block were allocated together.
		 * */
		ObjectAndBlock
	};

	// TODO If the alloc strategy is ObjectAndBlock, we don't need to store the object directly because it will be right after the block in storage

	/**
	 * @brief Defines a shared resource block for a concrete type.
	 *
	 * @tparam T The underlying object type.
	 * @tparam AllocStrategy The resource block's allocation strategy.
	 */
	template<typename T, ESharedResourceAllocStrategy AllocStrategy = ESharedResourceAllocStrategy::Object>
	class TSharedResourceBlock : public ISharedResourceBlock
	{
	public:

		using ElementType = T;

		/**
		 * @brief Sets default values for this resource block's properties.
		 *
		 * @param object The underlying object to manage.
		 */
		explicit TSharedResourceBlock(ElementType* object)
			: m_StrongRefCount { 1 }
			, m_WeakRefCount { 1 }
			, m_Object { object }
		{
		}

		/** @copydoc ISharedResourceBlock::AddStrongRef() */
		virtual void AddStrongRef() override
		{
			++m_StrongRefCount;
		}

		/** @copydoc ISharedResourceBlock::AddWeakRef() */
		virtual void AddWeakRef() override
		{
			++m_WeakRefCount;
		}

		/** @copydoc ISharedResourceBlock::GetObject() */
		virtual void* GetObject() override
		{
			return m_Object;
		}

		/** @copydoc ISharedResourceBlock::HasObject() */
		virtual bool HasObject() const override
		{
			return m_Object != nullptr;
		}

		/** @copydoc ISharedResourceBlock::RemoveStrongRef() */
		virtual void RemoveStrongRef() override
		{
			if (--m_StrongRefCount > 0)
			{
				return;
			}

			if (UM_ENSURE(m_Object != nullptr))
			{
				// If the allocation strategy was for both the object and this resource block, then we just need to
				// destroy the object but keep ourselves alive in case there are any weak references
				if constexpr (AllocStrategy == ESharedResourceAllocStrategy::ObjectAndBlock)
				{
					FMemory::DestructObject(m_Object);
				}
				else if constexpr (AllocStrategy == ESharedResourceAllocStrategy::Object)
				{
					FMemory::Free(m_Object);
				}
				m_Object = nullptr;
			}

			RemoveWeakRef();
		}

		/** @copydoc ISharedResourceBlock::RemoveWeakRef() */
		virtual void RemoveWeakRef() override
		{
			if (--m_WeakRefCount > 0)
			{
				return;
			}

			// If AllocStrategy is Object, then we were created using AllocResourceBlock below, so we just need to
			// free ourselves. If it is ObjectAndBlock then the memory for us and the object was allocated using
			// AllocForMakeShared below, in which case our memory location is the one that needs to be freed (because
			// we are allocated first in the chunk containing both). In both cases, the following call is valid
			FMemory::FreeObject(this);
		}

#if WITH_TESTING
		/** @copydoc ISharedResourceBlock::GetStrongRefCount() */
		[[nodiscard]] virtual int32 GetStrongRefCount() const override
		{
			return m_StrongRefCount;
		}

		/** @copydoc ISharedResourceBlock::GetWeakRefCount() */
		[[nodiscard]] virtual int32 GetWeakRefCount() const override
		{
			return m_WeakRefCount;
		}
#endif

	private:

		std::atomic<int32> m_StrongRefCount = 0;
		std::atomic<int32> m_WeakRefCount = 0;
		ElementType* m_Object = nullptr;
	};

	/**
	 * @brief Allocates a shared resource block.
	 *
	 * @tparam ElementType The underlying object type.
	 * @tparam AllocStrategy The shared resource block's allocation strategy.
	 * @param object The underlying object.
	 * @return The shared resource block for \p object. Can be null if \p object is null.
	 */
	template<typename ElementType>
	ISharedResourceBlock* AllocResourceBlock(ElementType* object)
	{
		if (object == nullptr)
		{
			return nullptr;
		}

		using FResourceBlock = TSharedResourceBlock<ElementType, ESharedResourceAllocStrategy::Object>;
		return FMemory::AllocateObject<FResourceBlock>(object);
	}

	/**
	 * @brief Gets the size of a combined resource block for the given type.
	 *
	 * @tparam ElementType The element type.
	 * @return The size of a combined resource block.
	 */
	template<typename ElementType>
	int32 GetCombinedResourceBlockSize()
	{
		using FResourceBlock = TSharedResourceBlock<ElementType, ESharedResourceAllocStrategy::ObjectAndBlock>;
		return static_cast<int32>(sizeof(ElementType) + sizeof(FResourceBlock));
	}

	/**
	 * @brief Allocates a shared resource block and its underlying object at the given memory location.
	 *
	 * @tparam ElementType The underlying object type.
	 * @tparam ConstructTypes The types of the arguments to pass along to the object's constructor.
	 * @param memory The memory location to be allocated at. Must be at least as many bytes as returned by GetCombinedResourceBlockSize<ElementType>().
	 * @param args The arguments to pass along to the object's constructor.
	 * @return The shared resource block for the allocated object.
	 */
	template<typename ElementType, typename... ConstructTypes>
	ISharedResourceBlock* AllocCombinedResourceBlockAt(void* memory, ConstructTypes&&... args)
	{
		using FResourceBlock = TSharedResourceBlock<ElementType, ESharedResourceAllocStrategy::ObjectAndBlock>;

		constexpr usize resourceBlockOffset = 0;
		constexpr usize objectOffset = sizeof(FResourceBlock);

		if (memory == nullptr)
		{
			return nullptr;
		}

		// Construct the object FIRST so the resource block points to something valid
		void* objectLocation = reinterpret_cast<uint8*>(memory) + objectOffset;
		FMemory::ConstructObjectAt<ElementType>(objectLocation, Forward<ConstructTypes>(args)...);

		// Construct the resource block
		void* resourceBlockLocation = reinterpret_cast<uint8*>(memory) + resourceBlockOffset;
		FMemory::ConstructObjectAt<FResourceBlock>(resourceBlockLocation, reinterpret_cast<ElementType*>(objectLocation));

		return reinterpret_cast<FResourceBlock*>(resourceBlockLocation);
	}

	/**
	 * @brief Allocates enough memory for a shared resource block and its underlying object.
	 *
	 * @tparam ElementType The underlying object type.
	 * @tparam ConstructTypes The types of the arguments to pass along to the object's constructor.
	 * @param args The arguments to pass along to the object's constructor.
	 * @return The shared resource block for the allocated object.
	 */
	template<typename ElementType, typename... ConstructTypes>
	ISharedResourceBlock* AllocCombinedResourceBlock(ConstructTypes&&... args)
	{
		const int32 resourceBlockSize = GetCombinedResourceBlockSize<ElementType>();
		void* resourceBlockMemory = FMemory::Allocate(resourceBlockSize);
		if (resourceBlockMemory == nullptr)
		{
			return nullptr;
		}

		return AllocCombinedResourceBlockAt<ElementType>(resourceBlockMemory, Forward<ConstructTypes>(args)...);
	}
}