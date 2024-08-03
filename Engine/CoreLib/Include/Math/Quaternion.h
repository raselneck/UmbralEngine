#pragma once

#include "Math/Vector3.h"

/**
 * @brief Defines a four-dimensional vector (x, y, z, w), which is used to efficiently rotate an object about
 *        the (x, y, z) vector by the angle theta, where w = cos(theta/2).
 */
class FQuaternion
{
public:

	/** @brief A quaternion representing no rotation. */
	static const FQuaternion Identity;

	/** @brief The X-value of the vector component of the quaternion. */
	float X = 0.0f;

	/** @brief The Y-value of the vector component of the quaternion. */
	float Y = 0.0f;

	/** @brief The Z-value of the vector component of the quaternion. */
	float Z = 0.0f;

	/** @brief The rotation component of the quaternion. */
	float W = 0.0f;

	/**
	 * @brief Sets default values for this quaternion's properties.
	 */
	constexpr FQuaternion() = default;

	/**
	 * @brief Sets default values for this quaternion's properties.
	 *
	 * @param x The initial value for the X-component of the quaternion.
	 * @param y The initial value for the Y-component of the quaternion.
	 * @param z The initial value for the Z-component of the quaternion.
	 * @param w The initial value for the W-component of the quaternion.
	 */
	constexpr FQuaternion(const float x, const float y, const float z, const float w)
		: X { x }
		, Y { y }
		, Z { z }
		, W { w }
	{
	}

	/**
	 * @brief Concatenates two Quaternions; the result represents the first rotation followed by the second rotation.
	 *
	 * @param first The first FQuaternion rotation in the series.
	 * @param second The second FQuaternion rotation in the series.
	 * @return The FQuaternion rotation representing the concatenation of first followed by second.
	 */
	[[nodiscard]] static FQuaternion Concatenate(const FQuaternion& first, const FQuaternion& second);

	/**
	 * @brief Converts this quaternion into its conjugate.
	 */
	constexpr void Conjugate()
	{
		X = -X;
		Y = -Y;
		Z = -Z;
	}

	/**
	 * @brief Creates a FQuaternion from a vector and an angle to rotate about the vector.
	 *
	 * @param axis The vector to rotate around.
	 * @param angle The angle, in radians, to rotate around the vector.
	 * @return The created FQuaternion.
	 */
	[[nodiscard]] static FQuaternion CreateFromAxisAngle(const FVector3& axis, float angle);

	// TODO: CreateFromRotationMatrix

	/**
	 * @brief Creates a new FQuaternion from specified yaw, pitch, and roll angles.
	 *
	 * @param yaw The yaw angle, in radians, around the Y-axis.
	 * @param pitch The pitch angle, in radians, around the X-axis.
	 * @param roll The roll angle, in radians, around the Z-axis.
	 * @return A new FQuaternion expressing the specified yaw, pitch, and roll angles.
	 */
	[[nodiscard]] static FQuaternion CreateFromYawPitchRoll(float yaw, float pitch, float roll);

	/**
	 * @brief Divides a FQuaternion by another FQuaternion.
	 *
	 * @param numerator The source quaternion.
	 * @param denominator The divisor.
	 * @return The result of the division.
	 */
	[[nodiscard]] static FQuaternion Divide(const FQuaternion& numerator, const FQuaternion& denominator);

	/**
	 * @brief Calculates the dot product of two Quaternions.
	 *
	 * @param first The first source quaternion.
	 * @param second The second source quaternion.
	 * @return The dot product of the quaternions.
	 */
	[[nodiscard]] static float Dot(const FQuaternion& first, const FQuaternion& second);

	/**
	 * @brief Gets this FQuaternion's conjugate.
	 *
	 * @return A FQuaternion representing this quaternion's conjugate.
	 */
	[[nodiscard]] constexpr FQuaternion GetConjugate() const
	{
		FQuaternion result = *this;
		result.Conjugate();
		return result;
	}

	/**
	 * @brief Gets a unit quaternion representing the same rotation as this quaternion.
	 *
	 * @return The resulting unit quaternion.
	 */
	[[nodiscard]] FQuaternion GetNormalized() const
	{
		FQuaternion result = *this;
		result.Normalize();
		return result;
	}

	/**
	 * @brief Gets the pointer to this matrix's values.
	 *
	 * @return The pointer to this matrix's values.
	 */
	[[nodiscard]] const float* GetValuePtr() const;

	/**
	 * @brief Gets the pointer to this matrix's values.
	 *
	 * @return The pointer to this matrix's values.
	 */
	[[nodiscard]] float* GetValuePtr();

	/**
	 * @brief Returns the inverse of a FQuaternion.
	 *
	 * @param value The source quaternion.
	 * @return The inverse of the FQuaternion.
	 */
	[[nodiscard]] static FQuaternion Inverse(const FQuaternion& value);

	/**
	 * @brief Checks to see if this quaternion is nearly equal to another quaternion.
	 *
	 * @param value The value to compare to.
	 * @param tolerance The floating point tolerance to use.
	 * @return True if this quaternion is nearly equal to the given quaternion, within \p tolerance, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsNearlyEqual(const FQuaternion& other, const float tolerance = FMath::SmallNumber) const
	{
		return FMath::IsNearlyEqual(X, other.X, tolerance) &&
		       FMath::IsNearlyEqual(Y, other.Y, tolerance) &&
		       FMath::IsNearlyEqual(Z, other.Z, tolerance) &&
		       FMath::IsNearlyEqual(W, other.W, tolerance);
	}

	/**
	 * @brief Checks to see if all components of this quaternion are nearly zero.
	 *
	 * @param tolerance The floating point tolerance to use.
	 * @return True if all components of this quaternion are nearly zero, within \p tolerance, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsNearlyZero(const float tolerance = FMath::SmallNumber) const
	{
		return FMath::IsNearlyZero(X, tolerance) &&
		       FMath::IsNearlyZero(Y, tolerance) &&
		       FMath::IsNearlyZero(Z, tolerance) &&
		       FMath::IsNearlyZero(W, tolerance);
	}

	/**
	 * @brief Calculates the length of this quaternion.
	 *
	 * @return The length of this quaternion.
	 */
	[[nodiscard]] float Length() const;

	/**
	 * @brief Calculates the length of this FQuaternion squared.
	 *
	 * @return The length of this FQuaternion squared.
	 */
	[[nodiscard]] constexpr float LengthSquared() const
	{
		return X * X + Y * Y + Z * Z + W * W;
	}

	/**
	 * @brief Linearly interpolates between two quaternions.
	 *
	 * @param from The source quaternion.
	 * @param to The destination quaternion.
	 * @param amount The weighting factor.
	 * @return The resulting quaternion.
	 */
	[[nodiscard]] static FQuaternion Lerp(const FQuaternion& from, const FQuaternion& to, float amount);

	/**
	 * @brief Multiplies two quaternions.
	 *
	 * @param first The first source quaternion.
	 * @param second The second source quaternion.
	 * @return The result of the multiplication/
	 */
	[[nodiscard]] static FQuaternion Multiply(const FQuaternion& first, const FQuaternion& second);

	/**
	 * @brief Normalizes this quaternion.
	 */
	void Normalize();

	/**
	 * @brief Interpolates between two quaternions using spherical linear interpolation.
	 *
	 * @param from The source value.
	 * @param to The destination value.
	 * @param amount Value that indicates how far to interpolate between the quaternions.
	 * @return The result of the interpolation.
	 */
	[[nodiscard]] static FQuaternion Slerp(const FQuaternion& from, const FQuaternion& to, float amount);

	/**
	 * @brief Adds another quaternion to this quaternion.
	 *
	 * @param value The quaternion to add.
	 * @return The result of the addition.
	 */
	constexpr FQuaternion& operator+=(const FQuaternion& value)
	{
		X += value.X;
		Y += value.Y;
		Z += value.Z;
		W += value.W;
		return *this;
	}

	/**
	 * @brief Adds two quaternions.
	 *
	 * @param first The first source quaternion.
	 * @param second The second source quaternion.
	 * @return The sum of the quaternions.
	 */
	constexpr friend FQuaternion operator+(FQuaternion first, const FQuaternion& second)
	{
		first += second;
		return first;
	}

	/**
	 * @brief Subtracts another quaternion from this quaternion.
	 *
	 * @param value The quaternion to subtract.
	 * @return The result of the subtraction.
	 */
	constexpr FQuaternion& operator-=(const FQuaternion& value)
	{
		X -= value.X;
		Y -= value.Y;
		Z -= value.Z;
		W -= value.W;
		return *this;
	}

	/**
	 * @brief Subtracts one quaternion from another.
	 *
	 * @param first The first source quaternion.
	 * @param second The second source quaternion.
	 * @return The result of the subtraction.
	 */
	constexpr friend FQuaternion operator-(FQuaternion first, const FQuaternion& second)
	{
		first -= second;
		return first;
	}

	/**
	 * @brief Negates a quaternion.
	 *
	 * @param value The source vector.
	 * @return The negated quaternion.
	 */
	constexpr friend FQuaternion operator-(FQuaternion value)
	{
		value.X = -value.X;
		value.Y = -value.Y;
		value.Z = -value.Z;
		value.W = -value.W;
		return value;
	}

	/**
	 * @brief Multiplies this quaternion by a scalar value.
	 *
	 * @param scalar The scalar value.
	 * @return The result of the multiplication.
	 */
	constexpr FQuaternion& operator*=(const float scalar)
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
		W *= scalar;
		return *this;
	}

	/**
	 * @brief Multiplies a quaternion by a scalar value.
	 *
	 * @param value The source quaternion.
	 * @param scalar The scalar value.
	 * @return The result of the multiplication.
	 */
	constexpr friend FQuaternion operator*(FQuaternion value, const float scalar)
	{
		value *= scalar;
		return value;
	}

	/**
	 * @brief Multiplies a quaternion by a scalar value.
	 *
	 * @param value The source quaternion.
	 * @param scalar The scalar value.
	 * @return The result of the multiplication.
	 */
	constexpr friend FQuaternion operator*(const float scalar, FQuaternion value)
	{
		value *= scalar;
		return value;
	}

	/**
	 * @brief Multiplies this quaternion by another quaternion.
	 *
	 * @param value The other quaternion.
	 * @return The result of the multiplication.
	 */
	FQuaternion& operator*=(const FQuaternion& value);

	/**
	 * @brief Multiplies a quaternion by another quaternion.
	 *
	 * @param value1 The first source quaternion.
	 * @param value2 The second source quaternion.
	 * @return The result of the multiplication.
	 */
	friend FQuaternion operator*(FQuaternion value1, const FQuaternion& value2)
	{
		value1 *= value2;
		return value1;
	}

	/**
	 * @brief Divides this quaternion by a scalar value.
	 *
	 * @param scalar The scalar value.
	 * @return The result of the division.
	 */
	constexpr FQuaternion& operator/=(const float scalar)
	{
		const float inverseScalar = FMath::SafeDivide(1.0f, scalar);
		X *= inverseScalar;
		Y *= inverseScalar;
		Z *= inverseScalar;
		W *= inverseScalar;
		return *this;
	}

	/**
	 * @brief Divides a quaternion by a scalar value.
	 *
	 * @param value The quaternion.
	 * @param scalar The scalar value.
	 * @return The result of the division.
	 */
	constexpr friend FQuaternion operator/(FQuaternion value, const float scalar)
	{
		value /= scalar;
		return value;
	}

	/**
	 * @brief Divides this quaternion by another quaternion.
	 *
	 * @param value The other quaternion.
	 * @return The result of the division.
	 */
	FQuaternion& operator/=(const FQuaternion& value);

	/**
	 * @brief Divides a quaternion by another quaternion.
	 *
	 * @param first The first source quaternion.
	 * @param second The second source quaternion.
	 * @return The result of the division.
	 */
	friend FQuaternion operator/(FQuaternion first, const FQuaternion& second)
	{
		first /= second;
		return first;
	}

	/**
	 * @brief Tests two quaternions for equality.
	 *
	 * @param first The first source quaternion.
	 * @param second The second source quaternion.
	 * @return True if the quaternions are equal, otherwise false.
	 */
	constexpr friend bool operator==(const FQuaternion& first, const FQuaternion& second)
	{
		return first.IsNearlyEqual(second);
	}

	/**
	 * @brief Tests two quaternions for inequality.
	 *
	 * @param first The first source quaternion.
	 * @param second The second source quaternion.
	 * @return True if the quaternions are not equal, otherwise false.
	 */
	constexpr friend bool operator!=(const FQuaternion& first, const FQuaternion& second)
	{
		return first.IsNearlyEqual(second) == false;
	}
};