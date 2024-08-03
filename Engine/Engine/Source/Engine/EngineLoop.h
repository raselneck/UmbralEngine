#pragma once

#include "Engine/Engine.h"
#include "Engine/EngineViewport.h"
#include "Engine/EngineWindow.h"
#include "Engine/Error.h"
#include "Engine/GameTime.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"
#include "EngineLoop.Generated.h"

class UApplication;

/**
 * @brief Controls most aspects of an engine loop.
 */
UM_CLASS(Abstract)
class UEngineLoop : public UObject
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Requests to exit the engine loop.
	 *
	 * @param exitCode The code to use for the exit.
	 */
	void Exit(int32 exitCode);

	/**
	 * @brief Gets the application this engine loop is associated with.
	 *
	 * @return The application this engine loop is associated with.
	 */
	[[nodiscard]] TObjectPtr<UApplication> GetApplication() const
	{
		return m_Application;
	}

	/**
	 * @brief Gets the associated engine.
	 *
	 * @return The engine this engine loop is associated with.
	 */
	[[nodiscard]] TObjectPtr<UEngine> GetEngine() const;

	/**
	 * @brief Gets the engine loop's exit code.
	 *
	 * @return The code the engine loop was exited with.
	 */
	[[nodiscard]] int32 GetExitCode() const
	{
		return m_ExitCode;
	}

	/**
	 * @brief Checks to see if this engine loop is still running.
	 *
	 * @return True if this engine loop is running, otherwise false.
	 */
	[[nodiscard]] bool IsRunning() const
	{
		return m_IsRunning;
	}

	/**
	 * @brief Runs the engine loop until an exit is requested.
	 */
	void Run();

protected:

	/**
	 * @brief Called at the beginning of a frame.
	 */
	virtual void BeginFrame();

	/**
	 * @brief Called just before running the engine loop.
	 */
	virtual void BeginRun();

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/**
	 * @brief Called at the end of a frame.
	 */
	virtual void EndFrame();

	/**
	 * @brief Called just after the engine loop has finished running.
	 */
	virtual void EndRun();

	/**
	 * @brief Polls the system for all events.
	 */
	virtual void PollEvents();

private:

	/**
	 * @brief Runs a single frame of the engine loop.
	 *
	 * @param application The current application.
	 * @param viewport The currently running viewport.
	 */
	void RunFrame(const TObjectPtr<UApplication>& application);

	UM_PROPERTY()
	TObjectPtr<UApplication> m_Application;

#if WITH_IMGUI
	UM_PROPERTY()
	TObjectPtr<UImGuiSystem> m_ImGuiSystem;
#endif

	FGameTime m_GameTime;
	int32 m_ExitCode = 0;
	bool m_IsRunning = false;
};