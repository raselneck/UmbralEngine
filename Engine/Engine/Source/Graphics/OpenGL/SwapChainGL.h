#pragma once

#include "Graphics/SwapChain.h"
#include <SDL2/SDL.h>
#include "SwapChainGL.Generated.h"

class UGraphicsDeviceGL;

UM_CLASS(ChildOf=UGraphicsDeviceGL)
class USwapChainGL final : public USwapChain
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets the associated graphics device.
	 *
	 * @return The associated graphics device.
	 */
	[[nodiscard]] TObjectPtr<UGraphicsDeviceGL> GetGraphicsDevice() const;

	/** @copydoc USwapChain::SetSwapInterval */
	virtual TErrorOr<void> SetSwapInterval(ESwapInterval swapInterval) override;

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc USwapChain::SwapBuffers */
	virtual void SwapBuffers() override;
};