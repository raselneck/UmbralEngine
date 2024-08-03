#pragma once

#include "Engine/CoreTypes.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "Math/Vector2.h"

namespace Private
{
	template<typename T> struct TSizeAspectRatio;
	template<> struct TSizeAspectRatio<int8>   { using Type = float; };
	template<> struct TSizeAspectRatio<int16>  { using Type = float; };
	template<> struct TSizeAspectRatio<int32>  { using Type = float; };
	template<> struct TSizeAspectRatio<uint8>  { using Type = float; };
	template<> struct TSizeAspectRatio<uint16> { using Type = float; };
	template<> struct TSizeAspectRatio<uint32> { using Type = float; };
	template<> struct TSizeAspectRatio<float>  { using Type = float; };
	template<> struct TSizeAspectRatio<int64>  { using Type = double; };
	template<> struct TSizeAspectRatio<uint64> { using Type = double; };
	template<> struct TSizeAspectRatio<double> { using Type = double; };

	/**
	 * @brief Defines a size.
	 *
	 * @tparam T The size's element type.
	 */
	template<typename T>
	class TSize
	{
	public:

		using ElementType = T;
		using AspectRatioType = typename TSizeAspectRatio<T>::Type;
		using PointType = ::Private::TPoint<ElementType>;

		/** @brief This size's width. */
		ElementType Width = 0;

		/** @brief This size's height. */
		ElementType Height = 0;

		/**
		 * @brief Gets this size as a point.
		 *
		 * @return This size as a point.
		 */
		[[nodiscard]] constexpr PointType AsPoint() const
		{
			return PointType { Width, Height };
		}

		/**
		 * @brief Gets this size as a two-component vector.
		 *
		 * @return This size as a two-component vector.
		 */
		[[nodiscard]] constexpr FVector2 AsVector2() const
		{
			return FVector2 { static_cast<float>(Width), static_cast<float>(Height) };
		}

		/**
		 * @brief Casts this size to another typed size.
		 *
		 * @tparam U The other type.
		 * @return This size as another typed size.
		 */
		template<typename U>
		[[nodiscard]] constexpr TSize<U> Cast() const
		{
			return TSize<U> { static_cast<U>(Width), static_cast<U>(Height) };
		}

		/**
		 * @brief Gets this size's aspect ratio.
		 *
		 * @return This size's aspect ratio.
		 */
		[[nodiscard]] constexpr AspectRatioType GetAspectRatio() const
		{
			const AspectRatioType aspectWidth = static_cast<AspectRatioType>(Width);
			const AspectRatioType aspectHeight = static_cast<AspectRatioType>(Height);
			return FMath::SafeDivide(aspectWidth, aspectHeight);
		}

		/**
		 * @brief Checks to see if both components of this size are zero.
		 *
		 * @return True if both components of this size are zero, otherwise false.
		 */
		[[nodiscard]] constexpr bool IsEmpty() const
		{
			if constexpr (IsInt<ElementType>)
			{
				return Width == 0 || Height == 0;
			}
			else
			{
				return FMath::IsNearlyZero(Width) || FMath::IsNearlyZero(Height);
			}
		}
	};
}

using ::Private::TSize;

/** @brief Defines a size with floating point values. */
using FFloatSize = TSize<float>;

/** @brief Defines a size with signed 32-bit integer values. */
using FIntSize = TSize<int32>;