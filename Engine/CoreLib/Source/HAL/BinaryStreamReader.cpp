#include "HAL/BinaryStreamReader.h"
#include "Misc/StringBuilder.h"
#include "Templates/Swap.h"

#define ASSERT_CAN_READ_FROM_STREAM UM_ASSERT(CanReadFromStream(), "Binary reader cannot read from underlying stream")

#define READ_AND_RETURN_POD_FROM_STREAM(Type)           \
	Type value {};                                      \
	m_FileStream->Read(&value, sizeof(Type));           \
	if constexpr (sizeof(Type) >= 2)                    \
		ReverseBytesIfNecessary(m_Endianness, value);   \
	return value

template<typename T>
static inline void ReverseBytesIfNecessary(const EEndianness endianness, T& value)
{
	if (GetSystemEndianness() == endianness)
	{
		return;
	}

	TSpan<uint8> valueBytes { reinterpret_cast<uint8*>(&value), static_cast<int32>(sizeof(T)) };
	for (int32 idx = 0; idx < valueBytes.Num() / 2; ++idx)
	{
		Swap(valueBytes[idx], valueBytes[valueBytes.Num() - idx - 1]);
	}
}

EEndianness FBinaryStreamReader::GetEndianness() const
{
	return m_Endianness;
}

char FBinaryStreamReader::ReadChar()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(char);
}

double FBinaryStreamReader::ReadDouble()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(double);
}

float FBinaryStreamReader::ReadFloat()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(float);
}

int8 FBinaryStreamReader::ReadInt8()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(int8);
}

int16 FBinaryStreamReader::ReadInt16()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(int16);
}

int32 FBinaryStreamReader::ReadInt32()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(int32);
}

int64 FBinaryStreamReader::ReadInt64()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(int64);
}

uint8 FBinaryStreamReader::ReadUint8()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(uint8);
}

uint16 FBinaryStreamReader::ReadUint16()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(uint16);
}

uint32 FBinaryStreamReader::ReadUint32()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(uint32);
}

uint64 FBinaryStreamReader::ReadUint64()
{
	ASSERT_CAN_READ_FROM_STREAM;
	READ_AND_RETURN_POD_FROM_STREAM(uint64);
}

FString FBinaryStreamReader::ReadString(const FString::SizeType numBytes)
{
	ASSERT_CAN_READ_FROM_STREAM;

	UM_ASSERT(numBytes >= 0, "Attempting to read string with negative length");

	if (numBytes == 0)
	{
		return {};
	}

	FStringBuilder builder;
	(void)builder.AddZeroed(numBytes + 1);

	m_FileStream->Read(builder.GetChars(), static_cast<uint64>(numBytes));

	return builder.ReleaseString();
}

void FBinaryStreamReader::SetEndianness(const EEndianness endianness)
{
	m_Endianness = endianness;
}

void FBinaryStreamReader::SetFileStream(TSharedPtr<IFileStream> fileStream)
{
	m_FileStream = MoveTemp(fileStream);
}

bool FBinaryStreamReader::CanReadFromStream()
{
	return m_FileStream.IsValid() && m_FileStream->IsOpen();
}