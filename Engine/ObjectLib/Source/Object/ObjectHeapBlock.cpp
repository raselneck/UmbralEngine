#include "Containers/StaticArray.h"
#include "Object/Object.h"
#include "Object/ObjectHeapBlock.h"

static const TStaticArray<int32, 11> GObjectHeapBlockSizes
{{
	128,
	//160,
	192,
	//224,
	256,
	//288,
	320,
	//352,
	384,
	//416,
	448,
	//480,
	512,
	//544,
	//576,
	//608,
	640,
	//672,
	//704,
	//736,
	768,
	//800,
	//832,
	//864,
	896,
	//928,
	//960,
	//992,
	1024
}};

FObjectHeapBlock::FObjectHeapBlock(const int32 cellSize)
	: m_CellSize { cellSize }
{
	UM_ASSERT(GObjectHeapBlockSizes.Contains(cellSize), "Given cell size is not supported");

	m_FreeList = GetCell(0);

	const int32 numCells = GetNumCells();
	for (int32 idx = 0; idx < numCells; ++idx)
	{
		GetCell(idx)->NotifyDestroyed({}, GetCell(idx + 1));
	}
}

FObjectHeapBlock::~FObjectHeapBlock()
{
	constexpr TBadge<FObjectHeapBlock> badge;

	const int32 numCells = GetNumCells();
	for (int32 idx = 0; idx < numCells; ++idx)
	{
		FObjectHeader* cell = GetCell(idx);
		if (cell->HasObject())
		{
			cell->NotifyDestroyed(badge, nullptr);
		}
	}
}

void FObjectHeapBlock::AccumulateValidHeaders(TArray<FObjectHeader*>& headers) const
{
	const int32 numCells = GetNumCells();
	for (int32 idx = 0; idx < numCells; ++idx)
	{
		FObjectHeader* cell = GetCell(idx);
		if (cell->HasObject())
		{
			headers.Add(cell);
		}
	}
}

void* FObjectHeapBlock::Allocate(const FClassInfo* objectClass)
{
	if (objectClass == nullptr ||
	    CanAllocate(objectClass->GetSize()) == false ||
	    m_FreeList == nullptr)
	{
		return nullptr;
	}

	FObjectHeader* objectHeader = m_FreeList;
	m_FreeList = m_FreeList->GetNextFreeHeader();

	constexpr TBadge<FObjectHeapBlock> badge;
	objectHeader->NotifyAllocated(badge);

	return static_cast<void*>(objectHeader->GetObject());
}

bool FObjectHeapBlock::CanAllocate(const int32 size)
{
	return m_CellSize - static_cast<int32>(sizeof(FObjectHeader)) >= size;
}

TUniquePtr<FObjectHeapBlock> FObjectHeapBlock::Create(const int32 cellSize)
{
	void* heapBlockLocation = FMemory::Allocate(FObjectHeapBlock::BlockSize);
	FMemory::ConstructObjectAt<FObjectHeapBlock>(heapBlockLocation, cellSize);

	return TUniquePtr<FObjectHeapBlock> { reinterpret_cast<FObjectHeapBlock*>(heapBlockLocation) };
}

void FObjectHeapBlock::DeleteMarkedCell(TBadge<class FObjectHeap>, FObjectHeader* cell)
{
	UM_ASSERT(OwnsCell(cell), "Attempting to delete object from heap block it does not belong to");

	constexpr TBadge<FObjectHeapBlock> badge;
	cell->NotifyDestroyed(badge, m_FreeList);
	m_FreeList = cell;
}

void FObjectHeapBlock::DeleteMarkedObjects()
{
	constexpr TBadge<FObjectHeapBlock> badge;

	const int32 numCells = GetNumCells();
	for (int32 idx = 0; idx < numCells; ++idx)
	{
		FObjectHeader* cell = GetCell(idx);
		UObject* object = cell->GetObject();

		if (object && object->IsMarkedForGarbageCollection())
		{
			cell->NotifyDestroyed(badge, m_FreeList);
			m_FreeList = cell;
		}
	}
}

int32 FObjectHeapBlock::GetAlignedCellSize(const int32 objectSize)
{
	const int32 index = GObjectHeapBlockSizes.IndexOfByPredicate([objectSize](const int32 cellSize)
	{
		return cellSize - static_cast<int32>(sizeof(FObjectHeader)) >= objectSize;
	});

	UM_ASSERT(index != INDEX_NONE, "Given object size is too large for any cell size");

	return GObjectHeapBlockSizes[index];
}

TSpan<const int32> FObjectHeapBlock::GetAlignedCellSizes()
{
	return GObjectHeapBlockSizes;
}

FObjectHeader* FObjectHeapBlock::GetCell(const int32 index) const
{
	if (index < 0 || index >= GetNumCells())
	{
		return nullptr;
	}

	uint8* blockLocation = reinterpret_cast<uint8*>(const_cast<FObjectHeapBlock*>(this));
	uint8* firstCellLocation = blockLocation + sizeof(FObjectHeapBlock);

	return reinterpret_cast<FObjectHeader*>(firstCellLocation + index * m_CellSize);
}

int32 FObjectHeapBlock::GetNumCells() const
{
	constexpr int32 memorySizeForCells = FObjectHeapBlock::BlockSize - sizeof(FObjectHeapBlock);
	return memorySizeForCells / m_CellSize;
}

bool FObjectHeapBlock::OwnsCell(const FObjectHeader* cell) const
{
	const uint8* startLocation = reinterpret_cast<const uint8*>(this) + sizeof(FObjectHeapBlock);
	const uint8* endLocation = startLocation + FObjectHeapBlock::BlockSize;
	const void* cellLocation = static_cast<const void*>(cell);

	return cellLocation >= startLocation && cellLocation <= endLocation;
}