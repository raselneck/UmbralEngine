#pragma once

#include "Containers/StringView.h"
#include "Math/Math.h"
#include "Math/Vector2.h"
#include "Misc/StringFormatting.h"

class FMatrix4;

/**
 * @brief Defines a vector with three components.
 */
class FVector3
{
public:

	/** @brief A vector pointing backward (0, 0, 1). */
	static const FVector3 Backward;

	/** @brief A vector pointing backward (0, -1, 0). */
	static const FVector3 Down;

	/** @brief A vector pointing backward (0, 0, -1). */
	static const FVector3 Forward;

	/** @brief A vector pointing backward (-1, 0, 0). */
	static const FVector3 Left;

	/** @brief A vector with all of its components set to one. */
	static const FVector3 One;

	/** @brief A vector pointing right (1, 0, 0). */
	static const FVector3 Right;

	/** @brief The unit vector for the X-axis. */
	static const FVector3 UnitX;

	/** @brief The unit vector for the Y-axis. */
	static const FVector3 UnitY;

	/** @brief The unit vector for the Z-axis. */
	static const FVector3 UnitZ;

	/** @brief A vector pointing up (0, 1, 0). */
	static const FVector3 Up;

	/** @brief A vector with all of its components set to zero. */
	static const FVector3 Zero;

	/** @brief The X-component of the vector. */
	float X = 0.0f;

	/** @brief The Y-component of the vector. */
	float Y = 0.0f;

	/** @brief The Z-component of the vector. */
	float Z = 0.0f;

	/**
	 * @brief Sets default values for this vector's properties.
	 */
	constexpr FVector3() = default;

	/**
	 * @brief Sets default values for this vector's properties.
	 *
	 * @param value The value to initialize all components to.
	 */
	constexpr explicit FVector3(const float value)
		: X { value }
		, Y { value }
		, Z { value }
	{
	}

	/**
	 * @brief Sets default values for this vector's properties.
	 *
	 * @param xy The initial X- and Y-component values.
	 * @param z The initial Z-component value.
	 */
	constexpr FVector3(const FVector2& xy, const float z)
		: X { xy.X }
		, Y { xy.Y }
		, Z { z }
	{
	}

	/**
	 * @brief Sets default values for this vector's properties.
	 *
	 * @param x The initial X-component value.
	 * @param y The initial Y-component value.
	 * @param z The initial Z-component value.
	 */
	constexpr FVector3(const float x, const float y, const float z)
		: X { x }
		, Y { y }
		, Z { z }
	{
	}

	/**
	 * @brief Returns a FVector3 containing the 2D Cartesian coordinates of a point specified in barycentric (areal) coordinates relative to a 2D triangle.
	 *
	 * @param value1 A FVector3 containing the 2D Cartesian coordinates of vertex 1 of the triangle.
	 * @param value2 A FVector3 containing the 2D Cartesian coordinates of vertex 2 of the triangle.
	 * @param value3 A FVector3 containing the 2D Cartesian coordinates of vertex 3 of the triangle.
	 * @param amount1 Barycentric coordinate \b b2, which expresses the weighting factor toward vertex 2 (specified in \em value2).
	 * @param amount2 Barycentric coordinate \b b3, which expresses the weighting factor toward vertex 3 (specified in \em value3).
	 * @return The 2D Cartesian coordinates of the specified point are placed in this FVector3 on exit.
	 */
	[[nodiscard]] static FVector3 Barycentric(const FVector3& value1, const FVector3& value2,
	                                          const FVector3& value3, float amount1, float amount2);

	/**
	 * @brief  Performs a Catmull-Rom interpolation using the specified positions.
	 *
	 * @param value1 The first position in the interpolation.
	 * @param value2 The second position in the interpolation.
	 * @param value3 The third position in the interpolation.
	 * @param value4 The fourth position in the interpolation.
	 * @param amount Weighting factor.
	 * @return A vector that is the result of the Catmull-Rom interpolation.
	 */
	[[nodiscard]] static FVector3 CatmullRom(const FVector3& value1, const FVector3& value2,
	                                         const FVector3& value3, const FVector3& value4,
	                                         float amount);

	/**
	 * @brief Restricts a value to be within a specified range.
	 *
	 * @param value The value to clamp.
	 * @param minValue The minimum value.
	 * @param maxValue The maximum value.
	 * @return The clamped value.
	 */
	[[nodiscard]] static constexpr FVector3 Clamp(const FVector3& value, const FVector3& minValue, const FVector3& maxValue)
	{
		return FVector3
		{
			FMath::Clamp(value.X, minValue.X, maxValue.X),
			FMath::Clamp(value.Y, minValue.Y, maxValue.Y),
			FMath::Clamp(value.Z, minValue.Z, maxValue.Z)
		};
	}

	/**
	 * @brief Creates a vector with the given component values, then normalizes it.
	 *
	 * @param x The initial X component value.
	 * @param y The initial Y component value.
	 * @param z The initial Z component value.
	 * @return The normalized vector.
	 */
	[[nodiscard]] static FVector3 CreateNormalized(float x, float y, float z)
	{
		FVector3 result { x, y, z };
		result.Normalize();
		return result;
	}

	/**
	 * @brief Calculates the cross product of two vectors.
	 *
	 * @param value1 The first source vector.
	 * @param value2 The second source vector.
	 * @return The cross product of the vectors.
	 */
	[[nodiscard]] static constexpr FVector3 Cross(const FVector3& value1, const FVector3& value2)
	{
		return FVector3
		{
			  value1.Y * value2.Z - value2.Y * value1.Z,
			-(value1.X * value2.Z - value2.X * value1.Z),
			  value1.X * value2.Y - value2.X * value1.Y
		};
	}

	/**
	 * @brief Calculates the cross product of this vector with another.
	 *
	 * @param other The other vector.
	 * @return The cross product of this vector \p other.
	 */
	[[nodiscard]] constexpr FVector3 Cross(const FVector3& other) const
	{
		return Cross(*this, other);
	}

	/**
	 * @brief Calculates the distance between two vectors.
	 *
	 * @param from The first source vector.
	 * @param to The second source vector.
	 * @return The distance between the vectors.
	 */
	[[nodiscard]] static float Distance(const FVector3& from, const FVector3& to)
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
	[[nodiscard]] static constexpr float DistanceSquared(const FVector3& from, const FVector3& to)
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
	[[nodiscard]] static constexpr float Dot(const FVector3& first, const FVector3& second)
	{
		return first.X * second.X + first.Y * second.Y + first.Z * second.Z;
	}

	/**
	 * @brief Calculates the dot product of this vector and another.
	 *
	 * @param other The other vector.
	 * @return The dot product of this vector and \p other.
	 */
	[[nodiscard]] constexpr float Dot(const FVector3& other) const
	{
		return X * other.X + Y * other.Y + Z * other.Z;
	}

	/**
	 * @brief Gets a unit vector in the same direction as this vector.
	 *
	 * @return The resulting unit vector.
	 */
	[[nodiscard]] FVector3 GetNormalized() const
	{
		FVector3 result = *this;
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
	[[nodiscard]] static FVector3 Hermite(const FVector3& value1, const FVector3& tangent1,
	                                      const FVector3& value2, const FVector3& tangent2,
	                                      float amount);

	/**
	 * @brief Checks to see if this vector is nearly equal to another vector.
	 *
	 * @param value The value to compare to.
	 * @param tolerance The floating point tolerance to use.
	 * @return True if this vector is nearly equal to the given vector, within \p tolerance, otherwise false.
	 */
	[[nodiscard]] constexpr bool IsNearlyEqual(const FVector3& value, const float tolerance = FMath::SmallNumber) const
	{
		return FMath::IsNearlyEqual(X, value.X, tolerance) &&
		       FMath::IsNearlyEqual(Y, value.Y, tolerance) &&
		       FMath::IsNearlyEqual(Z, value.Z, tolerance);
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
		       FMath::IsNearlyZero(Z, tolerance);
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
		return X * X + Y * Y + Z * Z;
	}

	/**
	 * @brief Linearly interpolates between two vectors.
	 *
	 * @param from The source value.
	 * @param to The sestination value.
	 * @param amount The weighting factor.
	 * @return The interpolated vector.
	 */
	[[nodiscard]] static constexpr FVector3 Lerp(const FVector3& from, const FVector3& to, const float amount)
	{
		return FVector3
		{
			FMath::Lerp(from.X, to.X, amount),
			FMath::Lerp(from.Y, to.Y, amount),
			FMath::Lerp(from.Z, to.Z, amount)
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
	[[nodiscard]] static constexpr FVector3 LerpPrecise(const FVector3& from, const FVector3& to, const float amount)
	{
		return FVector3
		{
			FMath::LerpPrecise(from.X, to.X, amount),
			FMath::LerpPrecise(from.Y, to.Y, amount),
			FMath::LerpPrecise(from.Z, to.Z, amount)
		};
	}

	/**
	 * @brief Returns a vector that contains the highest value from each matching pair of components.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @return The vector with maximum components.
	 */
	[[nodiscard]] static constexpr FVector3 Max(const FVector3& first, const FVector3& second)
	{
		return FVector3
		{
			FMath::Max(first.X, second.X),
			FMath::Max(first.Y, second.Y),
			FMath::Max(first.Z, second.Z)
		};
	}

	/**
	 * @brief Returns a vector that contains the lowest value from each matching pair of components.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @param result The minimized vector.
	 */
	[[nodiscard]] static constexpr FVector3 Min(const FVector3& first, const FVector3& second)
	{
		return FVector3
		{
			FMath::Min(first.X, second.X),
			FMath::Min(first.Y, second.Y),
			FMath::Min(first.Z, second.Z)
		};
	}

	/**
	 * @brief Turns the current vector into a unit vector.
	 */
	void Normalize();

	/**
	 * @brief Returns a normalized version of the specified vector.
	 *
	 * @param value The source vector.
	 * @param result The normalized vector.
	 */
	[[nodiscard]] static FVector3 Normalize(const FVector3& value)
	{
		FVector3 result = value;
		result.Normalize();
		return result;
	}

	/**
	 * @brief Returns the reflection of a vector off a surface that has the specified normal.
	 *
	 * @param vector The source vector.
	 * @param normal The normal of the surface.
	 * @return The reflected vector.
	 */
	[[nodiscard]] static FVector3 Reflect(const FVector3& vector, const FVector3& normal);

	/**
	 * @brief Interpolates between two values using a cubic equation.
	 *
	 * @param value1 The source value.
	 * @param value2 The destination value.
	 * @param amount The weighting factor.
	 * @return The interpolated value.
	 */
	[[nodiscard]] static FVector3 SmoothStep(const FVector3& value1, const FVector3& value2, float amount);

	/**
	 * @brief Transforms a vector using the given transformation matrix.
	 *
	 * @param value The vector to transform.
	 * @param transform The transformation matrix.
	 * @return The transformed vector.
	 */
	[[nodiscard]] static FVector3 Transform(const FVector3& value, const FMatrix4& transform);

	/**
	 * @brief Transforms a normalized vector (or direction) using the given transformation matrix.
	 *
	 * @param normal The normal to transform.
	 * @param transform The transformation matrix.
	 * @param result The transformed normal.
	 */
	[[nodiscard]] static FVector3 TransformNormal(const FVector3& normal, const FMatrix4& transform);

	/**
	 * @brief Adds another vector to this vector.
	 *
	 * @param value The vector to add.
	 * @return The result of the addition.
	 */
	constexpr FVector3& operator+=(const FVector3& value)
	{
		X += value.X;
		Y += value.Y;
		Z += value.Z;
		return *this;
	}

	/**
	 * @brief Adds two vectors.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @return The sum of the source vectors.
	 */
	constexpr friend FVector3 operator+(FVector3 first, const FVector3& second)
	{
		first += second;
		return first;
	}

	/**
	 * @brief Subtracts another vector from this vector.
	 *
	 * @param value The vector to subtract.
	 * @return The result of the subtraction.
	 */
	constexpr FVector3& operator-=(const FVector3& value)
	{
		X -= value.X;
		Y -= value.Y;
		Z -= value.Z;
		return *this;
	}

	/**
	 * @brief Subtracts one vector from another.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @return The result of the subtraction.
	 */
	constexpr friend FVector3 operator-(FVector3 first, const FVector3& second)
	{
		first -= second;
		return first;
	}

	/**
	 * @brief Returns a vector pointing in the opposite direction.
	 *
	 * @param value The source vector.
	 * @return Vector pointing in the opposite direction.
	 */
	constexpr friend FVector3 operator-(FVector3 value)
	{
		value.X = -value.X;
		value.Y = -value.Y;
		value.Z = -value.Z;
		return value;
	}

	/**
	 * @brief Multiplies this vector by a scalar value.
	 *
	 * @param scalar The scalar value.
	 * @return The result of the multiplication.
	 */
	constexpr FVector3& operator*=(const float scalar)
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
		return *this;
	}

	/**
	 * @brief Multiplies a vector by a scalar value.
	 *
	 * @param value The source vector.
	 * @param scalar The scalar value.
	 * @return The result of the multiplication.
	 */
	constexpr friend FVector3 operator*(FVector3 value, const float scalar)
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
	constexpr FVector3& operator*=(const FVector3& other)
	{
		X *= other.X;
		Y *= other.Y;
		Z *= other.Z;
		return *this;
	}

	/**
	 * @brief Multiplies the components of two vectors by each other.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @param result The result of the multiplication.
	 */
	constexpr friend FVector3 operator*(FVector3 first, const FVector3& second)
	{
		first *= second;
		return first;
	}

	/**
	 * @brief Divides this vector by a scalar value.
	 *
	 * @param scalar The scalar value.
	 * @return The result of the division.
	 */
	constexpr FVector3& operator/=(const float scalar)
	{
		const float inverseScalar = FMath::SafeDivide(1.0f, scalar);
		X *= inverseScalar;
		Y *= inverseScalar;
		Z *= inverseScalar;
		return *this;
	}

	/**
	 * @brief Divides a vector by a scalar value.
	 *
	 * @param value The source vector.
	 * @param divisor The divisor.
	 * @return The result of the division.
	 */
	constexpr friend FVector3 operator/(FVector3 value, const float scalar)
	{
		value /= scalar;
		return value;
	}

	/**
	 * @brief Tests two vectors for equality.
	 *
	 * @param first The first source vector.
	 * @param second The second source vector.
	 * @return True if the vectors are equal, otherwise false.
	 */
	constexpr friend bool operator==(const FVector3& first, const FVector3& second)
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
	constexpr friend bool operator!=(const FVector3& first, const FVector3& second)
	{
		return first.IsNearlyEqual(second) == false;
	}
};

template<>
struct TFormatter<FVector3>
{
	void BuildString(const FVector3& value, FStringBuilder& builder);
	bool Parse(FStringView formatString);
};