#pragma once

#include "Engine/IntTypes.h"
#include "Meta/MetaMacros.h"
#include "MouseButton.Generated.h"

/**
 * @brief Defines all mouse buttons.
 */
UM_ENUM()
enum class EMouseButton : uint8
{
	/** @brief The left mouse button. */
	Left,
	/** @brief The middle mouse button. */
	Middle,
	/** @brief The right mouse button. */
	Right,
	/** @brief The first extra mouse button. */
	Extra1,
	/** @brief The second extra mouse button. */
	Extra2
};