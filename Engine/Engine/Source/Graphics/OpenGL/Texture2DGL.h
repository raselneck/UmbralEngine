#pragma once

#include "Graphics/Image.h"
#include "Graphics/Texture.h"
#include "Texture2DGL.Generated.h"

class UTextureManagerGL;

/**
 * @brief Defines a 2D texture.
 */
UM_CLASS()
class UTexture2DGL : public UTexture2D
{
	UM_GENERATED_BODY();

	static constexpr uint32 InvalidTextureHandle = static_cast<uint32>(-1);

public:

	/**
	 * @brief Attempts to bind this texture.
	 */
	[[nodiscard]] int32 Bind() const;

	/** @copydoc UTexture2D::GetHeight */
	[[nodiscard]] virtual int32 GetHeight() const override
	{
		return m_Height;
	}

	/**
	 * @brief Gets this texture's handle.
	 *
	 * @returns This texture's handle.
	 */
	[[nodiscard]] uint32 GetTextureHandle() const
	{
		return m_TextureHandle;
	}

	/**
	 * @brief Gets the slot this texture is currently bound to. Will return -1 if this texture is not bound.
	 *
	 * @returns The slot this texture is currently bound to.
	 */
	[[nodiscard]] int32 GetTextureSlot() const;

	/**
	 * @brief Gets this texture's resource name.
	 *
	 * @return This texture's resource name.
	 */
	[[nodiscard]] FStringView GetResourceName() const
	{
		return m_ResourceName.AsStringView();
	}

	/** @copydoc UTexture2D::GetWidth */
	[[nodiscard]] virtual int32 GetWidth() const override
	{
		return m_Width;
	}

	/**
	 * @brief Checks to see if this texture is bound.
	 *
	 * @returns
	 */
	[[nodiscard]] bool IsBound() const;

	/** @copydoc UObject::SetData */
	virtual void SetData(int32 width, int32 height, const void* pixels, ETextureFormat format, EGenerateMipMaps generateMipMaps) override;

	/** @copydoc UObject::SetSamplerState */
	virtual void SetSamplerState(const FSamplerState& samplerState) override;

	/**
	 * @brief If this texture is bound, it will be un-bound from its slot.
	 */
	void Unbind() const;

protected:

	/** @copydoc UObject::Created */
	virtual void Created(const FObjectCreationContext& context) override;

	/** @copydoc UObject::Destroyed */
	virtual void Destroyed() override;

private:

	/**
	 * @brief Gets the associated texture manager.
	 *
	 * @return The associated texture manager.
	 */
	[[nodiscard]] TObjectPtr<UTextureManagerGL> GetTextureManager() const;

	UM_PROPERTY()
	FString m_ResourceName;

	uint32 m_TextureHandle = InvalidTextureHandle;
	int32 m_Width = 0;
	int32 m_Height = 0;
	bool m_HasMipMaps = false;
};