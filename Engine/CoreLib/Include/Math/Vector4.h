#pragma once

#include "Math/Vector3.h"

/**
 * @brief Defines a vector with four components.
 */
class FVector4
{
public:

	/** @brief A vector with all of its components set to one. */
	static const FVector4 One;

	/** @brief The unit vector for the X-axis. */
	static const FVector4 UnitX;

	/** @brief The unit vector for the Y-axis. */
	static const FVector4 UnitY;

	/** @brief The unit vector for the Z-axis. */
	static const FVector4 UnitZ;

	/** @brief The unit vector for the W-axis. */
	static const FVector4 UnitW;

	/** @brief A vector with all of its components set to zero. */
	static const FVector4 Zero;

	/** @brief The X-component of the vector. */
	float X = 0.0f;

	/** @brief The Y-component of the vector. */
	float Y = 0.0f;

	/** @brief The Z-component of the vector. */
	float Z = 0.0f;

	/** @brief The W-component of the vector. */
	float W = 0.0f;

	/**
	 * @brief Sets default values for this vector's properties.
	 */
	constexpr FVector4() = default;

	/**
	 * @brief Sets default values for this vector's properties.
	 *
	 * @param value The value to initialize all components to.
	 */
	constexpr explicit FVector4(const float value)
		: X { value }
		, Y { value }
		, Z { value }
		, W { value }
	{
	}

	/**
	 * @brief Sets default values for this vector's properties.
	 *
	 * @param xyz The Vector3 containing the initial values for the X-, Y-, and Z-components of the vector.
	 * @param w The initial value for the W-component of the vector.
	 */
	constexpr FVector4(const FVector3& xyz, const float w)
		: X { xyz.X }
		, Y { xyz.Y }
		, Z { xyz.Z }
		, W { w }
	{
	}

	/**
	 * @brief Sets default values for this vector's properties.
	 *
	 * @param x The initial value for the X-component of the vector.
	 * @param y The initial value for the Y-component of the vector.
	 * @param z The initial value for the Z-component of the vector.
	 * @param w The initial value for the W-component of the vector.
	 */
	constexpr FVector4(const float x, const float y, const float z, const float w)
		: X { x }
		, Y { y }
		, Z { z }
		, W { w }
	{
	}

	/**
	 * @brief Returns a vector containing the 2D Cartesian coordinates of a point specified in barycentric (areal)
	 *        coordinates relative to a 2D triangle.
	 *
	 * @param value1 A FVector4 containing the 2D Cartesian coordinates of vertex 1 of the triangle.
	 * @param value2 A FVector4 containing the 2D Cartesian coordinates of vertex 2 of the triangle.
	 * @param value3 A FVector4 containing the 2D Cartesian coordinates of vertex 3 of the triangle.
	 * @param amount1 Barycentric coordinate \b b2, which expresses the weighting factor toward vertex 2 (specified in \p value2).
	 * @param amount2 Barycentric coordinate \b b3, which expresses the weighting factor toward vertex 3 (specified in \p value3).
	 * @return The 2D Cartesian coordinates of the specified point are placed in this FVector4 on exit.
	 */
	[[nodiscard]] static FVector4 Barycentric(const FVector4& value1, const FVector4& value2,
	                                          const FVector4& value3, float amount1, float amount2);

	/**
	 * @brief Performs a Catmull-Rom interpolation using the specified positions.
	 *
	 * @param value1 The first position in the interpolation.
	 * @param value2 The second position in the interpolation.
	 * @param value3 The third position in the interpolation.
	 * @param value4 The fourth position in the interpolation.
	 * @param amount Weighting factor.
	 * @return A vector that is the result of the Catmull-Rom interpolation.
	 */
	[[nodiscard]] static FVector4 CatmullRom(const FVector4& value1, const FVector4& value2,
	                                         const FVector4& value3, const FVector4& value4, float amount);

	/**
	 * @brief Restricts a value to be within a specified range.
	 *
	 * @param value The value to clamp.
	 * @param minValue The minimum value.
	 * @param maxValue The maximum value.
	 * @return The clamped value.
	 */
	[[nodiscard]] static constexpr FVector4 Clamp(const FVector4& value, const FVector4& minValue, const FVector4& maxValue)
	{
		return FVector4
		{
			FMath::Clamp(value.X, minValue.X, maxValue.X),
			FMath::Clamp(value.Y, minValue.Y, maxValue.Y),
			FMath::Clamp(value.Z, minValue.Z, maxValue.Z),
			FMath::Clamp(value.W, minValue.W, maxValue.W)
		};
	}

	/**
	 * @brief Calculates the distance between two vectors.
	 *
	 * @param from The source vector.
	 * @param to The destination vector.
	 * @return The distance between the vectors.
	 */
	[[nodiscard]] static float Distance(const FVector4& from, const FVector4& to)
	{
		return (to - from).Length();
	}

	/**
	 * @brief Calculates the distance between two vectors squared.
	 *
	 * @param from The source vector.
	 * @param to The destination vector.
	 * @return The distance between the vectors.
	 */
	[[nodiscard]] static float DistanceSquared(const FVector4& from, const FVector4& to)
	{
		return (to - from).LengthSquared();
	}

	/**
	 * @brief Gets a unit vector from this vector.
	 *
	 * @return The resulting unit vector.
	 */
	[[nodiscard]] FVector4 GetNormalized() const
	{
		FVector4 result = *this;
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
	 * @brief Performs a Hermite spline interpolation.
	 *
	 * @param value1 The first source position.
	 * @param tangent1 The first source tangent.
	 * @param value2 The second source position.
	 * @param tangent2 The second source tangent.
	 * @param amount The weighting factor.
	 * @return The result of the Hermite spline interpolation.
	 */
	[[nodiscard]] static FVector4 Hermite(const FVector4& value1, const FVector4& tangent1,
	                                      const FVector4& value2, const FVector4& tangent2, float amount);

	/**
	 * @brief Checks to see if this vector is nearly equal to another vector.
	 *
	 * @param value The value to compare to.
	 * @param tolerance The floating point tolerance to use.
	 * @return True if this vector is nearly equal to the given vector, within \p tolerance, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsNearlyEqual(const FVector4& value, const float tolerance = FMath::SmallNumber) const
	{
		return FMath::IsNearlyEqual(X, value.X, tolerance) &&
		       FMath::IsNearlyEqual(Y, value.Y, tolerance) &&
		       FMath::IsNearlyEqual(Z, value.Z, tolerance) &&
		       FMath::IsNearlyEqual(W, value.W, tolerance);
	}

	/**
	 * @brief Checks to see if all components in this vector are nearly zero.
	 *
	 * @param tolerance The floating point tolerance to use.
	 * @return True if all components in this vector are nearly zero, within \p tolerance, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsNearlyZero(const float tolerance = FMath::SmallNumber) const
	{
		return FMath::IsNearlyZero(X, tolerance) &&
		       FMath::IsNearlyZero(Y, tolerance) &&
		       FMath::IsNearlyZero(Z, tolerance) &&
		       FMath::IsNearlyZero(W, tolerance);
	}

	/**
	 * @brief Calculates the length of the vector.
	 *
	 * @return Length of the vector.
	 */
	[[nodiscard]] float Length() const;

	/**
	 * @brief Calculates the length of the vector squared.
	 *
	 * @return Length of the vector squared.
	 */
	[[nodiscard]] constexpr float LengthSquared() const
	{
		return X * X + Y * Y + Z * Z + W * W;
	}

	/**
	 * @brief Linearly interpolates between two vectors.
	 *
	 * @param from The source value.
	 * @param to The destination value.
	 * @param amount The weighting factor.
	 * @return The interpolated vector.
	 */
	[[nodiscard]] static constexpr FVector4 Lerp(const FVector4& from, const FVector4& to, const float amount)
	{
		return FVector4
		{
			FMath::Lerp(from.X, to.X, amount),
			FMath::Lerp(from.Y, to.Y, amount),
			FMath::Lerp(from.Z, to.Z, amount),
			FMath::Lerp(from.W, to.W, amount)
		};
	}

	/**
	 * @brief Linearly interpolates between two vectors. This method is a less efficient, more precise version of Lerp.
	 *
	 * @param from The source value.
	 * @param to The destination value.
	 * @param amount The weighting factor.
	 * @return The interpolated value.
	 */
	[[nodiscard]] static constexpr FVector4 LerpPrecise(const FVector4& from, const FVector4& to, const float amount)
	{
		return FVector4
		{
			FMath::LerpPrecise(from.X, to.X, amount),
			FMath::LerpPrecise(from.Y, to.Y, amount),
			FMath::LerpPrecise(from.Z, to.Z, amount),
			FMath::LerpPrecise(from.W, to.W, amount)
		};
	}

	/**
	 * @brief Returns a vector that contains the highest value from each matching pair of components.
	 *
	 * @param value1 The first source vector.
	 * @param value2 The second source vector.
	 * @return The maximized vector.
	 */
	[[nodiscard]] static constexpr FVector4 Max(const FVector4& value1, const FVector4& value2)
	{
		return FVector4
		{
			FMath::Max(value1.X, value2.X),
			FMath::Max(value1.Y, value2.Y),
			FMath::Max(value1.Z, value2.Z),
			FMath::Max(value1.W, value2.W)
		};
	}

	/**
	 * @brief Returns a vector that contains the lowest value from each matching pair of components.
	 *
	 * @param value1 The first source vector.
	 * @param value2 The second source vector.
	 * @param result The minimized vector.
	 */
	[[nodiscard]] static constexpr FVector4 Min(const FVector4& value1, const FVector4& value2)
	{
		return FVector4
		{
			FMath::Min(value1.X, value2.X),
			FMath::Min(value1.Y, value2.Y),
			FMath::Min(value1.Z, value2.Z),
			FMath::Min(value1.W, value2.W)
		};
	}

	/**
	 * @brief Turns the current vector into a unit vector.
	 */
	void Normalize();

	/**
	 * @brief Interpolates between two values using a cubic equation.
	 *
	 * @param value1 The source value.
	 * @param value2 The destination value.
	 * @param amount The weighting factor.
	 * @return The interpolated value.
	 */
	[[nodiscard]] static FVector4 SmoothStep(const FVector4& value1, const FVector4& value2, float amount);

	/**
	 * @brief Adds another vector to this vector.
	 *
	 * @param value The vector to add.
	 * @return The result of the addition.
	 */
	constexpr FVector4& operator+=(const FVector4& value)
	{
		X += value.X;
		Y += value.Y;
		Z += value.Z;
		W += value.W;
		return *this;
	}

	/**
	 * @brief Adds two vectors.
	 *
	 * @param value1 The first source vector.
	 * @param value2 The second source vector.
	 * @return The sum of the source vectors.
	 */
	constexpr friend FVector4 operator+(FVector4 value1, const FVector4& value2)
	{
		value1 += value2;
		return value1;
	}

	/**
	 * @brief Subtracts another vector from this vector.
	 *
	 * @param value The vector to subtract.
	 * @return The result of the subtraction.
	 */
	constexpr FVector4& operator-=(const FVector4& value)
	{
		X -= value.X;
		Y -= value.Y;
		Z -= value.Z;
		W -= value.W;
		return *this;
	}

	/**
	 * @brief Subtracts one vector from another.
	 *
	 * @param value1 The first source vector.
	 * @param value2 The second source vector.
	 * @return The result of the subtraction.
	 */
	constexpr friend FVector4 operator-(FVector4 value1, const FVector4& value2)
	{
		value1 -= value2;
		return value1;
	}

	/**
	 * @brief Returns a vector pointing in the opposite direction.
	 *
	 * @param value The source vector.
	 * @return Vector pointing in the opposite direction.
	 */
	constexpr friend FVector4 operator-(FVector4 value)
	{
		value.X = -value.X;
		value.Y = -value.Y;
		value.Z = -value.Z;
		value.W = -value.W;
		return value;
	}

	/**
	 * @brief Multiplies this vector by a scalar value.
	 *
	 * @param scalar The scalar value.
	 * @return The result of the multiplication.
	 */
	constexpr FVector4& operator*=(const float scalar)
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
		W *= scalar;
		return *this;
	}

	/**
	 * @brief Multiplies a vector by a scalar value.
	 *
	 * @param value The source vector.
	 * @param scalar The scalar value.
	 * @return The result of the multiplication.
	 */
	constexpr friend FVector4 operator*(FVector4 value, const float scalar)
	{
		value *= scalar;
		return value;
	}

	/**
	 * @brief Multiplies this vector's components by the components of another.
	 *
	 * @param other The source vector.
	 * @return The result of the multiplication.
	 */
	constexpr FVector4& operator*=(const FVector4& other)
	{
		X *= other.X;
		Y *= other.Y;
		Z *= other.Z;
		W *= other.W;
		return *this;
	}

	/**
	 * @brief Multiplies the components of two vectors by each other.
	 *
	 * @param value1 The first source vector.
	 * @param value2 The second source vector.
	 * @param result The result of the multiplication.
	 */
	constexpr friend FVector4 operator*(FVector4 value1, const FVector4& value2)
	{
		value1 *= value2;
		return value1;
	}

	/**
	 * @brief Divides this vector by a scalar value.
	 *
	 * @param scalar The scalar value.
	 * @return The result of the division.
	 */
	constexpr FVector4& operator/=(const float scalar)
	{
		const float inverseScalar = FMath::SafeDivide(1.0f, scalar);
		X *= inverseScalar;
		Y *= inverseScalar;
		Z *= inverseScalar;
		W *= inverseScalar;
		return *this;
	}

	/**
	 * @brief Divides a vector by a scalar value.
	 *
	 * @param value The source vector.
	 * @param divisor The divisor.
	 * @return The result of the division.
	 */
	constexpr friend FVector4 operator/(FVector4 value, const float scalar)
	{
		value /= scalar;
		return value;
	}

	/**
	 * @brief Tests two vectors for equality.
	 *
	 * @param value1 The first source vector.
	 * @param value2 The second source vector.
	 * @return True if the vectors are equal, otherwise false.
	 */
	constexpr friend bool operator==(const FVector4& value1, const FVector4& value2)
	{
		return value1.IsNearlyEqual(value2);
	}

	/**
	 * @brief Tests two vectors for inequality.
	 *
	 * @param value1 The first source vector.
	 * @param value2 The second source vector.
	 * @return True if the vectors are not equal, otherwise false.
	 */
	constexpr friend bool operator!=(const FVector4& value1, const FVector4& value2)
	{
		return value1.IsNearlyEqual(value2) == false;
	}
};