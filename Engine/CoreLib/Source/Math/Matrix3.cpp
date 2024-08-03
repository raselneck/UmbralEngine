#include "Math/Matrix3.h"
#include "Memory/Memory.h"

const FMatrix3 FMatrix3::Identity
{
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
};

const float* FMatrix3::GetValuePtr() const
{
	return &M11;
}

float* FMatrix3::GetValuePtr()
{
	return &M11;
}

void FMatrix3::SetIdentity()
{
	FMemory::Copy(this, &FMatrix3::Identity, sizeof(FMatrix3));
}

FMatrix3::ArrayType FMatrix3::ToArray() const
{
	static_assert(sizeof(ArrayType) == sizeof(*this), "Matrix3 array type is not valid");

	ArrayType result;
	FMemory::Copy(result.GetData(), GetValuePtr(), sizeof(FMatrix3));
	return result;
}