#include "Math/Vector4.h"
#include "Math/Math.h"
#include "Math/Vector3.h"

const FVector4 FVector4::One   { 1.0f, 1.0f, 1.0f, 1.0f };
const FVector4 FVector4::UnitX { 1.0f, 0.0f, 0.0f, 0.0f };
const FVector4 FVector4::UnitY { 0.0f, 1.0f, 0.0f, 0.0f };
const FVector4 FVector4::UnitZ { 0.0f, 0.0f, 1.0f, 0.0f };
const FVector4 FVector4::UnitW { 0.0f, 0.0f, 0.0f, 1.0f };
const FVector4 FVector4::Zero  { 0.0f, 0.0f, 0.0f, 0.0f };

FVector4 FVector4::Barycentric(const FVector4& value1, const FVector4& value2,
                               const FVector4& value3, const float amount1, const float amount2)
{
	return FVector4
	{
		FMath::Barycentric(value1.X, value2.X, value3.X, amount1, amount2),
		FMath::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2),
		FMath::Barycentric(value1.Z, value2.Z, value3.Z, amount1, amount2),
		FMath::Barycentric(value1.W, value2.W, value3.W, amount1, amount2)
	};
}

FVector4 FVector4::CatmullRom(const FVector4& value1, const FVector4& value2,
                              const FVector4& value3, const FVector4& value4, const float amount)
{
	return FVector4
	{
		FMath::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount),
		FMath::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount),
		FMath::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount),
		FMath::CatmullRom(value1.W, value2.W, value3.W, value4.W, amount)
	};
}

const float* FVector4::GetValuePtr() const
{
	static_assert(sizeof(FVector4) == sizeof(float) * 4, "Size of FVector4 is invalid.");
	return reinterpret_cast<const float*>(this);
}

float* FVector4::GetValuePtr()
{
	return reinterpret_cast<float*>(this);
}

FVector4 FVector4::Hermite(const FVector4& value1, const FVector4& tangent1,
                           const FVector4& value2, const FVector4& tangent2, const float amount)
{
	return FVector4
	{
		FMath::Hermite(value1.X, tangent1.X, value2.X, tangent2.X, amount),
		FMath::Hermite(value1.Y, tangent1.Y, value2.Y, tangent2.Y, amount),
		FMath::Hermite(value1.Z, tangent1.Z, value2.Z, tangent2.Z, amount),
		FMath::Hermite(value1.W, tangent1.W, value2.W, tangent2.W, amount)
	};
}

float FVector4::Length() const
{
	return FMath::Sqrt(LengthSquared());
}

void FVector4::Normalize()
{
	const float oneOverLength = FMath::SafeDivide(1.0f, Length());
	X *= oneOverLength;
	Y *= oneOverLength;
	Z *= oneOverLength;
	W *= oneOverLength;
}

FVector4 FVector4::SmoothStep(const FVector4& value1, const FVector4& value2, const float amount)
{
	return FVector4
	{
		FMath::SmoothStep(value1.X, value2.X, amount),
		FMath::SmoothStep(value1.Y, value2.Y, amount),
		FMath::SmoothStep(value1.Z, value2.Z, amount),
		FMath::SmoothStep(value1.W, value2.W, amount)
	};
}