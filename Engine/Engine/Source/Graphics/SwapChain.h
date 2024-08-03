#pragma once

#include "Containers/Array.h"
#include "Engine/Error.h"
#include "Misc/Badge.h"
#include "Object/Object.h"
#include "SwapChain.Generated.h"

/**
 * @brief An enumeration of supported swap intervals.
 */
UM_ENUM()
enum class ESwapInterval
{
	/**
	 * @brief Adaptive vsync works the same as vsync, but if you've already missed the
	 *        vertical retrace for a given frame, it swaps buffers immediately, which
	 *        might be less jarring for the user during occasional framerate drops.
	 */
	Adaptive = -1,

	/**
	 * @brief Will immediately display buffers, but may cause tearing.
	 */
	Immediate = 0,

	/**
	 * @brief Waits for the GPU to display and swap buffers.
	 */
	Synchronized = 1
};

/**
 * @brief Defines the base for all swap chains.
 */
UM_CLASS(Abstract)
class USwapChain : public UObject
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Sets the swap interval.
	 *
	 * @param swapInterval The new swap interval.
	 */
	virtual TErrorOr<void> SetSwapInterval(ESwapInterval swapInterval);

	/**
	 * @brief Swaps the underlying buffers.
	 */
	virtual void SwapBuffers();
};