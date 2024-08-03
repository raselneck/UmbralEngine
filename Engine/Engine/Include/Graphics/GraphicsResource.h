#pragma once

#include "Graphics/GraphicsContextState.h"
#include "Object/Object.h"
#include "GraphicsResource.Generated.h"

class UGraphicsDevice;

/**
 * @brief Defines the base for all graphics resources.
 */
UM_CLASS(ChildOf=UGraphicsDevice)
class UGraphicsResource : public UObject
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets the graphics device used to create this resource.
	 *
	 * @return The graphics device used to create this resource.
	 */
	[[nodiscard]] TObjectPtr<UGraphicsDevice> GetGraphicsDevice() const;

	/**
	 * @brief Gets the graphics device used to create this resource.
	 *
	 * @tparam GraphicsDeviceType The type of the graphics device.
	 * @return The graphics device used to create this resource.
	 */
	template<typename GraphicsDeviceType>
	[[nodiscard]] TObjectPtr<GraphicsDeviceType> GetGraphicsDevice() const
	{
		static_assert(IsBaseOf<UGraphicsDevice, GraphicsDeviceType>);
		return Cast<GraphicsDeviceType>(GetGraphicsDevice());
	}

protected:

	/**
	 * @brief An enumeration defining whether or not a graphics device context is available.
	 */
	using EContextState = EGraphicsContextState;

	/**
	 * @brief Sets the graphics device's context active for the calling thread.
	 */
	void SetActiveContext() const;

	/**
	 * @brief Sets the graphics device's context active for the calling thread.
	 */
	[[nodiscard]] EContextState SetActiveContextIfPossible() const;
};