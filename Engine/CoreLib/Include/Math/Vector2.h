#pragma once

#include "Math/Math.h"

/**
 * @brief Defines a vector with two components.
 */
class FVector2
{
public:

	/** @brief A two-component vector with both of its components set to one. */
	static const FVector2 One;

	/** @brief The unit two-component vector for the X-axis. */
	static const FVector2 UnitX;

	/** @brief The unit two-component vector for the Y-axis. */
	static const FVector2 UnitY;

	/** @brief A two-component vector with both of its components set to zero. */
	static const FVector2 Zero;

	/** @brief The X-component of the vector. */
	float X = 0.0f;

	/** @brief The Y-component of the vector. */
	float Y = 0.0f;

	/**
	 * @brief Sets default values for this vector's properties.
	 */
	constexpr FVector2() = default;

	/**
	 * @brief Sets default values for this vector's properties.
	 *
	 * @param value The value to initialize both components to.
	 */
	constexpr explicit FVector2(const float value)
		: X { value }
		, Y { value }
	{
	}

	/**
	 * @brief Sets default values for this vector's properties.
	 *
	 * @param x The initial value for the X-component of the vector.
	 * @param y The initial value for the Y-component of the vector.
	 */
	constexpr FVector2(const float x, const float y)
		: X { x }
		, Y { y }
	{
	}

	/**
	 * @brief Returns a vector containing the 2D Cartesian coordinates of a point specified in barycentric (areal)
	 *        coordinates relative to a 2D triangle.
	 *
	 * @param value1 A vector containing the 2D Cartesian coordinates of vertex 1 of the triangle.
	 * @param value2 A vector containing the 2D Cartesian coordinates of vertex 2 of the triangle.
	 * @param value3 A vector containing the 2D Cartesian coordinates of vertex 3 of the triangle.
	 * @param amount1 Barycentric coordinate \b b2, which expresses the weighting factor toward vertex 2 (specified in \p value2).
	 * @param amount2 Barycentric coordinate \b b3, which expresses the weighting factor toward vertex 3 (specified in \p value3).
	 * @return The 2D Cartesian coordinates of the specified point are placed in this vector on exit.
	 */
	[[nodiscard]] static FVector2 Barycentric(const FVector2& value1, const FVector2& value2, const FVector2& value3,
	                                          float amount1, float amount2);

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
	[[nodiscard]] static FVector2 CatmullRom(const FVector2& value1, const FVector2& value2,
	                                         const FVector2& value3, const FVector2& value4, float amount);

	/**
	 * @brief Restricts a value to be within a specified range.
	 *
	 * @param value The value to clamp.
	 * @param minValue The minimum Value.
	 * @param maxValue The maximum Value.
	 * @return The clamped value.
	 */
	[[nodiscard]] static constexpr FVector2 Clamp(const FVector2& value, const FVector2& minValue, const FVector2& maxValue)
	{
		FVector2 result;
		result.X = FMath::Clamp(value.X, minValue.X, maxValue.X);
		result.Y = FMath::Clamp(value.Y, minValue.Y, maxValue.Y);
		return result;
	}

	/**
	 * @brief Calculates the distance between two vectors.
	 *
	 * @param from The first source vector.
	 * @param to The second source vector.
	 * @return The distance between the vectors.
	 */
	[[nodiscard]] static float Distance(const FVector2& from, const FVector2& to)
	{
		return (to - from).Length();
	}

	/**
	 * @brief Calculates the distance between two vectors squared.
	 *
	 * @param from The first source vector.
	 * @param to The second source vector.
	 * @return The distance between the vectors.
	 */
	[[nodiscard]] static constexpr float DistanceSquared(const FVector2& from, const FVector2& to)
	{
		return (to - from).LengthSquared();
	}

	/**
	 * @brief Calculates the dot product of two vectors.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @return The dot product of the two vectors.
	 */
	[[nodiscard]] static constexpr float Dot(const FVector2& first, const FVector2& second)
	{
		return first.X * second.X + first.Y * second.Y;
	}

	/**
	 * @brief Calculates the dot product of this vector and another.
	 *
	 * @param other The Other vector.
	 * @return The dot product of this vector and \p Other.
	 */
	[[nodiscard]] constexpr float Dot(const FVector2& other) const
	{
		return Dot(*this, other);
	}

	/**
	 * @brief Gets a unit vector in the same direction as this vector.
	 *
	 * @return The resulting unit vector.
	 */
	[[nodiscard]] FVector2 GetNormalized() const
	{
		FVector2 result = *this;
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
	static FVector2 Hermite(const FVector2& value1, const FVector2& tangent1,
	                        const FVector2& value2, const FVector2& tangent2,
	                        float amount);

	/**
	 * @brief Checks to see if this vector is nearly equal to another vector.
	 *
	 * @param value The value to compare to.
	 * @param tolerance The floating point tolerance to use.
	 * @return True if this vector is nearly equal to the given vector, within \p tolerance, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsNearlyEqual(const FVector2& value, const float tolerance = FMath::SmallNumber) const
	{
		return FMath::IsNearlyEqual(X, value.X, tolerance) &&
		       FMath::IsNearlyEqual(Y, value.Y, tolerance);
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
		       FMath::IsNearlyZero(Y, tolerance);
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
		return X * X + Y * Y;
	}

	/**
	 * @brief Linearly interpolates between two vectors.
	 *
	 * @param from The source value.
	 * @param to The sestination value.
	 * @param amount The weighting factor.
	 * @return The interpolated vector.
	 */
	[[nodiscard]] static constexpr FVector2 Lerp(const FVector2& from, const FVector2& to, const float amount)
	{
		FVector2 result;
		result.X = FMath::Lerp(from.X, to.X, amount);
		result.Y = FMath::Lerp(from.Y, to.Y, amount);
		return result;
	}

	/**
	 * @brief Linearly interpolates between two vectors. This method is a less efficient, more precise version of Lerp.
	 *
	 * @param from The source value.
	 * @param to The destination value.
	 * @param amount The weighting factor.
	 * @return The interpolated value.
	 */
	[[nodiscard]] static constexpr FVector2 LerpPrecise(const FVector2& from, const FVector2& to, const float amount)
	{
		FVector2 result;
		result.X = FMath::LerpPrecise(from.X, to.X, amount);
		result.Y = FMath::LerpPrecise(from.Y, to.Y, amount);
		return result;
	}

	/**
	 * @brief Returns a vector that contains the highest value from each matching pair of components.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @return The maximized vector.
	 */
	[[nodiscard]] static constexpr FVector2 Max(const FVector2& first, const FVector2& second)
	{
		FVector2 result;
		result.X = FMath::Max(first.X, second.X);
		result.Y = FMath::Max(first.Y, second.Y);
		return result;
	}

	/**
	 * @brief Returns a vector that contains the lowest value from each matching pair of components.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @param result The minimized vector.
	 */
	[[nodiscard]] static constexpr FVector2 Min(const FVector2& first, const FVector2& second)
	{
		FVector2 result;
		result.X = FMath::Min(first.X, second.X);
		result.Y = FMath::Min(first.Y, second.Y);
		return result;
	}

	/**
	 * @brief Turns the current vector into a unit vector.
	 */
	void Normalize();

	/**
	 * @brief Interpolates between two values using a cubic equation.
	 *
	 * @param from The source value.
	 * @param to The destination value.
	 * @param amount The weighting factor.
	 * @return The interpolated value.
	 */
	[[nodiscard]] static FVector2 SmoothStep(const FVector2& from, const FVector2& to, float amount);

	// TODO: ToPoint

	// TODO: Transform (with 3x3 matrix)

	/**
	 * @brief Adds another vector to this vector.
	 *
	 * @param other The vector to add.
	 * @return The result of the addition.
	 */
	constexpr FVector2& operator+=(const FVector2& other)
	{
		X += other.X;
		Y += other.Y;
		return *this;
	}

	/**
	 * @brief Adds two vectors.
	 *
	 * @param value The first source vector.
	 * @param toAdd The second source vector.
	 * @return The sum of the source vectors.
	 */
	constexpr friend FVector2 operator+(FVector2 value, const FVector2& toAdd)
	{
		value += toAdd;
		return value;
	}

	/**
	 * @brief Subtracts another vector from this vector.
	 *
	 * @param other The vector to subtract.
	 * @return The result of the subtraction.
	 */
	constexpr FVector2& operator-=(const FVector2& other)
	{
		X -= other.X;
		Y -= other.Y;
		return *this;
	}

	/**
	 * @brief Subtracts one vector from another.
	 *
	 * @param value The first source vector.
	 * @param toSubtract The second source vector.
	 * @return The result of the subtraction.
	 */
	constexpr friend FVector2 operator-(FVector2 value, const FVector2& toSubtract)
	{
		value -= toSubtract;
		return value;
	}

	/**
	 * @brief Returns a vector pointing in the opposite direction.
	 *
	 * @param other The source vector.
	 * @return Vector pointing in the opposite direction.
	 */
	constexpr friend FVector2 operator-(FVector2 other)
	{
		other.X = -other.X;
		other.Y = -other.Y;
		return other;
	}

	/**
	 * @brief Multiplies this vector by a Scalar value.
	 *
	 * @param scalar The Scalar value.
	 * @return The result of the multiplication.
	 */
	constexpr FVector2& operator*=(const float scalar)
	{
		X *= scalar;
		Y *= scalar;
		return *this;
	}

	/**
	 * @brief Multiplies a vector by a scalar.
	 *
	 * @param value The source vector.
	 * @param scalar The scalar.
	 * @return The result of the multiplication.
	 */
	constexpr friend FVector2 operator*(FVector2 value, const float scalar)
	{
		value *= scalar;
		return value;
	}

	/**
	 * @brief Multiplies a vector by a Scalar Value.
	 *
	 * @param value The source vector.
	 * @param scalar The Scalar Value.
	 * @return The result of the multiplication.
	 */
	constexpr friend FVector2 operator*(const float scalar, FVector2 value)
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
	constexpr FVector2& operator*=(const FVector2& other)
	{
		X *= other.X;
		Y *= other.Y;
		return *this;
	}

	/**
	 * @brief Multiplies the components of two vectors by each other.
	 *
	 * @param value The first source vector.
	 * @param toMultiply The second source vector.
	 * @param result The result of the multiplication.
	 */
	constexpr friend FVector2 operator*(FVector2 value, const FVector2& toMultiply)
	{
		value *= toMultiply;
		return value;
	}

	/**
	 * @brief Divides this vector by a Scalar value.
	 *
	 * @param scalar The Scalar value.
	 * @return The result of the division.
	 */
	constexpr FVector2& operator/=(const float scalar)
	{
		const float inverseScalar = FMath::SafeDivide(1.0f, scalar);
		X *= inverseScalar;
		Y *= inverseScalar;
		return *this;
	}

	/**
	 * @brief Divides a vector by a Scalar Value.
	 *
	 * @param value The source vector.
	 * @param divisor The divisor.
	 * @return The result of the division.
	 */
	constexpr friend FVector2 operator/(FVector2 value, const float scalar)
	{
		value /= scalar;
		return value;
	}

	/**
	 * @brief Divides a vector by another vector.
	 *
	 * @param value The source vector.
	 * @param divisor The divisor vector.
	 * @return The result of the division.
	 */
	constexpr friend FVector2 operator/(FVector2 value, const FVector2& other)
	{
		value.X = FMath::SafeDivide(value.X, other.X);
		value.Y = FMath::SafeDivide(value.Y, other.Y);
		return value;
	}

	/**
	 * @brief Tests two vectors for equality.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @return True if the vectors are equal, otherwise false.
	 */
	constexpr friend bool operator==(const FVector2& first, const FVector2& second)
	{
		return first.IsNearlyEqual(second);
	}

	/**
	 * @brief Tests two vectors for inequality.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @return True if the vectors are not equal, otherwise false.
	 */
	constexpr friend bool operator!=(const FVector2& first, const FVector2& second)
	{
		return first.IsNearlyEqual(second) == false;
	}
};