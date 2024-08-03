#pragma once

#include "Engine/Application.h"
#include "Engine/SDL/EngineLoopSDL.h"
#include "Engine/SDL/VideoDisplaySDL.h"
#include "ApplicationSDL.Generated.h"

class FEngineWindowParametersSDL;
class UEngineLoopSDL;
class UEngineViewport;
class UEngineWindowSDL;
class UGraphicsDevice;
class UInputManagerSDL;
struct SDL_WindowEvent;

/**
 * @brief Defines a rendering context for an engine window.
 */
UM_STRUCT()
class FApplicationRenderingContextSDL : public IApplicationRenderingContext
{
	UM_GENERATED_BODY();

public:

	/**
	 * @brief Creates a new window rendering context.
	 *
	 * @param window The window. Must not be null.
	 */
	explicit FApplicationRenderingContextSDL(TObjectPtr<UEngineWindowSDL> window);

	/**
	 * @brief Destroys this rendering context.
	 */
	virtual ~FApplicationRenderingContextSDL() = default;

	/**
	 * @brief Creates this rendering context's graphics device if it does not exist.
	 *
	 * @return True if the graphics device was created, otherwise false.
	 */
	[[nodiscard]] bool ConditionalCreateGraphicsDevice();

#if WITH_IMGUI
	/**
	 * @brief Creates this rendering context's ImGui renderer if it does not exist.
	 *
	 * @return True if the ImGui renderer was created, otherwise false.
	 */
	[[nodiscard]] bool ConditionalCreateImGuiRenderer();
#endif

	/**
	 * @brief Creates this rendering context's input manager if it does not exist.
	 *
	 * @return True if the input manager was created, otherwise false.
	 */
	[[nodiscard]] bool ConditionalCreateInputManager();

	/**
	 * @brief Creates this rendering context's viewport if it does not exist.
	 *
	 * @param viewportClass The class to use for the viewport.
	 * @return True if the viewport was created, otherwise false.
	 */
	[[nodiscard]] bool ConditionalCreateViewport(TSubclassOf<UEngineViewport> viewportClass);

	/**
	 * @brief Creates this rendering context's swap chain if it does not exist.
	 *
	 * @return True if the swap chain was created, otherwise false.
	 */
	[[nodiscard]] bool ConditionalCreateSwapChain();

	/** @copydoc IApplicationRenderingContext::GetGraphicsDevice */
	[[nodiscard]] virtual TObjectPtr<UGraphicsDevice> GetGraphicsDevice() const override
	{
		return m_GraphicsDevice;
	}

#if WITH_IMGUI
	/** @copydoc IApplicationRenderingContext::GetImGuiRenderer */
	[[nodiscard]] virtual TObjectPtr<UImGuiRenderer> GetImGuiRenderer() const override
	{
		return m_ImGuiRenderer;
	}
#endif

	/** @copydoc IApplicationRenderingContext::GetInputManager */
	[[nodiscard]] virtual TObjectPtr<UInputManager> GetInputManager() const override;

	/**
	 * @brief Gets the SDL input manager from this rendering context.
	 *
	 * @return The SDL input manager from this rendering context.
	 */
	[[nodiscard]] TObjectPtr<UInputManagerSDL> GetInputManagerSDL() const
	{
		return m_InputManager;
	}

	/** @copydoc IApplicationRenderingContext::GetSwapChain */
	[[nodiscard]] virtual TObjectPtr<USwapChain> GetSwapChain() const override
	{
		return m_SwapChain;
	}

	/** @copydoc IApplicationRenderingContext::GetViewport */
	[[nodiscard]] virtual TObjectPtr<UEngineViewport> GetViewport() const override
	{
		return m_Viewport;
	}

	/** @copydoc IApplicationRenderingContext::GetWindow */
	[[nodiscard]] virtual TObjectPtr<UEngineWindow> GetWindow() const override;

	/**
	 * @brief Gets the SDL window from this rendering context.
	 *
	 * @return The SDL window from this rendering context.
	 */
	[[nodiscard]] TObjectPtr<UEngineWindowSDL> GetWindowSDL() const
	{
		return m_Window;
	}

	/**
	 * @brief Gets the window's ID.
	 *
	 * @return The window's ID.
	 */
	[[nodiscard]] uint32 GetWindowId() const;

	/**
	 * @brief Checks to see if this rendering context is marked for deletion.
	 *
	 * @return True if this rendering context is marked for deletion, otherwise false.
	 */
	[[nodiscard]] bool IsMarkedForDeletion() const
	{
		return m_MarkedForDeletion;
	}

	/**
	 * @brief Marks this rendering context for deletion.
	 */
	void MarkForDeletion()
	{
		m_MarkedForDeletion = true;
	}

private:

	UM_PROPERTY()
	TObjectPtr<UGraphicsDevice> m_GraphicsDevice;

#if WITH_IMGUI
	UM_PROPERTY()
	TObjectPtr<UImGuiRenderer> m_ImGuiRenderer;
#endif

	UM_PROPERTY()
	TObjectPtr<UInputManagerSDL> m_InputManager;

	UM_PROPERTY()
	TObjectPtr<USwapChain> m_SwapChain;

	UM_PROPERTY()
	TObjectPtr<UEngineViewport> m_Viewport;

	UM_PROPERTY()
	TObjectPtr<UEngineWindowSDL> m_Window;

	// FIXME Sometimes there can be a condition where a window is closed and requests to be deleted,
	//       but then ImGui requests that the window be deleted as well. If we immediately delete the
	//       window and its rendering stack, then we will hit the ensure for an invalid index in
	//       UApplicationSDL::DestroyRenderingContext. To get around that, we clean up windows after
	//       everything to do with ImGui has finished this frame
	bool m_MarkedForDeletion = false;
};

/**
 * @brief Defines an SDL-backed application.
 */
UM_CLASS()
class UApplicationSDL : public UApplication
{
	UM_GENERATED_BODY();

public:

	/** @copydoc UApplication::CreateGraphicsDevice */
	[[nodiscard]] virtual TObjectPtr<UGraphicsDevice> CreateGraphicsDevice(TObjectPtr<UEngineWindow> window) override;

	/** @copydoc UApplication::CreateInputManager */
	[[nodiscard]] virtual TObjectPtr<UInputManager> CreateInputManager(TObjectPtr<UEngineWindow> window) override;

	/** @copydoc UApplication::CreateSwapChain */
	[[nodiscard]] virtual TObjectPtr<USwapChain> CreateSwapChain(TObjectPtr<UGraphicsDevice> graphicsDevice) override;

	/** @copydoc UApplication::CreateViewportForWindow */
	[[nodiscard]] virtual TObjectPtr<UEngineViewport> CreateViewportForWindow(TSubclassOf<UEngineViewport> viewportClass, TObjectPtr<UEngineWindow> window) override;

	/** @copydoc UApplication::CreateWindow */
	[[nodiscard]] virtual TObjectPtr<UEngineWindow> CreateWindow(const FString& title, EGraphicsApi graphicsApi, int32 width, int32 height) override;

	/**
	 * @brief Creates a window using the given window parameters.
	 *
	 * @param params The window parameters.
	 * @return The created window.
	 */
	[[nodiscard]] TObjectPtr<UEngineWindowSDL> CreateWindow(const FEngineWindowParametersSDL& params);

	/** @copydoc UApplication::CreateWindowAndViewport */
	[[nodiscard]] virtual TObjectPtr<UEngineViewport> CreateWindowAndViewport(const FString& title, EGraphicsApi graphicsApi, int32 width, int32 height, TSubclassOf<UEngineViewport> viewportClass) override;

	/**
	 * @brief Creates a window and viewport.
	 *
	 * @param params The window parameters.
	 * @param viewportClass The viewport class.
	 * @return The created viewport.
	 */
	[[nodiscard]] TObjectPtr<UEngineViewport> CreateWindowAndViewport(const FEngineWindowParametersSDL& params, TSubclassOf<UEngineViewport> viewportClass);

	/**
	 * @brief Creates a window and viewport.
	 *
	 * @tparam ViewportType The viewport type.
	 * @param params The window parameters.
	 * @return The created viewport.
	 */
	template<typename ViewportType>
	[[nodiscard]] TObjectPtr<ViewportType> CreateWindowAndViewport(const FEngineWindowParametersSDL& params)
	{
		TObjectPtr<UEngineViewport> viewport = CreateWindowAndViewport(params, ViewportType::StaticType());
		return Cast<ViewportType>(viewport);
	}

	/**
	 * @brief Destroys the rendering context for the given window.
	 *
	 * @param window The window.
	 */
	void DestroyRenderingContext(TObjectPtr<UEngineWindowSDL> window);

	/**
	 * @brief Dispatches the given window event to the associated window.
	 *
	 * @param event The window event.
	 */
	void DispatchWindowEvent(const SDL_WindowEvent& event);

	/**
	 * @brief Gets an input manager by its associated window'd ID.
	 *
	 * @param id The window ID.
	 * @return The input manager.
	 */
	[[nodiscard]] TObjectPtr<UInputManagerSDL> GetInputManagerFromWindowId(uint32 id) const;

	/**
	 * @brief Gets the main rendering context.
	 *
	 * @return The main rendering context.
	 */
	[[nodiscard]] const FApplicationRenderingContextSDL* GetMainRenderingContext() const
	{
		return m_RenderingContexts.GetData();
	}

	/**
	 * @brief Gets the main rendering context.
	 *
	 * @return The main rendering context.
	 */
	[[nodiscard]] FApplicationRenderingContextSDL* GetMainRenderingContext()
	{
		return m_RenderingContexts.GetData();
	}

	/** @copydoc UApplication::GetNumRenderingContexts */
	[[nodiscard]] virtual int32 GetNumRenderingContexts() const override
	{
		return m_RenderingContexts.Num();
	}

	/** @copydoc UApplication::GetNumVideoDisplays */
	[[nodiscard]] virtual int32 GetNumVideoDisplays() const override
	{
		return m_VideoDisplays.Num();
	}

	/** @copydoc UApplication::GetRenderingContext */
	[[nodiscard]] virtual const IApplicationRenderingContext* GetRenderingContext(int32 index) const override;

	/**
	 * @brief Gets the rendering context associated with the given window.
	 *
	 * @param window The window.
	 * @return The rendering context.
	 */
	[[nodiscard]] const FApplicationRenderingContextSDL* GetRenderingContextForWindow(TObjectPtr<const UEngineWindow> window) const;

	/** @copydoc UApplication::GetVideoDisplay */
	[[nodiscard]] virtual const IVideoDisplay* GetVideoDisplay(int32 index) const override;

	/**
	 * @brief Gets a window by its ID.
	 *
	 * @param id The window ID.
	 * @return The window.
	 */
	[[nodiscard]] TObjectPtr<UEngineWindowSDL> GetWindowFromWindowId(uint32 id) const;

	/**
	 * @brief De-references all rendering contexts pending deletion so they can be reclaimed by the garbage collector.
	 */
	void PurgeRenderingContextsPendingDeletion();

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UApplication::CreateEngineLoop */
	[[nodiscard]] virtual TObjectPtr<UEngineLoop> CreateEngineLoop() override;

#if WITH_IMGUI
	/** @copydoc UApplication::CreateImGuiSystem */
	[[nodiscard]] virtual TObjectPtr<UImGuiSystem> CreateImGuiSystem() override;
#endif

	/** @copydoc UObject::Destroyed */
	virtual void Destroyed() override;

	/** @copydoc UObject::ManuallyVisitReferencedObjects */
	virtual void ManuallyVisitReferencedObjects(FObjectHeapVisitor& visitor) override;

private:

	UM_PROPERTY()
	TArray<FApplicationRenderingContextSDL> m_RenderingContexts;

	TArray<FVideoDisplaySDL> m_VideoDisplays;
};