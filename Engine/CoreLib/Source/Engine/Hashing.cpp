#include "Engine/Hashing.h"
#include "Engine/Assert.h"

namespace Private
{
	// Hashing is using the FNV-1a algorithm http://www.isthe.com/chongo/tech/comp/fnv/
	constexpr uint64 HashInit = 14695981039346656037ULL;
	constexpr uint64 HashPrime = 0x100000001b3ULL;

	uint64 HashBytesWithInitialHash(const TSpan<const uint8> bytes, const uint64 initialHash)
	{
		uint64 hash = initialHash;
		for (const uint8 byte : bytes)
		{
			hash ^= byte;
			hash *= HashPrime;
		}

		UM_ASSERT(hash != INVALID_HASH, "Somehow hashed a byte array into the invalid hash value");

		return hash;
	}

	uint64 HashBytes(TSpan<const uint8> bytes)
	{
		return HashBytesWithInitialHash(bytes, HashInit);
	}

	uint64 HashCombine(uint64 firstHash, uint64 secondHash)
	{
		return HashBytesWithInitialHash(CastToBytes(secondHash), firstHash);
	}
}