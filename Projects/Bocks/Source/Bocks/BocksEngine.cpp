#include "Bocks/BocksEngine.h"
#include "Bocks/BocksViewport.h"

TSubclassOf<UEngineViewport> UBocksEngine::GetViewportClass() const
{
	return UBocksViewport::StaticType();
}