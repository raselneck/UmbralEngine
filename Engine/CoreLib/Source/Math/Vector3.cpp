#include "Math/Vector3.h"
#include "Math/Math.h"
#include "Math/Matrix4.h"
#include "Containers/InternalString.h"

const FVector3 FVector3::Backward {  0.0f,  0.0f,  1.0f };
const FVector3 FVector3::Down     {  0.0f, -1.0f,  0.0f };
const FVector3 FVector3::Forward  {  0.0f,  0.0f, -1.0f };
const FVector3 FVector3::Left     { -1.0f,  0.0f,  0.0f };
const FVector3 FVector3::One      {  1.0f,  1.0f,  1.0f };
const FVector3 FVector3::Right    {  1.0f,  0.0f,  0.0f };
const FVector3 FVector3::UnitX    {  1.0f,  0.0f,  0.0f };
const FVector3 FVector3::UnitY    {  0.0f,  1.0f,  0.0f };
const FVector3 FVector3::UnitZ    {  0.0f,  0.0f,  1.0f };
const FVector3 FVector3::Up       {  0.0f,  1.0f,  0.0f };
const FVector3 FVector3::Zero     {  0.0f,  0.0f,  0.0f };

FVector3 FVector3::Barycentric(const FVector3& value1, const FVector3& value2, const FVector3& value3,
                               const float amount1, const float amount2)
{
	return FVector3
	{
		FMath::Barycentric(value1.X, value2.X, value3.X, amount1, amount2),
		FMath::Barycentric(value1.Y, value2.Y, value3.Y, amount1, amount2),
		FMath::Barycentric(value1.Z, value2.Z, value3.Z, amount1, amount2)
	};
}

FVector3 FVector3::CatmullRom(const FVector3& value1, const FVector3& value2,
                              const FVector3& value3, const FVector3& value4,
                              const float amount)
{
	return FVector3
	{
		FMath::CatmullRom(value1.X, value2.X, value3.X, value4.X, amount),
		FMath::CatmullRom(value1.Y, value2.Y, value3.Y, value4.Y, amount),
		FMath::CatmullRom(value1.Z, value2.Z, value3.Z, value4.Z, amount)
	};
}

const float* FVector3::GetValuePtr() const
{
	return &X;
}

float* FVector3::GetValuePtr()
{
	return &X;
}

FVector3 FVector3::Hermite(const FVector3& value1, const FVector3& tangent1,
                           const FVector3& value2, const FVector3& tangent2,
                           const float amount)
{
	return FVector3
	{
		FMath::Hermite(value1.X, tangent1.X, value2.X, tangent2.X, amount),
		FMath::Hermite(value1.Y, tangent1.Y, value2.Y, tangent2.Y, amount),
		FMath::Hermite(value1.Z, tangent1.Z, value2.Z, tangent2.Z, amount)
	};
}

float FVector3::Length() const
{
	return FMath::Sqrt(LengthSquared());
}

void FVector3::Normalize()
{
	if (FMath::IsNearlyEqual(LengthSquared(), 1.0f, FMath::KindaSmallNumber))
	{
		return;
	}

	const float inverseLength = FMath::SafeDivide(1.0f, Length());
	X *= inverseLength;
	Y *= inverseLength;
	Z *= inverseLength;
}

FVector3 FVector3::Reflect(const FVector3& vector, const FVector3& normal)
{
	// I is the original array
	// N is the normal of the incident plane
	// R = I - (2 * N * Dot(I, N))

	const float dotProduct = Dot(vector, normal);
	return FVector3
	{
		vector.X - (2.0f * normal.X) * dotProduct,
		vector.Y - (2.0f * normal.Y) * dotProduct,
		vector.Z - (2.0f * normal.Z) * dotProduct
	};
}

FVector3 FVector3::SmoothStep(const FVector3& value1, const FVector3& value2, const float amount)
{
	return FVector3
	{
		FMath::SmoothStep(value1.X, value2.X, amount),
		FMath::SmoothStep(value1.Y, value2.Y, amount),
		FMath::SmoothStep(value1.Z, value2.Z, amount)
	};
}

FVector3 FVector3::Transform(const FVector3& value, const FMatrix4& transform)
{
	return FVector3
	{
		(value.X * transform.M11) + (value.Y * transform.M21) + (value.Z * transform.M31) + transform.M41,
		(value.X * transform.M12) + (value.Y * transform.M22) + (value.Z * transform.M32) + transform.M42,
		(value.X * transform.M13) + (value.Y * transform.M23) + (value.Z * transform.M33) + transform.M43
	};
}

FVector3 FVector3::TransformNormal(const FVector3& normal, const FMatrix4& transform)
{
	return FVector3
	{
		(normal.X * transform.M11) + (normal.Y * transform.M21) + (normal.Z * transform.M31),
		(normal.X * transform.M12) + (normal.Y * transform.M22) + (normal.Z * transform.M32),
		(normal.X * transform.M13) + (normal.Y * transform.M23) + (normal.Z * transform.M33)
	};
}

void TFormatter<FVector3>::BuildString(const FVector3& value, FStringBuilder& builder)
{
	FToCharsArgs args;
	args.NumDecimals = 3;

	builder.Append("("_sv);
	Private::AppendCharsForFloat(builder, value.X, args);
	builder.Append(", "_sv);
	Private::AppendCharsForFloat(builder, value.Y, args);
	builder.Append(", "_sv);
	Private::AppendCharsForFloat(builder, value.Z, args);
	builder.Append(")"_sv);
}

bool TFormatter<FVector3>::Parse(const FStringView formatString)
{
	// TODO Copy float formatting for number of decimals
	return formatString.IsEmpty();
}