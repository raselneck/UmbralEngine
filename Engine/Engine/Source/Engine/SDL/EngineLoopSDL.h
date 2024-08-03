#pragma once

#include "Engine/EngineLoop.h"
#include "EngineLoopSDL.Generated.h"

class UApplicationSDL;
class UEngineWindowSDL;
class UInputManagerSDL;

#if WITH_IMGUI
class UImGuiSystemSDL;
#endif

UM_CLASS()
class UEngineLoopSDL : public UEngineLoop
{
	UM_GENERATED_BODY();

protected:

	/** @copydoc UEngineLoop::BeginRun */
	virtual void BeginRun() override;

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UEngineLoop::EndFrame */
	virtual void EndFrame() override;

	/** @copydoc UEngineLoop::PollEvents */
	virtual void PollEvents() override;

private:

	UM_PROPERTY()
	TObjectPtr<UApplicationSDL> m_Application;

#if WITH_IMGUI
	UM_PROPERTY()
	TObjectPtr<UImGuiSystemSDL> m_ImGuiSystem;
#endif
};