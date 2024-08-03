#include "Engine/Logging.h"
#include "Graphics/OpenGL/GraphicsDeviceGL.h"
#include "Graphics/OpenGL/SaveBoundResourceScope.h"
#include "Graphics/OpenGL/Texture2DGL.h"
#include "Graphics/OpenGL/TextureManagerGL.h"
#include "Graphics/OpenGL/UmbralToGL.h"

int32 UTexture2DGL::Bind() const
{
	const TObjectPtr<UTextureManagerGL> textureManager = GetTextureManager();
	return textureManager->BindTexture(this);
}

int32 UTexture2DGL::GetTextureSlot() const
{
	const TObjectPtr<UTextureManagerGL> textureManager = GetTextureManager();
	return textureManager->GetBoundSlot(this);
}

bool UTexture2DGL::IsBound() const
{
	const TObjectPtr<UTextureManagerGL> textureManager = GetTextureManager();
	return textureManager->IsBound(this);
}

void UTexture2DGL::SetData(const int32 width, const int32 height, const void* pixels, const ETextureFormat format, const EGenerateMipMaps generateMipMaps)
{
	if (width <= 0 || width > MaxWidth)
	{
		UM_LOG(Error, "Invalid width given for 2D texture ({})", width);
		return;
	}

	if (height <= 0 || height > MaxHeight)
	{
		UM_LOG(Error, "Invalid height given for 2D texture ({})", height);
		return;
	}

	// TODO Inline this call so that we're not retrieving and storing the texture binding multiple times?
	SetSamplerState(ESamplerState::LinearClamp);

	const GLenum internalFormat = GL::GetTextureInternalFormat(format);
	const GLenum nativeFormat = GL::GetTextureFormat(format);
	const GLenum dataType = GL::GetTextureDataType(format);

	const FSaveBoundTexture2DScope saveTextureBinding { m_TextureHandle };
	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, nativeFormat, dataType, pixels));

	m_Width = width;
	m_Height = height;

	m_HasMipMaps = (generateMipMaps == EGenerateMipMaps::Yes);
	if (m_HasMipMaps)
	{
		GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
	}
}

void UTexture2DGL::SetSamplerState(const FSamplerState& samplerState)
{
	const GLenum magFilter = GL::GetTextureMagFilter(samplerState.Filter);
	const GLenum minFilter = m_HasMipMaps
	                       ? GL::GetTextureMinMipFilter(samplerState.Filter)
	                       : GL::GetTextureMinFilter(samplerState.Filter);
	const GLenum wrapS = GL::GetTextureWrapMode(samplerState.AddressU);
	const GLenum wrapT = GL::GetTextureWrapMode(samplerState.AddressV);

	const FSaveBoundTexture2DScope saveTextureBinding { m_TextureHandle };

	GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_TextureHandle));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT));
}

void UTexture2DGL::Unbind() const
{
	const TObjectPtr<UTextureManagerGL> textureManager = GetTextureManager();
	textureManager->UnbindTexture(this);
}

void UTexture2DGL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_ResourceName.Reset();
	m_ResourceName.Append("<texture>"_sv);

	GL_CHECK(glGenTextures(1, &m_TextureHandle));
}

void UTexture2DGL::Destroyed()
{
	Super::Destroyed();

	if (SetActiveContextIfPossible() == EContextState::Unavailable)
	{
		return;
	}

	const TObjectPtr<UTextureManagerGL> textureManager = GetTextureManager();
	const int32 textureSlot = textureManager->GetBoundSlot(this);
	if (textureSlot != INDEX_NONE)
	{
		textureManager->UnbindTextureSlot(textureSlot);
	}

	if (m_TextureHandle != InvalidTextureHandle)
	{
		GL_CHECK(glDeleteTextures(1, &m_TextureHandle));
	}
}

TObjectPtr<UTextureManagerGL> UTexture2DGL::GetTextureManager() const
{
	const TObjectPtr<const UGraphicsDeviceGL> graphicsDevice = GetTypedParent<UGraphicsDeviceGL>();
	return graphicsDevice->GetTextureManager();
}