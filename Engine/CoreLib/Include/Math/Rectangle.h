#pragma once

#include "Engine/IntTypes.h"
#include "Math/Point.h"
#include "Math/Size.h"

namespace Private
{
	/**
	 * @brief Defines the template base for rectangles.
	 *
	 * @tparam T The rectangle's element type;
	 */
	template<typename T>
	struct TRect
	{
		using ElementType = T;
		using PointType = ::Private::TPoint<ElementType>;
		using SizeType = ::Private::TSize<ElementType>;

		/** @brief This rectangle's X coordinate. */
		T X = 0;

		/** @brief This rectangle's Y coordinate. */
		T Y = 0;

		/** @brief This rectangle's width. */
		T Width = 0;

		/** @brief This rectangle's height. */
		T Height = 0;

		/**
		 * @brief Gets this rectangle's position.
		 *
		 * @return This rectangle's position.
		 */
		PointType GetPosition() const
		{
			return PointType { X, Y };
		}

		/**
		 * @brief Gets this rectangle's size.
		 *
		 * @return This rectangle's size.
		 */
		SizeType GetSize() const
		{
			return SizeType { Width, Height };
		}
	};
}

using FIntRect = Private::TRect<int32>;
using FFloatRect = Private::TRect<float>;