#pragma once

#include "Engine/IntTypes.h"
#include "Math/Math.h"
#include "Templates/IsInt.h"

namespace Private
{
	/**
	 * @brief Defines a 2D point.
	 */
	template<typename T>
	class TPoint final
	{
	public:

		using ElementType = T;

		/** This point's X coordinate. */
		ElementType X { 0 };

		/** This point's Y coordinate. */
		ElementType Y { 0 };

		/**
		 * @brief Sets default values for this point's properties.
		 */
		constexpr TPoint() noexcept = default;

		/**
		 * @brief Sets default values for this point's properties.
		 *
		 * @param x The initial X coordinate value.
		 * @param y The initial Y coordinate value.
		 */
		constexpr TPoint(const ElementType x, const ElementType y) noexcept
			: X { x }
			, Y { y }
		{
		}

		/**
		 * @brief Casts this point to another typed point.
		 *
		 * @tparam U The other type.
		 * @return This point as another typed point.
		 */
		template<typename U>
		[[nodiscard]] TPoint<U> Cast() const
		{
			return TPoint<U> { static_cast<U>(X), static_cast<U>(Y) };
		}

		/**
		 * @brief Adds a point's values to this point.
		 *
		 * @param value The point values to add.
		 * @return This point with the addition of \p Value.
		 */
		constexpr TPoint& operator+=(const TPoint& value) noexcept
		{
			X += value.X;
			Y += value.Y;
			return *this;
		}

		/**
		 * @brief Adds two points together.
		 *
		 * @param first The first point.
		 * @param second The point to add to the first.
		 * @return The sum of the two points.
		 */
		constexpr friend TPoint operator+(TPoint first, const TPoint& second) noexcept
		{
			first += second;
			return first;
		}

		/**
		 * Subtracts a point's values from this point.
		 * @param value The point values to subtract.
		 * @return This point with the subtraction of \p Value.
		 */
		constexpr TPoint& operator-=(const TPoint& value) noexcept
		{
			X -= value.X;
			Y -= value.Y;
			return *this;
		}

		/**
		 * Subtracts one point from another.
		 * @param first The first point.
		 * @param second The point to subtract from the first.
		 * @return The result of the subtraction.
		 */
		constexpr friend TPoint operator-(TPoint first, const TPoint& second) noexcept
		{
			first -= second;
			return first;
		}

		/**
		 * @brief Multiplies this point's component values by that of another.
		 *
		 * @param scalar The point values to multiply by.
		 * @return This point multiplied by \p Scalar.
		 */
		constexpr TPoint& operator*=(const T scalar) noexcept
		{
			X *= scalar;
			Y *= scalar;
			return *this;
		}

		/**
		 * Multiplies this point's component values by that of another.
		 * @param value The point values to multiply by.
		 * @return This point multiplied by \p Value.
		 */
		constexpr TPoint& operator*=(const TPoint& value) noexcept
		{
			X *= value.X;
			Y *= value.Y;
			return *this;
		}

		/**
		 * Multiplies the components of two point by each other.
		 * @param point1 The first point.
		 * @param point2 The second point.
		 * @return The result of the multiplication.
		 */
		constexpr friend TPoint operator*(TPoint point1, const TPoint& point2) noexcept
		{
			point1 *= point2;
			return point1;
		}

		/**
		 * @brief Divides this point's component values by that of another.
		 *
		 * @param value The point values to multiply by.
		 * @return This point divided by \p value.
		 */
		constexpr TPoint& operator/=(const T scalar) noexcept
		{
			const T InverseScalar = FMath::SafeDivide(1, scalar);
			X *= InverseScalar;
			Y *= InverseScalar;
			return *this;
		}

		/**
		 * Divides this point's component values by that of another.
		 * @param value The point values to multiply by.
		 * @return This point divided by \p value.
		 */
		constexpr TPoint& operator/=(const TPoint& value) noexcept
		{
			X = FMath::SafeDivide(X, value.X);
			Y = FMath::SafeDivide(Y, value.Y);
			return *this;
		}

		/**
		 * Divides the components of two point by each other.
		 * @param first The first point.
		 * @param second The second point.
		 * @return The result of the division.
		 */
		constexpr friend TPoint operator/(TPoint first, const TPoint& second) noexcept
		{
			first /= second;
			return first;
		}

		/**
		 * @brief Checks to see if another point is equal to this point.
		 *
		 * @param other The other point.
		 * @return True if the two points are equal, otherwise false.
		 */
		constexpr bool operator==(const TPoint& other) noexcept
		{
			if constexpr (IsInt<ElementType>)
			{
				return (X == other.X) && (Y == other.Y);
			}
			else
			{
				return FMath::IsNearlyEqual(X, other.X) && FMath::IsNearlyEqual(Y, other.Y);
			}
		}

		/**
		 * @brief Checks to see if another point is not equal to this point.
		 *
		 * @param other The other point.
		 * @return True if the two points are not equal, otherwise false.
		 */
		constexpr bool operator!=(const TPoint& other) noexcept
		{
			return operator==(other) == false;
		}
	};
}

using ::Private::TPoint;

/** @brief Defines a point with floating point values. */
using FFloatPoint = TPoint<float>;

/** @brief Defines a point with signed 32-bit integer values. */
using FIntPoint = TPoint<int32>;