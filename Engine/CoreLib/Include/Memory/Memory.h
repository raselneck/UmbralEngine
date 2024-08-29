#pragma once

#include "Engine/IntTypes.h"
#include "Templates/AndNotOr.h"
#include "Templates/IsBaseOf.h"
#include "Templates/IsTypeComplete.h"
#include "Templates/IsZeroConstructible.h"
#include "Templates/Forward.h"
#include "Templates/Move.h"
#include <new> /* For placement new */

// TODO To help make memory a little safer, can add an FMemoryHandle type that is basically a TSpan<uint8>

/**
 * @brief Defines a way to interact with low-level memory functions.
 */
class FMemory
{
public:

	using SizeType = int64;

	/**
	 * @brief Allocates a block of memory.
	 *
	 * @param numBytes The number of bytes to allocate.
	 * @return The allocated memory.
	 */
	[[nodiscard]] static void* Allocate(SizeType numBytes);

	/**
	 * @brief Allocates a block of aligned memory.
	 *
	 * @param numBytes The number of bytes to allocate.
	 * @param alignment The memory alignment to use, in bytes.
	 * @return The allocated memory.
	 */
	[[nodiscard]] static void* AllocateAligned(SizeType numBytes, SizeType alignment);

	/**
	 * @brief Allocates memory for an array.
	 *
	 * @param numElements The number of elements in the array.
	 * @param elementSize The size of each element in the array.
	 * @return The allocated memory.
	 */
	[[nodiscard]] static void* AllocateArray(SizeType numElements, SizeType elementSize);

	/**
	 * @brief Allocates memory for an array.
	 *
	 * @tparam ElementType The type of each element in the array.
	 * @param numElements The number of elements in the array.
	 * @return The allocated memory.
	 */
	template<typename ElementType>
	[[nodiscard]] static ElementType* AllocateArray(SizeType numElements)
	{
		return static_cast<ElementType*>(AllocateArray(numElements, sizeof(ElementType)));
	}

	/**
	 * @brief Allocates an object.
	 *
	 * @tparam ElementType The object's type.
	 * @tparam ConstructTypes The types of the arguments to pass along to the object's constructor.
	 * @param args The arguments to pass along to the object's constructor.
	 * @return A pointer to the allocated object.
	 */
	template<typename ElementType, typename... ConstructTypes>
	[[nodiscard]] static ElementType* AllocateObject(ConstructTypes... args)
	{
		void* objectMemory = Allocate(sizeof(ElementType));
		ConstructObjectAt<ElementType, ConstructTypes...>(objectMemory, Forward<ConstructTypes>(args) ...);
		return reinterpret_cast<ElementType*>(objectMemory);
	}

	/**
	 * @brief Constructs an object.
	 *
	 * @tparam ElementType The object's type.
	 * @tparam ConstructTypes The types of the arguments to pass along to the object's constructor.
	 * @param args The arguments to pass along to the object's constructor.
	 * @return A pointer to the allocated object.
	 */
	template<typename ElementType, typename... ConstructTypes>
	static ElementType ConstructObject(ConstructTypes... args)
	{
		ElementType value { Forward<ConstructTypes>(args)... };
		return MoveTemp(value);
	}

	/**
	 * @brief Constructs an object at the given memory location.
	 *
	 * @tparam ElementType The object's type.
	 * @tparam ConstructTypes The types of the arguments to pass along to the object's constructor.
	 * @param objectMemory The memory location.
	 * @param args The arguments to pass along to the object's constructor.
	 * @return A pointer to the allocated object.
	 */
	template<typename ElementType, typename... ConstructTypes>
	static void ConstructObjectAt(void* objectMemory, ConstructTypes... args)
	{
		if constexpr (IsZeroConstructible<ElementType> && sizeof...(args) == 0)
		{
			ZeroOut(objectMemory, sizeof(ElementType));
		}
		else
		{
			new (objectMemory) ElementType(Forward<ConstructTypes>(args) ...);
		}
	}

	/**
	 * @brief Copies memory from one location to another.
	 *
	 * @param destination The destination location.
	 * @param source The source location.
	 * @param numBytes The number of bytes to copy.
	 */
	static void Copy(void* destination, const void* source, SizeType numBytes);

	/**
	 * @brief Copies memory from one location to another starting at the end of the buffers.
	 *
	 * Note that this is much slower than Copy, but can be safer for certain operations (like prepending buffers).
	 *
	 * @param destination The destination buffer.
	 * @param source The source buffer.
	 * @param numBytes The number of bytes to copy.
	 */
	static void CopyInReverse(void* destination, const void* source, SizeType numBytes);

	/**
	 * @brief Destructs an object.
	 *
	 * @tparam ElementType The object's type.
	 * @param object The object to destruct. Assumed to be non-null.
	 */
	template<typename ElementType>
	static void DestructObject(ElementType* object)
	{
		using ElementTypeToDestruct = ElementType;
		object->~ElementTypeToDestruct();
	}

	/**
	 * @brief Frees a block of allocated memory.
	 *
	 * @param memory The block of allocated memory.
	 */
	static void Free(void* memory);

	/**
	 * @brief Frees a block of memory allocated using AllocateAligned.
	 *
	 * @param memory The block of memory.
	 */
	static void FreeAligned(void* memory);

	/**
	 * @brief Frees an allocated object.
	 *
	 * @tparam ElementType The object's type.
	 * @param object The object to free. Assumed to be non-null.
	 */
	template<typename ElementType>
	static void FreeObject(ElementType* object)
	{
		DestructObject(object);
		Free(object);
	}

	/**
	 * @brief Moves memory from one location to another.
	 *
	 * @param destination The destination location.
	 * @param source The source location.
	 * @param numBytes The number of bytes to move.
	 */
	static void Move(void* destination, const void* source, SizeType numBytes);

	/**
	 * @brief Re-allocates a block of memory to have a new size.
	 *
	 * @param memory The memory block to re-allocate.
	 * @param newNumBytes The new number of bytes for the block of memory.
	 * @return The location of the re-allocated block of memory.
	 */
	[[nodiscard]] static void* Reallocate(void* memory, SizeType newNumBytes);

	/**
	 * @brief Re-allocates a block of aligned memory to have a new size.
	 *
	 * @param memory The memory block to re-allocate.
	 * @param newNumBytes The new number of bytes for the block of memory.
	 * @param alignment The alignment.
	 * @return The location of the re-allocated block of memory.
	 */
	[[nodiscard]] static void* ReallocateAligned(void* memory, SizeType newNumBytes, SizeType alignment);

	/**
	 * @brief Zeroes out the given memory.
	 *
	 * @param memory The memory.
	 * @param numBytes The number of bytes to zero out.
	 */
	static void ZeroOut(void* memory, SizeType numBytes);

	/**
	 * @brief Zeroes out the given array.
	 *
	 * @param memory The array memory.
	 * @param elementSize The size of each element in the array.
	 * @param numElements The number of elements in the array.
	 */
	static void ZeroOutArray(void* memory, SizeType elementSize, SizeType numElements);

	/**
	 * @brief Zeroes out the given array.
	 *
	 * @tparam ElementType The type of each element in the array.
	 * @param elements The array elements to zero out.
	 * @param numElements The number of elements in the array.
	 */
	template<typename ElementType>
	static void ZeroOutArray(ElementType* elements, const SizeType numElements)
	{
		ZeroOutArray(elements, static_cast<SizeType>(sizeof(ElementType)), numElements);
	}

	/**
	 * @brief Zeroes out the given array.
	 *
	 * @tparam ElementType The type of each element in the array.
	 * @tparam N The number of elements in the array.
	 * @param elements The array elements to zero out.
	 */
	template<typename ElementType, SizeType N>
	static void ZeroOutArray(ElementType (&elements)[N])
	{
		ZeroOutArray(elements, static_cast<SizeType>(sizeof(ElementType)), N);
	}
};