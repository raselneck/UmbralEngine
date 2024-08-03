#include "Math/Vector2.h"
#include "Math/Math.h"

const FVector2 FVector2::One { 1.0f, 1.0f };
const FVector2 FVector2::UnitX { 1.0f, 0.0f };
const FVector2 FVector2::UnitY { 0.0f, 1.0f };
const FVector2 FVector2::Zero { 0.0f, 0.0f };

FVector2 FVector2::Barycentric(const FVector2& value1,
                               const FVector2& value2,
                               const FVector2& value3,
                               const float amount1,
                               const float amount2)
{
	FVector2 result;
	result.X = FMath::Barycentric(value1.X, value2.X, value3.X, amount1, amount2);
	result.Y = FMath::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2);
	return result;
}

FVector2 FVector2::CatmullRom(const FVector2& value1,
                              const FVector2& value2,
                              const FVector2& value3,
                              const FVector2& value4,
                              const float amount)
{
	FVector2 result;
	result.X = FMath::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount);
	result.Y = FMath::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount);
	return result;
}

const float* FVector2::GetValuePtr() const
{
	return &X;
}

float* FVector2::GetValuePtr()
{
	return &X;
}

FVector2 FVector2::Hermite(const FVector2& value1,
                           const FVector2& tangent1,
                           const FVector2& value2,
                           const FVector2& tangent2,
                           const float amount)
{
	FVector2 OutResult;
	OutResult.X = FMath::Hermite(value1.X, tangent1.X, value2.X, tangent2.X, amount);
	OutResult.Y = FMath::Hermite(value1.Y, tangent1.Y, value2.Y, tangent2.Y, amount);
	return OutResult;
}

float FVector2::Length() const
{
	return FMath::Sqrt(LengthSquared());
}

void FVector2::Normalize()
{
	if (FMath::IsNearlyEqual(LengthSquared(), 1.0f, FMath::KindaSmallNumber))
	{
		return;
	}

	const float inverseLength = FMath::SafeDivide(1.0f, Length());
	X *= inverseLength;
	Y *= inverseLength;
}

FVector2 FVector2::SmoothStep(const FVector2& from, const FVector2& to, const float amount)
{
	FVector2 result;
	result.X = FMath::SmoothStep(from.X, to.X, amount);
	result.Y = FMath::SmoothStep(from.Y, to.Y, amount);
	return result;
}