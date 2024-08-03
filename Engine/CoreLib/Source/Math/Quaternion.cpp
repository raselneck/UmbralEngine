#include "Math/Quaternion.h"
#include "Math/Math.h"
#include <cmath>

const FQuaternion FQuaternion::Identity { 0.0f, 0.0f, 0.0f, 1.0f };

FQuaternion FQuaternion::Concatenate(const FQuaternion& value1, const FQuaternion& value2)
{
	const float x1 = value1.X;
	const float y1 = value1.Y;
	const float z1 = value1.Z;
	const float w1 = value1.W;

	const float x2 = value2.X;
	const float y2 = value2.Y;
	const float z2 = value2.Z;
	const float w2 = value2.W;

	return FQuaternion
	{
		x2 * w1 + x1 * w2 + y2 * z1 - z2 * y1,
		y2 * w1 + y1 * w2 + z2 * x1 - x2 * z1,
		z2 * w1 + z1 * w2 + x2 * y1 - y2 * x1,
		w2 * w1 - x2 * x1 + y2 * y1 + z2 * z1
	};
}

FQuaternion FQuaternion::CreateFromAxisAngle(const FVector3& axis, const float angle)
{
	const float halfAngle = angle * 0.5f;
	const float sinHalfAngle = ::sinf(halfAngle);
	const float cosHalfAngle = ::cosf(halfAngle);
	const FVector3 normalizedAxis = axis.GetNormalized();

	return FQuaternion
	{
		normalizedAxis.X * sinHalfAngle,
		normalizedAxis.Y * sinHalfAngle,
		normalizedAxis.Z * sinHalfAngle,
		cosHalfAngle
	};
}

FQuaternion FQuaternion::CreateFromYawPitchRoll(const float yaw, const float pitch, const float roll)
{
	const float halfRoll = roll * 0.5f;
	const float halfPitch = pitch * 0.5f;
	const float halfYaw = yaw * 0.5f;

	const float sr = ::sinf(halfRoll);
	const float cr = ::cosf(halfRoll);
	const float sp = ::sinf(halfPitch);
	const float cp = ::cosf(halfPitch);
	const float sy = ::sinf(halfYaw);
	const float cy = ::cosf(halfYaw);

	return FQuaternion
	{
		(cy * sp * cr) + (sy * cp * sr),
		(sy * cp * cr) - (cy * sp * sr),
		(cy * cp * sr) - (sy * sp * cr),
		(cy * cp * cr) + (sy * sp * sr)
	};
}

FQuaternion FQuaternion::Divide(const FQuaternion& value1, const FQuaternion& value2)
{
	const float x1 = value1.X;
	const float y1 = value1.Y;
	const float z1 = value1.Z;
	const float w1 = value1.W;

	const float divisorLengthSquared = value2.LengthSquared();
	const float divisorMultiplier = FMath::SafeDivide(1.0f, divisorLengthSquared);
	const FQuaternion divisor = -value2 * divisorMultiplier;

	const float x2 = divisor.X;
	const float y2 = divisor.Y;
	const float z2 = divisor.Z;
	const float w2 = divisor.W;

	return FQuaternion
	{
		x1 * w2 + x2 * w1 + y1 * z2 - z1 * y2,
		y1 * w2 + y2 * w1 + z1 * x2 - x1 * z2,
		z1 * w2 + z2 * w1 + x1 * y2 - y1 * x2,
		w1 * w2 - x1 * x2 + y1 * y2 + z1 * z2
	};
}

float FQuaternion::Dot(const FQuaternion& value1, const FQuaternion& value2)
{
	return value1.X * value2.X + value1.Y * value2.Y + value1.Z * value2.Z + value1.W * value2.W;
}

const float* FQuaternion::GetValuePtr() const
{
	return &X;
}

float* FQuaternion::GetValuePtr()
{
	return &X;
}

FQuaternion FQuaternion::Inverse(const FQuaternion& value)
{
	// TODO Is this logic correct?

	const float lengthSquared = value.LengthSquared();
	const float inverseLength = FMath::SafeDivide(1.0f, lengthSquared);
	return FQuaternion
	{
		-value.X * inverseLength,
		-value.Y * inverseLength,
		-value.Z * inverseLength,
		 value.W * inverseLength
	};
}

float FQuaternion::Length() const
{
	return FMath::Sqrt(LengthSquared());
}

FQuaternion FQuaternion::Lerp(const FQuaternion& value1, const FQuaternion& value2, const float amount)
{
	const float invAmount = 1.0f - amount;
	const float dotProduct = value1.X * value2.X + value1.Y * value2.Y + value1.Z * value2.Z + value1.W * value2.W;

	FQuaternion result;
	if (dotProduct >= 0.0f)
	{
		result.X = (invAmount * value1.X) + (amount * value2.X);
		result.Y = (invAmount * value1.Y) + (amount * value2.Y);
		result.Z = (invAmount * value1.Z) + (amount * value2.Z);
		result.W = (invAmount * value1.W) + (amount * value2.W);
	}
	else
	{
		result.X = (invAmount * value1.X) - (amount * value2.X);
		result.Y = (invAmount * value1.Y) - (amount * value2.Y);
		result.Z = (invAmount * value1.Z) - (amount * value2.Z);
		result.W = (invAmount * value1.W) - (amount * value2.W);
	}

	result.Normalize();

	return result;
}

FQuaternion FQuaternion::Multiply(const FQuaternion& value1, const FQuaternion& value2)
{
	const float x1 = value1.X;
	const float y1 = value1.Y;
	const float z1 = value1.Z;
	const float w1 = value1.W;
	const float x2 = value2.X;
	const float y2 = value2.Y;
	const float z2 = value2.Z;
	const float w2 = value2.W;

	return FQuaternion
	{
		x1 * w2 + x2 * w1 + y1 * z2 - z1 * y2,
		y1 * w2 + y2 * w1 + z1 * x2 - x1 * z2,
		z1 * w2 + z2 * w1 + x1 * y2 - y1 * x2,
		w1 * w2 - x1 * x2 + y1 * y2 + z1 * z2
	};
}

void FQuaternion::Normalize()
{
	if (FMath::IsNearlyEqual(1.0f, LengthSquared()))
	{
		return;
	}

	const float inverseLength = FMath::SafeDivide(1.0f, Length());
	X *= inverseLength;
	Y *= inverseLength;
	Z *= inverseLength;
	W *= inverseLength;
}

FQuaternion FQuaternion::Slerp(const FQuaternion& value1, const FQuaternion& value2, const float amount)
{
	// NOTE: The MonoGame source code here was very obviously taken from decompiled XNA. If it somehow wasn't, then
	//       whoever originally wrote it is God awful at naming things. I've tried to rename vars here based off of
	//       what they seem like they would be from here: https://en.wikipedia.org/wiki/Slerp#Source_code

	float s0 = 0.0f;
	float s1 = 0.0f;
	float dotProduct = FQuaternion::Dot(value1, value2);
	const bool shouldNegate = dotProduct < 0.0f;
	if (shouldNegate)
	{
		dotProduct = -dotProduct;
	}

	constexpr float slerpThreshold = 1.0f - FMath::KindaSmallNumber;
	if (dotProduct > slerpThreshold)
	{
		s0 = 1.0f - amount;
		s1 = shouldNegate ? -amount : amount;
	}
	else
	{
		const float theta0 = ::acosf(dotProduct);
		const float invSinTheta0 = FMath::SafeDivide(1.0f, ::sinf(theta0));
		s0 = ::sinf((1.0f - amount) * theta0) * invSinTheta0;
		s1 = ::sinf(amount * theta0) * invSinTheta0;
		if (shouldNegate)
		{
			s1 = -s1;
		}
	}

	return FQuaternion
	{
		s0 * value1.X + s1 * value2.X,
		s0 * value1.Y + s1 * value2.Y,
		s0 * value1.Z + s1 * value2.Z,
		s0 * value1.W + s1 * value2.W
	};
}

FQuaternion& FQuaternion::operator*=(const FQuaternion& value)
{
	*this = FQuaternion::Multiply(*this, value);
	return *this;
}

FQuaternion& FQuaternion::operator/=(const FQuaternion& value)
{
	*this = FQuaternion::Divide(*this, value);
	return *this;
}