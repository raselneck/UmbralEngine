#pragma once

#include "Containers/Function.h"
#include "Engine/Error.h"
#include "Engine/VideoDisplay.h"
#include "Graphics/GraphicsApi.h"
#include "Math/Rectangle.h"
#include "Meta/MetaMacros.h"
#include "Meta/StructInfo.h"
#include "Object/Object.h"
#include "Application.Generated.h"

class UEngine;
class UEngineLoop;
class UEngineViewport;
class UEngineWindow;
class UInputManager;
class UGraphicsDevice;
class USwapChain;

#if WITH_IMGUI
class UImGuiRenderer;
class UImGuiSystem;
#endif

/**
 * @brief Defines the interface to be implemented by all application rendering contexts.
 *
 * TODO Should maybe be UM_INTERFACE?
 */
UM_STRUCT()
class IApplicationRenderingContext
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Gets the graphics device associated with this rendering context.
	 *
	 * @return The graphics device associated with this rendering context.
	 */
	[[nodiscard]] virtual TObjectPtr<UGraphicsDevice> GetGraphicsDevice() const = 0;

	/**
	 * @brief Gets the graphics device associated with this rendering context.
	 *
	 * @tparam GraphicsDeviceType The type of the graphics device.
	 * @return The graphics device associated with this rendering context.
	 */
	template<typename GraphicsDeviceType>
	[[nodiscard]] TObjectPtr<GraphicsDeviceType> GetGraphicsDevice() const
	{
		static_assert(IsBaseOf<UGraphicsDevice, GraphicsDeviceType>);
		return Cast<GraphicsDeviceType>(GetGraphicsDevice());
	}

#if WITH_IMGUI
	/**
	 * @brief Gets the ImGui renderer associated with this rendering context.
	 *
	 * @return The ImGui renderer associated with this rendering context.
	 */
	[[nodiscard]] virtual TObjectPtr<UImGuiRenderer> GetImGuiRenderer() const = 0;

	/**
	 * @brief Gets the ImGui renderer associated with this rendering context.
	 *
	 * @tparam ImGuiRendererType The type of the ImGui renderer.
	 * @return The ImGui renderer associated with this rendering context.
	 */
	template<typename ImGuiRendererType>
	[[nodiscard]] TObjectPtr<ImGuiRendererType> GetImGuiRenderer() const
	{
		static_assert(IsBaseOf<UImGuiRenderer, ImGuiRendererType>);
		return Cast<ImGuiRendererType>(GetImGuiRenderer());
	}
#endif

	/**
	 * @brief Gets the input manager associated with this rendering context.
	 *
	 * @return The input manager associated with this rendering context.
	 */
	[[nodiscard]] virtual TObjectPtr<UInputManager> GetInputManager() const = 0;

	/**
	 * @brief Gets the swap chain associated with this rendering context.
	 *
	 * @return The swap chain associated with this rendering context.
	 */
	[[nodiscard]] virtual TObjectPtr<USwapChain> GetSwapChain() const = 0;

	/**
	 * @brief Gets the viewport associated with this rendering context.
	 *
	 * @return The viewport associated with this rendering context.
	 */
	[[nodiscard]] virtual TObjectPtr<UEngineViewport> GetViewport() const = 0;

	/**
	 * @brief Gets the window associated with this rendering context.
	 *
	 * @return The window associated with this rendering context.
	 */
	[[nodiscard]] virtual TObjectPtr<UEngineWindow> GetWindow() const = 0;

	/**
	 * @brief Gets the window associated with this rendering context.
	 *
	 * @tparam EngineWindowType The type of the window.
	 * @return The window associated with this rendering context.
	 */
	template<typename EngineWindowType>
	[[nodiscard]] TObjectPtr<EngineWindowType> GetWindow() const
	{
		static_assert(IsBaseOf<UEngineWindow, EngineWindowType>);
		return Cast<EngineWindowType>(GetWindow());
	}

protected:

	/**
	 * @brief Destroys this application rendering context.
	 */
	virtual ~IApplicationRenderingContext() = default;
};

/**
 * @brief Defines the base for Umbral applications.
 */
UM_CLASS(Abstract)
class UApplication : public UObject
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Creates a graphics device for the given window.
	 *
	 * @param window The window the graphics device should be attached to.
	 * @return The graphics device.
	 */
	[[nodiscard]] virtual TObjectPtr<UGraphicsDevice> CreateGraphicsDevice(TObjectPtr<UEngineWindow> window);

	/**
	 * @brief Creates an input manger.
	 *
	 * @param window The window the input manager should be associated with.
	 * @return The input manager.
	 */
	[[nodiscard]] virtual TObjectPtr<UInputManager> CreateInputManager(TObjectPtr<UEngineWindow> window);

	/**
	 * @brief Creates a swap chain for the given graphics device.
	 *
	 * @param graphicsDevice The graphics device the swap chain should be attached to.
	 * @return The swap chain.
	 */
	[[nodiscard]] virtual TObjectPtr<USwapChain> CreateSwapChain(TObjectPtr<UGraphicsDevice> graphicsDevice);

	/**
	 * @brief Creates a default viewport for a window.
	 *
	 * The default class for the viewport is taken from the engine.
	 *
	 * @param window The window.
	 * @return The viewport.
	 */
	[[nodiscard]] TObjectPtr<UEngineViewport> CreateViewportForWindow(TObjectPtr<UEngineWindow> window);

	/**
	 * @brief Creates a viewport for a window.
	 *
	 * @param viewportClass The viewport's class.
	 * @param window The window.
	 * @return The viewport.
	 */
	[[nodiscard]] virtual TObjectPtr<UEngineViewport> CreateViewportForWindow(TSubclassOf<UEngineViewport> viewportClass, TObjectPtr<UEngineWindow> window);

	/**
	 * @brief Creates an engine window.
	 *
	 * @param title The window title.
	 * @param graphicsApi The graphics API to be used with the window.
	 * @param width The window's initial width.
	 * @param height The window's initial height.
	 * @return The engine window.
	 */
	[[nodiscard]] virtual TObjectPtr<UEngineWindow> CreateWindow(const FString& title, EGraphicsApi graphicsApi, int32 width, int32 height);

	/**
	 * @brief Creates a window and viewport, as well as a full graphics stack.
	 *
	 * @param title The window's title.
	 * @param graphicsApi The graphics API to be used with the window.
	 * @param width The window's width.
	 * @param height The window's height.
	 * @param viewportClass The viewport class.
	 * @return The created viewport.
	 */
	[[nodiscard]] virtual TObjectPtr<UEngineViewport> CreateWindowAndViewport(const FString& title, EGraphicsApi graphicsApi, int32 width, int32 height, TSubclassOf<UEngineViewport> viewportClass);

	/**
	 * @brief Calls a function for each rendering context.
	 *
	 * @param callback The callback.
	 */
	void ForEachRenderingContext(TFunction<EIterationDecision(const IApplicationRenderingContext&)> callback) const;

	/**
	 * @brief Gets the application's engine.
	 *
	 * @return The application's engine.
	 */
	[[nodiscard]] TObjectPtr<UEngine> GetEngine() const
	{
		return m_Engine;
	}

	/**
	 * @brief Gets the application's engine loop.
	 *
	 * @return The application's engine loop.
	 */
	[[nodiscard]] TObjectPtr<UEngineLoop> GetEngineLoop() const
	{
		return m_EngineLoop;
	}

	/**
	 * @brief Gets the application's engine loop.
	 *
	 * @tparam EngineLoopType The desired system-specific type of the engine loop.
	 * @return The application's engine loop.
	 */
	template<typename EngineLoopType>
	[[nodiscard]] TObjectPtr<EngineLoopType> GetEngineLoop() const
	{
		static_assert(IsBaseOf<UEngineLoop, EngineLoopType>);
		return Cast<EngineLoopType>(GetEngineLoop());
	}

	/**
	 * @brief Gets the application's exit code.
	 *
	 * @return The application's exit code.
	 */
	[[nodiscard]] int32 GetExitCode() const
	{
		return m_ExitCode;
	}

	/**
	 * @brief Gets the graphics device associated with the given window.
	 *
	 * @param window The window.
	 * @return The graphics device.
	 */
	[[nodiscard]] TObjectPtr<UGraphicsDevice> GetGraphicsDevice(TObjectPtr<const UEngineWindow> window) const;

#if WITH_IMGUI
	/**
	 * @brief Gets this application's ImGui system.
	 *
	 * @return This application's ImGui system.
	 */
	[[nodiscard]] TObjectPtr<UImGuiSystem> GetImGuiSystem() const
	{
		return m_ImGuiSystem;
	}

	/**
	 * @brief Gets this application's ImGui system.
	 *
	 * @tparam ImGuiSystemType The desired system-specific type of the ImGui system.
	 * @return This application's ImGui system.
	 */
	template<typename ImGuiSystemType>
	[[nodiscard]] TObjectPtr<ImGuiSystemType> GetImGuiSystem() const
	{
		static_assert(IsBaseOf<UImGuiSystem, ImGuiSystemType>);
		return Cast<ImGuiSystemType>(GetImGuiSystem());
	}
#endif

	/**
	 * @brief Gets the input manager associated with the given window.
	 *
	 * @param window The window.
	 * @return The input manager.
	 */
	[[nodiscard]] TObjectPtr<UInputManager> GetInputManager(TObjectPtr<const UEngineWindow> window) const;

	/**
	 * @brief Gets the number of rendering contexts.
	 *
	 * @return The number of rendering contexts.
	 */
	[[nodiscard]] virtual int32 GetNumRenderingContexts() const;

	/**
	 * @brief Gets the number of video displays currently available.
	 *
	 * @return The number of video displays currently available.
	 */
	[[nodiscard]] virtual int32 GetNumVideoDisplays() const;

	/**
	 * @brief Gets the rendering context at the given index.
	 *
	 * @param index The index.
	 * @return The rendering context at \p index.
	 */
	[[nodiscard]] virtual const IApplicationRenderingContext* GetRenderingContext(int32 index) const;

	/**
	 * @brief Gets the video display at the given index.
	 *
	 * @param index The index.
	 * @return The video display.
	 */
	[[nodiscard]] virtual const IVideoDisplay* GetVideoDisplay(int32 index) const;

	/**
	 * @brief Gets the bounds of a video display.
	 *
	 * @param index The index of the video display.
	 * @return The video display's bounds.
	 */
	[[nodiscard]] FIntRect GetVideoDisplayBounds(const int32 index) const
	{
		const IVideoDisplay* videoDisplay = GetVideoDisplay(index);
		if (videoDisplay == nullptr)
		{
			return {};
		}
		return videoDisplay->GetBounds();
	}

	/**
	 * @brief Gets the viewport associated with the given window.
	 *
	 * @param window The window.
	 * @return The viewport.
	 */
	[[nodiscard]] TObjectPtr<UEngineViewport> GetViewport(TObjectPtr<const UEngineWindow> window) const;

	/**
	 * @brief Attempts to run the application.
	 */
	TErrorOr<void> Run();

protected:

	/**
	 * @brief Called before the engine loop has started running.
	 */
	virtual void BeginRun();

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/**
	 * @brief Creates an engine loop.
	 *
	 * @return The engine loop.
	 */
	[[nodiscard]] virtual TObjectPtr<UEngineLoop> CreateEngineLoop();

#if WITH_IMGUI
	/**
	 * @brief Creates an ImGui system to help dispatch events.
	 *
	 * @return The ImGui system.
	 */
	[[nodiscard]] virtual TObjectPtr<UImGuiSystem> CreateImGuiSystem();
#endif

	/**
	 * @brief Called after the engine loop is done running.
	 */
	virtual void EndRun();

	/**
	 * @brief Gets the rendering context for the given window.
	 *
	 * @param window The window.
	 * @return The window's rendering context.
	 */
	[[nodiscard]] const IApplicationRenderingContext* GetRenderingContextForWindow(TObjectPtr<const UEngineWindow> window) const;

private:

	UM_PROPERTY()
	TObjectPtr<UEngine> m_Engine;

	UM_PROPERTY()
	TObjectPtr<UEngineLoop> m_EngineLoop;

#if WITH_IMGUI
	UM_PROPERTY()
	TObjectPtr<UImGuiSystem> m_ImGuiSystem;
#endif

	int32 m_ExitCode = 0;
};