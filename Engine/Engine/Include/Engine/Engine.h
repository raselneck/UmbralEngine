#pragma once

#include "Containers/String.h"
#include "Engine/GameTime.h"
#include "Object/Object.h"
#include "Engine.Generated.h"

class UEngineLoop;
class UEngineViewport;

/**
 * @brief Defines the base class for all engines.
 */
UM_CLASS()
class UEngine : public UObject
{
	UM_GENERATED_BODY();

	friend class UEngineLoop;

public:

	/**
	 * @brief Destroys this engine.
	 */
	virtual ~UEngine() override;

	/**
	 * @brief Called just before the engine loop begins.
	 */
	virtual void BeginRun(TBadge<UEngineLoop>);

	/**
	 * @brief Creates a new viewport.
	 *
	 * @param title The viewport's title.
	 * @param width The viewport's width.
	 * @param height The viewport's height.
	 * @param viewportClass The viewport's class.
	 * @return The created viewport.
	 */
	[[nodiscard]] TObjectPtr<UEngineViewport> CreateViewport(const FString& title, int32 width, int32 height, TSubclassOf<UEngineViewport> viewportClass);

	/**
	 * @brief Creates a new viewport.
	 *
	 * @tparam ViewportClass The viewport class.
	 * @param title The viewport's title.
	 * @param width The viewport's width.
	 * @param height The viewport's height.
	 * @return The created viewport.
	 */
	template<typename ViewportClass>
	[[nodiscard]] TObjectPtr<ViewportClass> CreateViewport(const FString& title, int32 width, int32 height)
	{
		TObjectPtr<UEngineViewport> viewport = CreateViewport(title, width, height, ViewportClass::StaticType());
		if (viewport.IsNull())
		{
			return nullptr;
		}
		return CastChecked<ViewportClass>(viewport);
	}

	/**
	 * @brief Called just after the engine loop ends.
	 */
	virtual void EndRun(TBadge<UEngineLoop>);

	/**
	 * @brief Gets this engine's viewport class.
	 *
	 * @return This engine's viewport class.
	 */
	[[nodiscard]] virtual TSubclassOf<UEngineViewport> GetViewportClass() const;

	/**
	 * @brief Checks to see if this engine is headless (like a server).
	 *
	 * @return True if this engine is headless, otherwise false.
	 */
	[[nodiscard]] virtual bool IsHeadless() const;
};