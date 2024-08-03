#include "Math/Matrix4.h"

#include "Containers/InternalString.h"
#include "Math/Math.h"
#include "Math/Rotator.h"
#include "Math/Quaternion.h"
#include "Memory/Memory.h"
#include "Misc/StringBuilder.h"
#include <cmath>

// clang-format off

const FMatrix4 FMatrix4::Identity
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

// clang-format on

void FMatrix4::CreateBillboard(const FVector3& objectPosition,
                               const FVector3& cameraPosition,
                               const FVector3& cameraUp,
                               const TOptional<FVector3>& cameraForward,
                               FMatrix4& result)
{
	result.SetIdentity();

	FVector3 cameraToObject = objectPosition - cameraPosition;

	// NOTE: We _could_ use IsNearlyZero, but that's a little too strict for this
	const float objectToCameraLengthSquared = cameraToObject.LengthSquared();
	if (objectToCameraLengthSquared < 0.0001f)
	{
		if (cameraForward.HasValue())
		{
			cameraToObject = -cameraForward.GetValue();
		}
		else
		{
			cameraToObject = FVector3::Forward;
		}
	}
	else
	{
		// TODO Is this correct?
		cameraToObject /= objectToCameraLengthSquared;
	}

	FVector3 objectRight = FVector3::Cross(cameraUp, cameraToObject);
	objectRight.Normalize();

	FVector3 objectUp = FVector3::Cross(cameraToObject, objectRight);

	result.M11 = objectRight.X;
	result.M12 = objectRight.Y;
	result.M13 = objectRight.Z;
	result.M14 = 0.0f;
	result.M21 = objectUp.X;
	result.M22 = objectUp.Y;
	result.M23 = objectUp.Z;
	result.M24 = 0.0f;
	result.M31 = cameraToObject.X;
	result.M32 = cameraToObject.Y;
	result.M33 = cameraToObject.Z;
	result.M34 = 0.0f;
	result.M41 = objectPosition.X;
	result.M42 = objectPosition.Y;
	result.M43 = objectPosition.Z;
	result.M44 = 1.0f;
}

void FMatrix4::CreateConstrainedBillboard(const FVector3& objectPosition,
                                          const FVector3& cameraPosition,
                                          const FVector3& rotateAxis,
                                          const TOptional<FVector3>& cameraForward,
                                          const TOptional<FVector3>& objectForwardVector,
                                          FMatrix4& result)
{
	result.SetIdentity();

	FVector3 objectForward;
	FVector3 objectRight;

	FVector3 cameraToObject = objectPosition - cameraPosition;

	float cameraToObjectLengthSquared = cameraToObject.LengthSquared();
	if (cameraToObjectLengthSquared < 0.0001f)
	{
		if (cameraForward.HasValue())
		{
			cameraToObject = -cameraForward.GetValue();
		}
		else
		{
			cameraToObject = FVector3::Forward;
		}
	}
	else
	{
		cameraToObject /= cameraToObjectLengthSquared;
	}

	static constexpr float ConstrainedBillboardThreshold = 0.998f;

	FVector3 objectUp = rotateAxis;
	float dotProduct = FVector3::Dot(rotateAxis, cameraToObject);
	if (FMath::Abs(dotProduct) > ConstrainedBillboardThreshold)
	{
		bool needDefaultForward = true;
		if (objectForwardVector.HasValue())
		{
			objectForward = objectForwardVector.GetValue();
			dotProduct = FVector3::Dot(rotateAxis, objectForward);
			needDefaultForward = (FMath::Abs(dotProduct) > ConstrainedBillboardThreshold);
		}

		if (needDefaultForward)
		{
			dotProduct = FVector3::Dot(rotateAxis, FVector3::Forward);
			objectForward = (FMath::Abs(dotProduct) > ConstrainedBillboardThreshold) ? FVector3::Right : FVector3::Forward;
		}

		objectRight = FVector3::Cross(rotateAxis, objectForward);
		objectRight.Normalize();
		objectForward = FVector3::Cross(objectRight, rotateAxis);
		objectForward.Normalize();
	}
	else
	{
		objectRight = FVector3::Cross(rotateAxis, cameraToObject);
		objectRight.Normalize();

		objectForward = FVector3::Cross(objectRight, objectUp);
		objectForward.Normalize();
	}

	result.M11 = objectRight.X;
	result.M12 = objectRight.Y;
	result.M13 = objectRight.Z;
	result.M21 = objectUp.X;
	result.M22 = objectUp.Y;
	result.M23 = objectUp.Z;
	result.M31 = objectForward.X;
	result.M32 = objectForward.Y;
	result.M33 = objectForward.Z;
	result.M41 = objectPosition.X;
	result.M42 = objectPosition.Y;
	result.M43 = objectPosition.Z;
}

void FMatrix4::CreateFromAxisAngle(const FVector3& axis, const float angle, FMatrix4& result)
{
	result.SetIdentity();

	const FVector3 normalizedAxis = axis.GetNormalized();
	const float x = normalizedAxis.X;
	const float y = normalizedAxis.Y;
	const float z = normalizedAxis.Z;
	const float sinAngle = FMath::Sin(angle);
	const float cosAngle = FMath::Cos(angle);
	const float xx = x * x;
	const float yy = y * y;
	const float zz = z * z;
	const float xy = x * y;
	const float xz = x * z;
	const float yz = y * z;
	result.M11 = xx + (cosAngle * (1.0f - xx));
	result.M12 = (xy - (cosAngle * xy)) + (sinAngle * z);
	result.M13 = (xz - (cosAngle * xz)) - (sinAngle * y);
	result.M21 = (xy - (cosAngle * xy)) - (sinAngle * z);
	result.M22 = yy + (cosAngle * (1.0f - yy));
	result.M23 = (yz - (cosAngle * yz)) + (sinAngle * x);
	result.M31 = (xz - (cosAngle * xz)) + (sinAngle * y);
	result.M32 = (yz - (cosAngle * yz)) - (sinAngle * x);
	result.M33 = zz + (cosAngle * (1.0f - zz));
}

void FMatrix4::CreateFromRotator(const FRotator& value, FMatrix4& result)
{
	const FQuaternion rotation = value.ToQuaternion();
	CreateFromQuaternion(rotation, result);
}

void FMatrix4::CreateFromQuaternion(const FQuaternion& value, FMatrix4& result)
{
	result.SetIdentity();

	const float xx = value.X * value.X;
	const float yy = value.Y * value.Y;
	const float zz = value.Z * value.Z;
	const float xy = value.X * value.Y;
	const float zw = value.Z * value.W;
	const float zx = value.Z * value.X;
	const float yw = value.Y * value.W;
	const float yz = value.Y * value.Z;
	const float xw = value.X * value.W;
	result.M11 = 1.0f - (2.0f * (yy + zz));
	result.M12 = 2.0f * (xy + zw);
	result.M13 = 2.0f * (zx - yw);
	result.M21 = 2.0f * (xy - zw);
	result.M22 = 1.0f - (2.0f * (zz + xx));
	result.M23 = 2.0f * (yz + xw);
	result.M31 = 2.0f * (zx + yw);
	result.M32 = 2.0f * (yz - xw);
	result.M33 = 1.0f - (2.0f * (yy + xx));
}

void FMatrix4::CreateFromYawPitchRoll(const float yaw, const float pitch, const float roll, FMatrix4& result)
{
	const FQuaternion rotation = FQuaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	CreateFromQuaternion(rotation, result);
}

void FMatrix4::CreateLookAt(const FVector3& cameraPosition,
                            const FVector3& cameraTarget,
                            const FVector3& cameraUp,
                            FMatrix4& result)
{
	result.SetIdentity();

	const FVector3 viewForward = FVector3::Normalize(cameraPosition - cameraTarget);
	const FVector3 viewRight = FVector3::Normalize(FVector3::Cross(cameraUp, viewForward));
	const FVector3 viewUp = FVector3::Cross(viewForward, viewRight);
	result.M11 = viewRight.X;
	result.M21 = viewRight.Y;
	result.M31 = viewRight.Z;
	result.M12 = viewUp.X;
	result.M22 = viewUp.Y;
	result.M32 = viewUp.Z;
	result.M13 = viewForward.X;
	result.M23 = viewForward.Y;
	result.M33 = viewForward.Z;
	result.M41 = -FVector3::Dot(viewRight, cameraPosition);
	result.M42 = -FVector3::Dot(viewUp, cameraPosition);
	result.M43 = -FVector3::Dot(viewForward, cameraPosition);
}

void FMatrix4::CreateOrthographic(const float width, const float height,
                                  const float zNearPlane, const float zFarPlane,
                                  FMatrix4& result)
{
	result.SetIdentity();

	result.M11 = 2.0f / width;
	result.M22 = 2.0f / height;
	result.M33 = 1.0f / (zNearPlane - zFarPlane);
	result.M43 = zNearPlane / (zNearPlane - zFarPlane);
}

void FMatrix4::CreateOrthographicOffCenter(const float left, const float right,
                                           const float bottom, const float top,
                                           const float zNearPlane, const float zFarPlane,
                                           FMatrix4& result)
{
	result.SetIdentity();

	result.M11 = 2.0f / (right - left);
	result.M22 = 2.0f / (top - bottom);
	result.M33 = 1.0f / (zNearPlane - zFarPlane);
	result.M41 = (left + right) / (left - right);
	result.M42 = (top + bottom) / (bottom - top);
	result.M43 = zNearPlane / (zNearPlane - zFarPlane);
}

void FMatrix4::CreatePerspectiveFieldOfView(const float fieldOfView, const float aspectRatio,
                                            const float nearPlaneDistance, const float farPlaneDistance,
                                            FMatrix4& result)
{
	result.SetIdentity();

	const float yScale = 1.0f / ::tanf(fieldOfView * 0.5f);
	const float xScale = yScale / aspectRatio;
	const float negFarRange = farPlaneDistance / (nearPlaneDistance - farPlaneDistance);

	result.M11 = xScale;
	result.M22 = yScale;
	result.M33 = negFarRange;
	result.M34 = -1.0f;
	result.M43 = nearPlaneDistance * negFarRange;
}

void FMatrix4::CreateRotationX(const float angle, FMatrix4& result)
{
	const float radians = FMath::ToRadians(angle);
	const float cosAngle = cosf(radians);
	const float sinAngle = sinf(radians);

	result.SetIdentity();
	result.M22 = cosAngle;
	result.M23 = sinAngle;
	result.M32 = -sinAngle;
	result.M33 = cosAngle;
}

void FMatrix4::CreateRotationY(const float angle, FMatrix4& result)
{
	const float radians = FMath::ToRadians(angle);
	const float cosAngle = cosf(radians);
	const float sinAngle = sinf(radians);

	result.SetIdentity();
	result.M11 = cosAngle;
	result.M13 = -sinAngle;
	result.M31 = sinAngle;
	result.M33 = cosAngle;
}

void FMatrix4::CreateRotationZ(const float angle, FMatrix4& result)
{
	const float radians = FMath::ToRadians(angle);
	const float cosAngle = cosf(radians);
	const float sinAngle = sinf(radians);

	result.SetIdentity();
	result.M11 = cosAngle;
	result.M12 = sinAngle;
	result.M21 = -sinAngle;
	result.M22 = cosAngle;
}

const float* FMatrix4::GetValuePtr() const
{
	return &M11;
}

float* FMatrix4::GetValuePtr()
{
	return &M11;
}

void FMatrix4::Multiply(const FMatrix4& first, const FMatrix4& second, FMatrix4& result)
{
	UM_ASSERT(&first  != &result, "Cannot provide references to the same `first' and `result' parameters");
	UM_ASSERT(&second != &result, "Cannot provide references to the same `second' and `result' parameters");

	result.M11 = (first.M11 * second.M11) + (first.M12 * second.M21) + (first.M13 * second.M31) + (first.M14 * second.M41);
	result.M12 = (first.M11 * second.M12) + (first.M12 * second.M22) + (first.M13 * second.M32) + (first.M14 * second.M42);
	result.M13 = (first.M11 * second.M13) + (first.M12 * second.M23) + (first.M13 * second.M33) + (first.M14 * second.M43);
	result.M14 = (first.M11 * second.M14) + (first.M12 * second.M24) + (first.M13 * second.M34) + (first.M14 * second.M44);
	result.M21 = (first.M21 * second.M11) + (first.M22 * second.M21) + (first.M23 * second.M31) + (first.M24 * second.M41);
	result.M22 = (first.M21 * second.M12) + (first.M22 * second.M22) + (first.M23 * second.M32) + (first.M24 * second.M42);
	result.M23 = (first.M21 * second.M13) + (first.M22 * second.M23) + (first.M23 * second.M33) + (first.M24 * second.M43);
	result.M24 = (first.M21 * second.M14) + (first.M22 * second.M24) + (first.M23 * second.M34) + (first.M24 * second.M44);
	result.M31 = (first.M31 * second.M11) + (first.M32 * second.M21) + (first.M33 * second.M31) + (first.M34 * second.M41);
	result.M32 = (first.M31 * second.M12) + (first.M32 * second.M22) + (first.M33 * second.M32) + (first.M34 * second.M42);
	result.M33 = (first.M31 * second.M13) + (first.M32 * second.M23) + (first.M33 * second.M33) + (first.M34 * second.M43);
	result.M34 = (first.M31 * second.M14) + (first.M32 * second.M24) + (first.M33 * second.M34) + (first.M34 * second.M44);
	result.M41 = (first.M41 * second.M11) + (first.M42 * second.M21) + (first.M43 * second.M31) + (first.M44 * second.M41);
	result.M42 = (first.M41 * second.M12) + (first.M42 * second.M22) + (first.M43 * second.M32) + (first.M44 * second.M42);
	result.M43 = (first.M41 * second.M13) + (first.M42 * second.M23) + (first.M43 * second.M33) + (first.M44 * second.M43);
	result.M44 = (first.M41 * second.M14) + (first.M42 * second.M24) + (first.M43 * second.M34) + (first.M44 * second.M44);
}

void FMatrix4::SetIdentity()
{
	FMemory::Copy(this, &FMatrix4::Identity, sizeof(FMatrix4));
}

void FMatrix4::ToArray(ArrayType& result) const
{
	static_assert(sizeof(ArrayType) == sizeof(FMatrix4), "Matrix4 array type is not valid");

	FMemory::Copy(result.m_Data, GetValuePtr(), sizeof(FMatrix4));
}

FMatrix3 FMatrix4::ToNormalMatrix() const
{
	return FMatrix3
	{
		M11, M12, M13,
		M21, M22, M23,
		M31, M32, M33
	};
}

void TFormatter<FMatrix4>::BuildString(const FMatrix4& value, FStringBuilder& builder)
{
	FToCharsArgs args;
	args.NumDecimals = 2;

	builder.Append("(("_sv);
	{
		// First row
		Private::AppendCharsForFloat(builder, value.M11, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M12, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M13, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M14, args);
		builder.Append("), ("_sv);

		// Second row
		Private::AppendCharsForFloat(builder, value.M21, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M22, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M23, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M24, args);
		builder.Append("), ("_sv);

		// Third row
		Private::AppendCharsForFloat(builder, value.M31, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M32, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M33, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M34, args);
		builder.Append("), ("_sv);

		// Fourth row
		Private::AppendCharsForFloat(builder, value.M41, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M42, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M43, args);
		builder.Append(", "_sv);
		Private::AppendCharsForFloat(builder, value.M44, args);
	}
	builder.Append("))"_sv);
}

bool TFormatter<FMatrix4>::Parse(FStringView formatString)
{
	return formatString.IsEmpty();
}