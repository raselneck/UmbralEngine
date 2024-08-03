#pragma once

#include "Graphics/OpenGL/UmbralToGL.h"

/**
 * @brief Defines a way to save a bound texture resource.
 *
 * @tparam TextureBinding The texture binding.
 * @tparam TextureType The texture type.
 */
template<GLenum TextureBinding, GLenum TextureType>
class TSaveBoundTextureScope final
{
public:

	/**
	 * @brief Saves the currently bound texture and then binds the given texture.
	 *
	 * @param textureToBind The texture to bind.
	 */
	explicit TSaveBoundTextureScope(const GLuint textureToBind)
	{
		GLint currentTexture = 0;
		glGetIntegerv(TextureBinding, &currentTexture);

		m_TextureToRestore = static_cast<GLuint>(currentTexture);

		glBindTexture(TextureType, textureToBind);
	}

	/**
	 * @brief Restores the previously bound texture.
	 */
	~TSaveBoundTextureScope()
	{
		glBindTexture(TextureType, m_TextureToRestore);
	}

private:

	GLuint m_TextureToRestore = 0;
};

/** @brief Will save the currently bound 2D texture. */
using FSaveBoundTexture2DScope = TSaveBoundTextureScope<GL_TEXTURE_BINDING_2D, GL_TEXTURE_2D>;

/** @brief Will save the currently bound cube map texture. */
using FSaveBoundTextureCubeMapScope = TSaveBoundTextureScope<GL_TEXTURE_BINDING_CUBE_MAP, GL_TEXTURE_CUBE_MAP>;