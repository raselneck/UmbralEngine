#pragma once

#include "Containers/StaticArray.h"
#include "Memory/Memory.h"

/**
 * @brief Represents the right-handed 3x3 floating point matrix, which can store translation, scale, and rotation information.
 */
class FMatrix3
{
public:

	using ArrayType = TStaticArray<float, 9>;

	/** @brief The 3x3 identity matrix. */
	static const FMatrix3 Identity;

	/** @brief A first row and first column value. */
	float M11 = 0.0f;

	/** @brief A first row and second column value. */
	float M12 = 0.0f;

	/** @brief A first row and third column value. */
	float M13 = 0.0f;

	/** @brief A second row and first column value. */
	float M21 = 0.0f;

	/** @brief A second row and second column value. */
	float M22 = 0.0f;

	/** @brief A second row and third column value. */
	float M23 = 0.0f;

	/** @brief A third row and first column value. */
	float M31 = 0.0f;

	/** @brief A third row and second column value. */
	float M32 = 0.0f;

	/** @brief A third row and third column value. */
	float M33 = 0.0f;

	/**
	 * @brief Sets default values for this 3x3 matrix's properties.
	 */
	constexpr FMatrix3() = default;

	// clang-format off

	/**
	 * @brief Sets default values for this 3x3 matrix's properties.
	 *
	 * @param m11 The initial value for the first column, first row.
	 * @param m12 The initial value for the first column, second row.
	 * @param m13 The initial value for the first column, third row.
	 * @param m21 The initial value for the second column, first row.
	 * @param m22 The initial value for the second column, second row.
	 * @param m23 The initial value for the second column, third row.
	 * @param m31 The initial value for the third column, first row.
	 * @param m32 The initial value for the third column, second row.
	 * @param m33 The initial value for the third column, third row.
	 */
	constexpr FMatrix3(float m11, float m12, float m13,
	                   float m21, float m22, float m23,
	                   float m31, float m32, float m33)
		: M11 { m11 }, M12 { m12 }, M13 { m13 }
		, M21 { m21 }, M22 { m22 }, M23 { m23 }
		, M31 { m31 }, M32 { m32 }, M33 { m33 }
	{
	}

	// clang-format on

	/**
	 * Gets the pointer to this matrix's values.
	 * @return The pointer to this matrix's values.
	 */
	[[nodiscard]] const float* GetValuePtr() const;

	/**
	 * Gets the pointer to this matrix's values.
	 * @return The pointer to this matrix's values.
	 */
	[[nodiscard]] float* GetValuePtr();

	/**
	 * Sets this matrix's values to represent the identity matrix.
	 */
	void SetIdentity();

	/**
	 * Gets an array with values equivalent to this matrix.
	 * @return An array with values equivalent to this matrix.
	 */
	[[nodiscard]] ArrayType ToArray() const;
};