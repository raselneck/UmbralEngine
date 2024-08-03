#include "Graphics/GuiRenderer.h"
#include "Graphics/Image.h"
#include "Graphics/InternalGL.h"
#include "Engine/Logging.h"
#include "HAL/File.h"
#include "Math/Matrix4.h"
#include "Math/Rectangle.h"
#include "nuklear_internal.h"
#include <SDL.h>

static nk_allocator GNuklearAllocator
{
	nk_handle_ptr(nullptr),
	[](nk_handle, void* oldMemory, nk_size newMemorySize)
	{
		return FMemory::Realloc(oldMemory, newMemorySize);
	},
	[](nk_handle, void* memory)
	{
		FMemory::Free(memory);
	}
};

const FVertexDeclaration& FGuiVertex::GetVertexDeclaration()
{
	static FVertexDeclaration declaration
	{{
		MAKE_VERTEX_ELEMENT(FGuiVertex, Position, Vector2, Position),
		MAKE_VERTEX_ELEMENT(FGuiVertex, UV, Vector2, TextureCoordinate),
		MAKE_VERTEX_ELEMENT(FGuiVertex, Color, Color, Color)
	}};

	return declaration;
}

FGuiRenderer::~FGuiRenderer()
{
	if (m_FontAtlas.glyphs)
	{
		nk_font_atlas_clear(&m_FontAtlas);
	}
	nk_buffer_free(&m_DrawCommands);
	nk_free(&m_DrawContext);
}

TErrorOr<void> FGuiRenderer::Initialize(TSharedPtr<FGameWindow> gameWindow)
{
	m_GameWindow = std::move(gameWindow);
	if (m_GameWindow.IsInvalid())
	{
		return MAKE_ERROR("Given invalid game window");
	}

	// Attempt to initialize the shader program
	constexpr FStringView vertexShaderPath = "Shaders/nuklear.vert.spv"_sv;
	constexpr FStringView fragmentShaderPath = "Shaders/nuklear.frag.spv"_sv;
	TRY_EVAL(m_Program, FShaderProgram::LoadFromBlobFiles(vertexShaderPath, fragmentShaderPath));

	// Initialize our draw command buffer
	nk_buffer_init(&m_DrawCommands, &GNuklearAllocator, NK_BUFFER_DEFAULT_INITIAL_SIZE);

	// Initialize our Nuklear context
	if (nk_init(&m_DrawContext, &GNuklearAllocator, nullptr) == nk_false)
	{
		return MAKE_ERROR("Failed to initialize GUI draw context");
	}

	// Set the Nuklear context's clipboard callbacks
	m_DrawContext.clip.copy = [](nk_handle, const char* text, int len)
	{
		if (len <= 0)
		{
			SDL_SetClipboardText(nullptr);
			return;
		}

		TArray<char> clipboardText;
		clipboardText.Reserve(len + 1);
		clipboardText.Append(text, len);
		clipboardText.Add('\0');

		SDL_SetClipboardText(clipboardText.GetData());
	};
	m_DrawContext.clip.paste = [](nk_handle, struct nk_text_edit* editor)
	{
		if (const char* clipboardText = SDL_GetClipboardText())
		{
			const int32 clipboardTextLength = TCharTraits<char>::GetNullTerminatedLength(clipboardText);
			nk_textedit_paste(editor, clipboardText, clipboardTextLength);
		}
	};

	constexpr bool useDefaultNuklearFont = false;

	/* Load Fonts: if none of these are loaded a default font will be used  */
	/* Load Cursor: if you uncomment cursor loading please hide the cursor */
	struct nk_font_atlas* atlas = &m_FontAtlas;
	nk_font_atlas_init(atlas, &GNuklearAllocator);
	nk_font_atlas_begin(atlas);
	nk_font* mainFont = nullptr;
	if constexpr (useDefaultNuklearFont)
	{
		struct nk_font_config fontConfig = nk_font_config(0.0f);
		mainFont = nk_font_atlas_add_default(atlas, 14.0f, &fontConfig);
	}
	else
	{
		TRY_EVAL(TArray<uint8> fontBytes, FFile::ReadAllBytes("Fonts/UbuntuMono-Regular.ttf"_sv));

		struct nk_font_config fontConfig = nk_font_config(0.0f);
		mainFont = nk_font_atlas_add_from_memory(atlas, fontBytes.GetData(), fontBytes.Num(), 14.0f, &fontConfig);
	}
	//struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);
	//struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 16, 0);
	//struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);
	//struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);
	//struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);
	//struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);

	// Attempt to bake the font atlas
	const void* atlasMemory;
	int32 atlasWidth, atlasHeight;
	atlasMemory = nk_font_atlas_bake(&m_FontAtlas, &atlasWidth, &atlasHeight, NK_FONT_ATLAS_RGBA32);

	const bool setAtlasTextureData = m_FontAtlasTexture.SetData(reinterpret_cast<const FColor*>(atlasMemory), atlasWidth, atlasHeight, false);
	if (setAtlasTextureData == false)
	{
		return MAKE_ERROR("Failed to set font atlas texture data");
	}

	nk_font_atlas_end(&m_FontAtlas, nk_handle_id(static_cast<int>(m_FontAtlasTexture.GetTextureHandle())), &m_NullTexture);
	if (m_FontAtlas.default_font)
	{
		nk_style_set_font(&m_DrawContext, &m_FontAtlas.default_font->handle);
	}

	//nk_style_load_all_cursors(&m_DrawContext, atlas->cursors);
	nk_style_set_font(&m_DrawContext, &mainFont->handle);

	return {};
}

void FGuiRenderer::Render()
{
	const FIntSize windowSize = m_GameWindow->GetSize();
	const FIntSize drawableSize = m_GameWindow->GetDrawableSize();
	const FMatrix4 orthoMatrix = FMatrix4::CreateOrthographicOffCenter(0.0f, windowSize.Width, windowSize.Height, 0.0f, 0.0f, 1.0f);
	const FVector2 renderScale
	{
		static_cast<float>(drawableSize.Width) / static_cast<float>(windowSize.Width),
		static_cast<float>(drawableSize.Height) / static_cast<float>(windowSize.Height)
	};

	/* setup global state */
	GL_CHECK(glViewport(0, 0, drawableSize.Width, drawableSize.Height));
	const GLboolean wasBlendDisabled = glIsEnabled(GL_BLEND);
	GL_CHECK(glEnable(GL_BLEND));
	GL_CHECK(glBlendEquation(GL_FUNC_ADD));
	GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	const GLboolean wasCullFaceEnabled = glIsEnabled(GL_CULL_FACE);
	GL_CHECK(glDisable(GL_CULL_FACE));
	const GLboolean wasDepthEnabled = glIsEnabled(GL_DEPTH_TEST);
	GL_CHECK(glDisable(GL_DEPTH_TEST));
	const GLboolean wasScissorDisabled = glIsEnabled(GL_SCISSOR_TEST);
	GL_CHECK(glEnable(GL_SCISSOR_TEST));
	GLint lastActiveTexture = 0; GL_CHECK(glGetIntegerv(GL_ACTIVE_TEXTURE, &lastActiveTexture));
	GL_CHECK(glActiveTexture(GL_TEXTURE0));

	/* setup program */
	m_Program.SetActiveProgram();
	(void)m_FontAtlasTexture.Bind(0);
	(void)m_Program.SetTexture("FontTexture", m_FontAtlasTexture);
	(void)m_Program.SetMatrix("ProjMtx", orthoMatrix);
	{
		{
			/* fill convert configuration */
			struct nk_convert_config config {};
			static const struct nk_draw_vertex_layout_element vertex_layout[] = {
				{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(FGuiVertex, Position)},
				{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(FGuiVertex, UV)},
				{NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(FGuiVertex, Color)},
				{NK_VERTEX_LAYOUT_END}
			};
			memset(&config, 0, sizeof(config));
			config.vertex_layout = vertex_layout;
			config.vertex_size = sizeof(FGuiVertex);
			config.vertex_alignment = alignof(FGuiVertex);
			config.null = m_NullTexture;
			config.circle_segment_count = 22;
			config.curve_segment_count = 22;
			config.arc_segment_count = 22;
			config.global_alpha = 1.0f;
			config.shape_AA = NK_ANTI_ALIASING_ON;
			config.line_AA = NK_ANTI_ALIASING_ON;

			/* setup buffers to load vertices and elements */
			constexpr nk_size INITIAL_VERTEX_MEM_SIZE = sizeof(FGuiVertex) * 4096;
			constexpr nk_size INITIAL_INDEX_MEM_SIZE = sizeof(nk_draw_index) * 16384;

			struct nk_buffer vbuf {}, ebuf {};
			nk_buffer_init(&vbuf, &GNuklearAllocator, INITIAL_VERTEX_MEM_SIZE);
			nk_buffer_init(&ebuf, &GNuklearAllocator, INITIAL_INDEX_MEM_SIZE);

			nk_convert(&m_DrawContext, &m_DrawCommands, &vbuf, &ebuf, &config);

			// NOTE This is all a lil bit silly...
			nk_memory_status vbufStatus {}, ebufStatus {};
			nk_buffer_info(&vbufStatus, &vbuf);
			nk_buffer_info(&ebufStatus, &ebuf);

			const int32 numVertices = static_cast<int32>(vbufStatus.allocated / sizeof(FGuiVertex));
			const int32 numIndices = static_cast<int32>(ebufStatus.allocated / sizeof(nk_draw_index));

			m_Vertices.SetData(reinterpret_cast<const FGuiVertex*>(nk_buffer_memory_const(&vbuf)), numVertices);
			m_Indices.SetData(reinterpret_cast<const nk_draw_index*>(nk_buffer_memory_const(&ebuf)), numIndices);

			nk_buffer_free(&vbuf);
			nk_buffer_free(&ebuf);
		}

		/* iterate over and execute each draw command */
		const struct nk_draw_command* cmd = nullptr;
		const nk_draw_index* offset = nullptr;
		nk_draw_foreach(cmd, &m_DrawContext, &m_DrawCommands)
		{
			if (cmd->elem_count == 0)
			{
				continue;
			}

			const int32 width = windowSize.Width;
			const int32 height = windowSize.Height;
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id)); // TODO Ummm... This might be wrong?

			glScissor((GLint)(cmd->clip_rect.x * renderScale.X),
			          (GLint)((height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) * renderScale.Y),
			          (GLint)(cmd->clip_rect.w * renderScale.X),
			          (GLint)(cmd->clip_rect.h * renderScale.Y));
			GL_CHECK(glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset));
			offset += cmd->elem_count;
		}

		nk_clear(&m_DrawContext);
		nk_buffer_clear(&m_DrawCommands);
	}

	if (wasBlendDisabled) { GL_CHECK(glDisable(GL_BLEND)); }
	if (wasCullFaceEnabled) { GL_CHECK(glEnable(GL_CULL_FACE)); }
	if (wasDepthEnabled) { GL_CHECK(glEnable(GL_DEPTH_TEST)); }
	if (wasScissorDisabled) { GL_CHECK(glDisable(GL_SCISSOR_TEST)); }
	GL_CHECK(glActiveTexture(lastActiveTexture));
}