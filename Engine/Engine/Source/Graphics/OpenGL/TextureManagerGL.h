#pragma once

#include "Containers/StaticArray.h"
#include "Graphics/GraphicsResource.h"
#include "Graphics/Texture.h"
#include "TextureManagerGL.Generated.h"

class UGraphicsDeviceGL;

UM_CLASS(ChildOf=UGraphicsDeviceGL)
class UTextureManagerGL : public UGraphicsResource
{
	UM_GENERATED_BODY();

	static constexpr int32 MaxNumTextures = 32;

public:

	/**
	 * @brief Binds the given texture and returns its bound slot.
	 *
	 * @param texture The texture.
	 * @return The texture's bound slot.
	 */
	[[nodiscard]] int32 BindTexture(TObjectPtr<const UTexture> texture);

	/**
	 * @brief Binds the given texture to the given slot.
	 *
	 * @param texture The texture.
	 * @param slot The slot.
	 */
	void BindTextureToSlot(TObjectPtr<const UTexture> texture, int32 slot);

	/**
	 * @brief Gets the slot the given texture is bound to.
	 *
	 * @param texture The texture.
	 * @return The slot \p texture is bound to, or INDEX_NONE if it is not bound.
	 */
	[[nodiscard]] int32 GetBoundSlot(TObjectPtr<const UTexture> texture) const;

	/**
	 * @brief Checks to see if the given texture is bound.
	 *
	 * @param texture The texture.
	 * @return True if the texture is bound, otherwise false.
	 */
	[[nodiscard]] bool IsBound(TObjectPtr<const UTexture> texture) const;

	/**
	 * @brief Checks to see if the given slot is valid.
	 *
	 * @param slot The slot.
	 * @return True if the slot is valid, otherwise false.
	 */
	[[nodiscard]] bool IsValidSlot(const int32 slot) const
	{
		return m_BoundTextures.IsValidIndex(slot);
	}

	/**
	 * @brief Unbinds all currently bound textures.
	 */
	void UnbindAllTextures();

	/**
	 * @brief Unbinds the given texture if it is currently bound.
	 *
	 * @param texture The texture to unbind.
	 */
	void UnbindTexture(TObjectPtr<const UTexture> texture);

	/**
	 * @brief Unbinds the texture from the given slot if it is currently bound to.
	 *
	 * @param slot The slot to unbind.
	 */
	void UnbindTextureSlot(int32 slot);

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UObject::ManuallyVisitReferencedObjects */
	virtual void ManuallyVisitReferencedObjects(FObjectHeapVisitor& visitor) override;

private:

	/**
	 * @brief Gets the first available texture slot.
	 *
	 * @return The first available texture slot.
	 */
	[[nodiscard]] int32 GetFirstAvailableSlot() const;

	// TODO Use TWeakObjectPtr instead so we can rebind over textures that may no longer be valid
	UM_PROPERTY()
	TStaticArray<TObjectPtr<const UTexture>, MaxNumTextures> m_BoundTextures;

	UM_PROPERTY()
	TObjectPtr<UGraphicsDeviceGL> m_GraphicsDevice;
};