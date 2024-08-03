#include "Math/Rotator.h"
#include "Math/Math.h"
#include "Math/Quaternion.h"
#include <cmath>

#ifndef WITH_ROTATOR_GIMBAL_LOCK_MITIGATION
#	define WITH_ROTATOR_GIMBAL_LOCK_MITIGATION 1
#endif

const FRotator FRotator::Identity;

FRotator FRotator::FromQuaternion(const FQuaternion& value)
{
	const FQuaternion normalizedQuat = value.GetNormalized();
	const float q0 = normalizedQuat.X;
	const float q1 = normalizedQuat.Y;
	const float q2 = normalizedQuat.Z;
	const float q3 = normalizedQuat.W;

	// Adapted from
	// https://www.dreamincode.net/forums/topic/349917-convert-from-quaternion-to-euler-angles-vector3/page__view__findpost__p__2038656?s=660ed3e2df53e03d803cb47ba0d2de35
	// Which was adapted from the 2nd edition of "3D Math Primer for Graphics and Game Development"

	// Pseudo-rotation matrix
	const float M11 = 1.0f - (2.0f * (q1 * q1 + q2 * q2));
	const float M12 = 2.0f * (q0 * q1 + q2 * q3);
	const float M13 = 2.0f * (q2 * q0 - q1 * q3);
	const float M22 = 1.0f - (2.0f * (q2 * q2 + q0 * q0));
	const float M31 = 2.0f * (q2 * q0 + q1 * q3);
	const float M32 = 2.0f * (q1 * q2 - q0 * q3);
	const float M33 = 1.0f - (2.f * (q1 * q1 + q0 * q0));

	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;

	// Test for poles
	const float singularity = -M32;
	if (singularity <= -1.0f)  // south pole
	{
		pitch = -FMath::HalfPi;
	}
	else if (singularity >= 1.0f)  // north pole
	{
		pitch = FMath::HalfPi;
	}
	else
	{
		pitch = ::asinf(singularity);
	}

#if WITH_ROTATOR_GIMBAL_LOCK_MITIGATION
	// Test for Gimbal lock
	constexpr float singularityMax = 1.0f - FMath::KindaSmallNumber;
	if (singularity > singularityMax)
	{
		yaw = 0.0f;
		roll = ::atan2f(M13, M11);
	}
	else
	{
		yaw = ::atan2f(M31, M33);
		roll = ::atan2f(M12, M22);
	}
#else
	yaw = ::atan2f(M31, M33);
	roll = ::atan2f(M12, M22);
#endif

	FRotator result;
	result.Yaw = FMath::WrapAngleDegrees(FMath::ToDegrees(yaw));
	result.Pitch = FMath::WrapAngleDegrees(FMath::ToDegrees(pitch));
	result.Roll = FMath::WrapAngleDegrees(FMath::ToDegrees(roll));
	return result;
}

const float* FRotator::GetValuePtr() const
{
	return &Yaw;
}

float* FRotator::GetValuePtr()
{
	return &Yaw;
}

FQuaternion FRotator::ToQuaternion() const
{
	return FQuaternion::CreateFromYawPitchRoll(
		FMath::ToRadians(Yaw),
		FMath::ToRadians(Pitch),
		FMath::ToRadians(Roll)
	);
}