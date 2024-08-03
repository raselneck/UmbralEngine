#include "Graphics/SwapChain.h"

TErrorOr<void> USwapChain::SetSwapInterval(ESwapInterval swapInterval)
{
	(void)swapInterval;
	UM_ASSERT_NOT_REACHED();
}

void USwapChain::SwapBuffers()
{
}