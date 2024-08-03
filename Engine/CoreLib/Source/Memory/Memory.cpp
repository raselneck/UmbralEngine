#include "Engine/Assert.h"
#include "Memory/Memory.h"
#include <cstring>
#include <mimalloc.h>

void* FMemory::Allocate(const SizeType numBytes)
{
	UM_ASSERT(numBytes >= 0, "Attempting to allocate a negative number of bytes");

	if (numBytes == 0)
	{
		return nullptr;
	}

	constexpr SizeType maximumSmallAllocationSize = static_cast<SizeType>(MI_SMALL_SIZE_MAX);
	if (numBytes <= maximumSmallAllocationSize)
	{
		return mi_zalloc_small(static_cast<size_t>(numBytes));
	}

	return mi_zalloc(static_cast<size_t>(numBytes));
}

void* FMemory::AllocateAligned(const SizeType numBytes, const SizeType alignment)
{
	UM_ASSERT(numBytes >= 0, "Attempting to allocate a negative number of bytes");
	UM_ASSERT(alignment >= 1, "Attempting to allocate with an alignment of zero or less");

	if (numBytes == 0)
	{
		return nullptr;
	}

	return mi_zalloc_aligned(static_cast<size_t>(numBytes), static_cast<size_t>(alignment));
}

void* FMemory::AllocateArray(SizeType numElements, SizeType elementSize)
{
	UM_ASSERT(numElements >= 0, "Attempting to allocate a negative number of elements");
	UM_ASSERT(elementSize > 0, "Attempting to allocate invalidly sized array elements");

	const usize numBytes = static_cast<usize>(numElements) * static_cast<usize>(elementSize);
	if (numBytes <= MI_SMALL_SIZE_MAX)
	{
		return mi_zalloc_small(numBytes);
	}

	return mi_zalloc(numBytes);
}

void FMemory::Copy(void* destination, const void* source, const SizeType numBytes)
{
	UM_ASSERT(numBytes >= 0, "Attempting to copy a negative number of bytes");
	::memcpy(destination, source, static_cast<size_t>(numBytes));
}

void FMemory::CopyInReverse(void* destination, const void* source, const SizeType numBytes)
{
	// TODO Might be able to speed this up by doing a normal memcpy for the non-overlapping region and then using this loop for the overlap

	uint8* dstBuffer = reinterpret_cast<uint8*>(destination);
	const uint8* srcBuffer = reinterpret_cast<const uint8*>(source);
	for (SizeType idx = numBytes - 1; idx >= 0; --idx)
	{
		dstBuffer[idx] = srcBuffer[idx];
	}
}

void FMemory::Free(void* memory)
{
	if (memory == nullptr)
	{
		return;
	}

	mi_free(memory);
}

void FMemory::FreeAligned(void* memory)
{
	if (memory == nullptr)
	{
		return;
	}

	// TODO Need to determine alignment for this
	//mi_free_aligned(memory, alignment);

	mi_free(memory);
}

void FMemory::Move(void* destination, const void* source, const SizeType numBytes)
{
	UM_ASSERT(numBytes >= 0, "Attempting to move a negative number of bytes");

	// TODO: Look into whether or not we need to use a mimalloc function
	::memmove(destination, source, static_cast<size_t>(numBytes));
}

void* FMemory::Reallocate(void* memory, const SizeType newNumBytes)
{
	UM_ASSERT(newNumBytes >= 0, "Attempting to re-allocate a negative number of bytes");

	if (newNumBytes == 0)
	{
		Free(memory);
		return nullptr;
	}

	return mi_rezalloc(memory, static_cast<size_t>(newNumBytes));
}

void* FMemory::ReallocateAligned(void* memory, const SizeType newNumBytes, const SizeType alignment)
{
	UM_ASSERT(newNumBytes >= 0, "Attempting to re-allocate a negative number of bytes");

	if (newNumBytes == 0)
	{
		FreeAligned(memory);
		return nullptr;
	}

	return mi_rezalloc_aligned(memory, static_cast<size_t>(newNumBytes), static_cast<size_t>(alignment));
}

void FMemory::ZeroOut(void* memory, const SizeType numBytes)
{
	if (memory == nullptr || numBytes == 0)
	{
		return;
	}

	UM_ASSERT(numBytes > 0, "Attempting to zero out a negative number of bytes");

	::memset(memory, 0, static_cast<usize>(numBytes));
}

void FMemory::ZeroOutArray(void* memory, SizeType elementSize, SizeType numElements)
{
	if (memory == nullptr || elementSize == 0 || numElements == 0)
	{
		return;
	}

	UM_ASSERT(elementSize > 0, "Attempting to zero out an array of negatively sized elements");
	UM_ASSERT(numElements > 0, "Attempting to zero out a negative number of elements");

	const usize numBytes = static_cast<usize>(elementSize) * static_cast<usize>(numElements);
	::memset(memory, 0, numBytes);
}