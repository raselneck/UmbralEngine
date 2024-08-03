#include "Engine/Logging.h"
#include "Graphics/OpenGL/GraphicsDeviceGL.h"
#include "Graphics/OpenGL/Texture2DGL.h"
#include "Graphics/OpenGL/TextureManagerGL.h"
#include "Graphics/OpenGL/UmbralToGL.h"

static FStringView GetTextureResourceName(const TObjectPtr<const UTexture>& texture)
{
	if (const UTexture2DGL* texture2D = Cast<UTexture2DGL>(texture.GetObject()))
	{
		return texture2D->GetResourceName();
	}

	return "<null>"_sv;
}

int32 UTextureManagerGL::BindTexture(const TObjectPtr<const UTexture> texture)
{
	if (texture.IsNull())
	{
		return INDEX_NONE;
	}

	int32 textureSlot = GetBoundSlot(texture);
	if (textureSlot != INDEX_NONE)
	{
		return textureSlot;
	}

	textureSlot = GetFirstAvailableSlot();
	if (textureSlot == INDEX_NONE)
	{
		UM_LOG(Error, "No slots available to bind texture \"{}\"", GetTextureResourceName(texture));
		return INDEX_NONE;
	}

	BindTextureToSlot(texture, textureSlot);

	return textureSlot;
}

void UTextureManagerGL::BindTextureToSlot(const TObjectPtr<const UTexture> texture, const int32 slot)
{
	if (IsBound(texture))
	{
		return;
	}

	if (IsValidSlot(slot) == false)
	{
		UM_LOG(Error, "Cannot bind to texture slot {} as it is invalid", slot);
		return;
	}

	GLuint textureHandle = 0;
	GLenum textureType = GL_TEXTURE_2D;
	FStringView textureName = "<null>"_sv;

	if (texture.IsValid())
	{
		if (const UTexture2DGL* texture2D = Cast<UTexture2DGL>(texture.GetObject()))
		{
			textureHandle = texture2D->GetTextureHandle();
			textureName = texture2D->GetResourceName();
		}
		// TODO Support cubemap textures
	}

	GL_CHECK(glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + slot)));
	GL_CHECK(glBindTexture(textureType, textureHandle));

	m_BoundTextures[slot] = texture;
}

int32 UTextureManagerGL::GetBoundSlot(const TObjectPtr<const UTexture> texture) const
{
	if (texture.IsNull())
	{
		return INDEX_NONE;
	}

	return m_BoundTextures.IndexOfByPredicate([texture](const TObjectPtr<const UTexture>& boundTexture)
	{
		return boundTexture.GetObject() == texture.GetObject();
	});
}

bool UTextureManagerGL::IsBound(const TObjectPtr<const UTexture> texture) const
{
	return GetBoundSlot(texture) != INDEX_NONE;
}

void UTextureManagerGL::UnbindAllTextures()
{
	m_BoundTextures.ForEach([this](const int32 index, const TObjectPtr<const UTexture>& texture)
	{
		if (texture.IsValid())
		{
			UnbindTextureSlot(index);
		}
		return EIterationDecision::Continue;
	});
}

void UTextureManagerGL::UnbindTexture(const TObjectPtr<const UTexture> texture)
{
	if (texture.IsNull())
	{
		return;
	}

	const int32 textureSlot = GetBoundSlot(texture);
	if (textureSlot == INDEX_NONE)
	{
		UM_LOG(Warning, "Cannot unbind texture \"{}\" as it is not currently bound", GetTextureResourceName(texture));
		return;
	}

	UnbindTextureSlot(textureSlot);
}

void UTextureManagerGL::UnbindTextureSlot(const int32 slot)
{
	if (IsValidSlot(slot) == false)
	{
		UM_LOG(Error, "Cannot unbind from texture slot {} as it is invalid", slot);
		return;
	}

	if (m_BoundTextures[slot].IsNull())
	{
		UM_LOG(Warning, "No texture is currently bound to slot {}", slot);
		return;
	}

	GL_CHECK(glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + slot)));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

	m_BoundTextures[slot].Reset();
}

void UTextureManagerGL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_GraphicsDevice = GetTypedParent<UGraphicsDeviceGL>();
}

void UTextureManagerGL::ManuallyVisitReferencedObjects(FObjectHeapVisitor& visitor)
{
	Super::ManuallyVisitReferencedObjects(visitor);

	for (const TObjectPtr<const UTexture>& texture : m_BoundTextures)
	{
		visitor.Visit(texture);
	}
}

int32 UTextureManagerGL::GetFirstAvailableSlot() const
{
	for (int32 slot = 0; slot < MaxNumTextures; ++slot)
	{
		if (m_BoundTextures[slot].IsNull())
		{
			return slot;
		}
	}

	return INDEX_NONE;
}