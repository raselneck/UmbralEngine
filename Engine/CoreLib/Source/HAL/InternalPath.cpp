#include "Containers/Array.h"
#include "HAL/InternalPath.h"
#include "Memory/Memory.h"

namespace Private
{
	FStringView GetTemporaryNullTerminatedStringView(const FStringView value)
	{
		using CharType = FStringView::CharType;

		static constexpr FStringView::SizeType maxNonAllocatingLength = 2048;
		static thread_local CharType nonAllocatingBuffer[maxNonAllocatingLength];
		static thread_local TArray<CharType> allocatingBuffer;

		if (value.IsEmpty() || value.IsNullTerminated())
		{
			return value;
		}

		[[likely]] if (value.Length() < maxNonAllocatingLength)
		{
			FMemory::ZeroOutArray(nonAllocatingBuffer);
			FMemory::Copy(nonAllocatingBuffer, value.GetChars(), value.Length() * sizeof(CharType));

			return FStringView { nonAllocatingBuffer, value.Length() };
		}

		allocatingBuffer.Reset();
		allocatingBuffer.Reserve(value.Length() + 1);
		allocatingBuffer.Append(value.GetChars(), value.Length());
		allocatingBuffer.Add(FStringView::CharTraitsType::NullChar);

		return FStringView { allocatingBuffer.AsSpan() };
	}
}