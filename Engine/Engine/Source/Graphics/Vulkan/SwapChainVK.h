#pragma once

#include "Graphics/SwapChain.h"
#include "SwapChainVK.Generated.h"

class UGraphicsDeviceVK;

UM_CLASS(ChildOf=UGraphicsDeviceVK)
class USwapChainVK final : public USwapChain
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets the associated graphics device.
	 *
	 * @return The associated graphics device.
	 */
	[[nodiscard]] TObjectPtr<UGraphicsDeviceVK> GetGraphicsDevice() const;

	/** @copydoc USwapChain::SetSwapInterval */
	virtual TErrorOr<void> SetSwapInterval(ESwapInterval swapInterval) override;

protected:

	/** @copydoc USwapChain::SwapBuffers */
	virtual void SwapBuffers() override;
};