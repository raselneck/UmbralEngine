#include "Memory/SmallBufferStorage.h"
#include "Memory/Memory.h"

namespace Private
{
	FHeapBufferStorage::FHeapBufferStorage(FHeapBufferStorage&& other) noexcept
		: m_Memory { other.m_Memory }
		, m_MemorySize { other.m_MemorySize }
	{
		other.m_Memory = nullptr;
		other.m_MemorySize = 0;
	}

	FHeapBufferStorage::~FHeapBufferStorage()
	{
		Free();
	}

	void FHeapBufferStorage::Allocate(const int32 size)
	{
		if (m_MemorySize >= size)
		{
			return;
		}

		if (m_Memory)
		{
			FMemory::Free(m_Memory);

			m_Memory = nullptr;
			m_MemorySize = 0;
		}

		if (size > 0)
		{
			m_Memory = FMemory::Allocate(size);
			m_MemorySize = size;
		}
	}

	void FHeapBufferStorage::Free()
	{
		if (m_Memory == nullptr)
		{
			return;
		}

		FMemory::Free(m_Memory);

		m_Memory = nullptr;
		m_MemorySize = 0;
	}

	FHeapBufferStorage& FHeapBufferStorage::operator=(FHeapBufferStorage&& other) noexcept
	{
		if (this == &other)
		{
			return *this;
		}

		Free();
		m_Memory = other.m_Memory;
		m_MemorySize = other.m_MemorySize;
		other.m_Memory = nullptr;
		other.m_MemorySize = 0;

		return *this;
	}
}

void FPotentiallySmallBuffer::Allocate(const int32 size)
{
	if (size == 0)
	{
		Free();
		return;
	}

	UM_ASSERT(size > 0, "Attempting to allocate negative number of bytes for potentially small buffer");

	const bool needsHeap = size > Private::FStackBufferStorage::Size;
	if (needsHeap)
	{
		if (IsUsingHeapBuffer() == false)
		{
			m_Storage.ResetToType<Private::FHeapBufferStorage>();
		}

		Private::FHeapBufferStorage& heapBuffer = m_Storage.GetValue<Private::FHeapBufferStorage>();
		heapBuffer.Allocate(size);
	}
	else
	{
		if (IsUsingHeapBuffer()) // Don't need the heap buffer but we're using it
		{
			Private::FHeapBufferStorage& heapBuffer = m_Storage.GetValue<Private::FHeapBufferStorage>();
			heapBuffer.Free();
		}

		m_Storage.ResetToType<Private::FStackBufferStorage>();
	}
}

void FPotentiallySmallBuffer::Free()
{
	m_Storage.Visit(FVariantVisitor
	{
		[](Private::FNoBufferStorage)
		{
			// Nothing to do
		},
		[](Private::FStackBufferStorage& stackBuffer)
		{
			FMemory::ZeroOut(stackBuffer.GetData(), sizeof(stackBuffer));
		},
		[](Private::FHeapBufferStorage& heapBuffer)
		{
			heapBuffer.Free();
			FMemory::ZeroOut(&heapBuffer, sizeof(heapBuffer));
		}
	});

	m_Storage.ResetToType<Private::FNoBufferStorage>();
}

const void* FPotentiallySmallBuffer::GetData() const
{
	return const_cast<FPotentiallySmallBuffer*>(this)->GetData();
}

void* FPotentiallySmallBuffer::GetData()
{
	return m_Storage.Visit<void*>(FVariantVisitor
	{
		[](Private::FNoBufferStorage)
		{
			return nullptr;
		},
		[](Private::FStackBufferStorage& stackBuffer)
		{
			return stackBuffer.GetData();
		},
		[](Private::FHeapBufferStorage& heapBuffer)
		{
			return heapBuffer.GetData();
		}
	});
}

int32 FPotentiallySmallBuffer::GetDataSize() const
{
	return m_Storage.Visit<int32>(FVariantVisitor
	{
		[](Private::FNoBufferStorage)
		{
			return 0;
		},
		[](const Private::FStackBufferStorage&)
		{
			return Private::FStackBufferStorage::Size;
		},
		[](const Private::FHeapBufferStorage& heapBuffer)
		{
			return heapBuffer.GetDataSize();
		}
	});
}

bool FPotentiallySmallBuffer::IsAllocated() const
{
	return m_Storage.GetValueIndex() != StorageType::IndexOfType<Private::FNoBufferStorage>();
}

bool FPotentiallySmallBuffer::IsUsingHeapBuffer() const
{
	return m_Storage.GetValueIndex() == StorageType::IndexOfType<Private::FHeapBufferStorage>();
}

bool FPotentiallySmallBuffer::IsUsingStackBuffer() const
{
	return m_Storage.GetValueIndex() == StorageType::IndexOfType<Private::FStackBufferStorage>();
}