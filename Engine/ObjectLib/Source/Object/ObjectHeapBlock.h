#pragma once

#include "Containers/Array.h"
#include "Object/ObjectHeader.h"
#include "Memory/UniquePtr.h"

/**
 * @brief Defines a block of memory for the object heap.
 */
class FObjectHeapBlock final
{
	friend class FMemory;

	UM_DISABLE_COPY(FObjectHeapBlock);
	UM_DISABLE_MOVE(FObjectHeapBlock);

public:

	/**
	 * @brief Gets the allocated size of each heap block.
	 */
	static constexpr int32 BlockSize = 16 * 1024;

	/**
	 * @brief Destroys this object heap block.
	 */
	~FObjectHeapBlock();

	/**
	 * @brief Adds all valid object headers in this block to the given list.
	 *
	 * @param headers The list of valid object headers.
	 */
	void AccumulateValidHeaders(TArray<FObjectHeader*>& headers) const;

	/**
	 * @brief Attempts to allocate an object with the given class.
	 *
	 * @param objectClass The object's class.
	 * @return The location where the object can be allocated, or nullptr if it could not be allocated.
	 */
	[[nodiscard]] void* Allocate(const FClassInfo* objectClass);

	/**
	 * @brief Checks to see if this object heap block can allocate an object of the given size.
	 *
	 * @param size The object size.
	 * @return True if this object heap block can allocate an object with \p size bytes, otherwise false.
	 */
	[[nodiscard]] bool CanAllocate(int32 size);

	/**
	 * @brief Creates a new heap block.
	 *
	 * @param cellSize The heap block's cell size.
	 * @return The object heap block.
	 */
	[[nodiscard]] static TUniquePtr<FObjectHeapBlock> Create(int32 cellSize);

	/**
	 * @brief Deletes a cell marked for deletion.
	 *
	 * @param cell The cell.
	 */
	void DeleteMarkedCell(TBadge<class FObjectHeap>, FObjectHeader* cell);

	/**
	 * @brief Deletes all marked objects in this heap block.
	 */
	void DeleteMarkedObjects();

	/**
	 * @brief Gets the next cell size for a given object size.
	 *
	 * @param objectSize The size of the object.
	 * @return The cell size for the object size.
	 */
	[[nodiscard]] static int32 GetAlignedCellSize(int32 objectSize);

	/**
	 * @brief Gets the collection of aligned cell sizes.
	 *
	 * @return The collection of aligned cell sizes.
	 */
	[[nodiscard]] static TSpan<const int32> GetAlignedCellSizes();

	/**
	 * @brief Gets the cell at the given index.
	 *
	 * @param index The cell's index.
	 * @return The cell.
	 */
	[[nodiscard]] FObjectHeader* GetCell(int32 index) const;

	/**
	 * @brief Gets the number of cells in this block.
	 *
	 * @return The number of cells in this block.
	 */
	[[nodiscard]] int32 GetNumCells() const;

	/**
	 * @brief Checks to see if this heap block owns the given cell.
	 *
	 * @param cell The cell.
	 * @return True if \p cell belongs to this heap block, otherwise false.
	 */
	[[nodiscard]] bool OwnsCell(const FObjectHeader* cell) const;

private:

	/**
	 * @brief Creates a new object heap block.
	 *
	 * @param cellSize
	 */
	explicit FObjectHeapBlock(int32 cellSize);

	FObjectHeader* m_FreeList = nullptr;
	int32 m_CellSize = 0;
};