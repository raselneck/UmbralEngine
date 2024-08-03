#include "Engine/Logging.h"
#include "Engine/SDL/EngineWindowSDL.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/OpenGL/GraphicsDeviceGL.h"
#include "Graphics/OpenGL/SaveBoundResourceScope.h"
#include "Graphics/OpenGL/Texture2DGL.h"
#include "Graphics/OpenGL/UmbralToGL.h"
#include "Graphics/Shader.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexBuffer.h"
#include "HAL/Directory.h"
#include "ImGui/ImGui.h"
#include "ImGui/OpenGL/ImGuiRendererGL.h"
#include "Math/Matrix4.h"
#include "Math/Rectangle.h"

void UImGuiRendererGL::Draw(const FGameTime& gameTime, const ImDrawData* drawData)
{
	static const FVertexDeclaration imguiVertexDeclaration
	{{
		{ UM_OFFSET_OF(ImDrawVert, pos), EVertexElementFormat::Vector2, EVertexElementUsage::Position, 0 },
		{ UM_OFFSET_OF(ImDrawVert, uv ), EVertexElementFormat::Vector2, EVertexElementUsage::TextureCoordinate, 0 },
		{ UM_OFFSET_OF(ImDrawVert, col), EVertexElementFormat::Color, EVertexElementUsage::Color, 0 },
	}};

	(void)gameTime;

	SetActiveContext();

	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	const int32 framebufferWidth = static_cast<int32>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
	const int32 framebufferHeight = static_cast<int32>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
	if (framebufferWidth <= 0 || framebufferHeight <= 0)
	{
		return;
	}

	m_GraphicsDevice->UseShaderProgram(m_ShaderProgram);

	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	SetupRenderState(drawData, framebufferWidth, framebufferHeight);

	// Will project scissor/clipping rectangles into framebuffer space
	const ImVec2& clipOffset = drawData->DisplayPos;         // (0,0) unless using multi-viewports
	const ImVec2& clipScale  = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	// Render command lists
	for (const ImDrawList* drawList : drawData->CmdLists)
	{
		m_VertexBuffer->SetData(drawList->VtxBuffer.Data, drawList->VtxBuffer.Size, imguiVertexDeclaration);
		m_IndexBuffer->SetData(drawList->IdxBuffer.Data, drawList->IdxBuffer.Size);

		m_GraphicsDevice->BindVertexBuffer(m_VertexBuffer);
		m_GraphicsDevice->BindIndexBuffer(m_IndexBuffer);

		for (const ImDrawCmd& drawCmd : drawList->CmdBuffer)
		{
			if (drawCmd.UserCallback != nullptr)
			{
				// User callback, registered via ImDrawList::AddCallback()
				// (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
				if (drawCmd.UserCallback == ImDrawCallback_ResetRenderState)
				{
					SetupRenderState(drawData, framebufferWidth, framebufferHeight);
				}
				else
				{
					drawCmd.UserCallback(drawList, &drawCmd);
				}
			}
			else
			{
				// Project scissor/clipping rectangles into framebuffer space
				const ImVec2 clipMin { (drawCmd.ClipRect.x - clipOffset.x) * clipScale.x, (drawCmd.ClipRect.y - clipOffset.y) * clipScale.y };
				const ImVec2 clipMax { (drawCmd.ClipRect.z - clipOffset.x) * clipScale.x, (drawCmd.ClipRect.w - clipOffset.y) * clipScale.y };
				if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
				{
					continue;
				}

				// Apply scissor/clipping rectangle (Y is inverted in OpenGL)
				GL_CHECK(glScissor(static_cast<int32>(clipMin.x),
				                   static_cast<int32>(static_cast<float>(framebufferHeight) - clipMax.y),
				                   static_cast<int32>(clipMax.x - clipMin.x),
				                   static_cast<int32>(clipMax.y - clipMin.y)));

				// Bind texture, Draw
//				GL_CHECK(glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(static_cast<uintptr_t>(pcmd->GetTexID()))));
				UM_ENSURE(drawCmd.GetTexID() == nullptr);
				GL_CHECK(glDrawElements(GL_TRIANGLES,
				                        static_cast<GLsizei>(drawCmd.ElemCount),
				                        sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
				                        reinterpret_cast<void*>(static_cast<uintptr_t>(drawCmd.IdxOffset * sizeof(ImDrawIdx)))));
			}
		}
	}

	// Restore modified GL state
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_stencil_test) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);

	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

ImGuiViewport* UImGuiRendererGL::GetImGuiViewport() const
{
	return m_Window->GetImGuiViewport();
}

void UImGuiRendererGL::NewFrame(const FGameTime& gameTime)
{
	(void)gameTime;

	ImGuiIO& io = ImGui::GetIO();
	if (io.Fonts->IsBuilt() == false && io.Fonts->Build())
	{
		UploadFontAtlasToTexture();
	}
}

void UImGuiRendererGL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_Window = FindAncestorOfType<UEngineWindowSDL>();
	m_GraphicsDevice = FindAncestorOfType<UGraphicsDeviceGL>();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendRendererName = "UmbralEngine";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	// Load the vertex and fragment shaders, then attempt to link them

	const FString vertexShaderPath = FDirectory::GetContentFilePath("Shaders"_sv, "ImGui.vert.spv"_sv);
	TObjectPtr<UShader> vertexShader = m_GraphicsDevice->CreateShader(EShaderType::Vertex);
	if (TErrorOr<void> loadResult = vertexShader->LoadFromFile(vertexShaderPath, EShaderFileType::Binary);
	    loadResult.IsError())
	{
		UM_LOG(Error, "Failed to load ImGui vertex shader \"{}\". Reason: {}", vertexShaderPath, loadResult.GetError().GetMessage());
		UM_ASSERT_NOT_REACHED_MSG("Failed to load ImGui vertex shader");
	}

	const FString fragmentShaderPath = FDirectory::GetContentFilePath("Shaders"_sv, "ImGui.frag.spv"_sv);
	TObjectPtr<UShader> fragmentShader = m_GraphicsDevice->CreateShader(EShaderType::Fragment);
	if (TErrorOr<void> loadResult = fragmentShader->LoadFromFile(fragmentShaderPath, EShaderFileType::Binary);
	    loadResult.IsError())
	{
		UM_LOG(Error, "Failed to load ImGui fragment shader \"{}\". Reason: {}", fragmentShaderPath, loadResult.GetError().GetMessage());
		UM_ASSERT_NOT_REACHED_MSG("Failed to load ImGui fragment shader");
	}

	m_ShaderProgram = m_GraphicsDevice->CreateShaderProgram();
	(void)m_ShaderProgram->AttachShader(vertexShader);
	(void)m_ShaderProgram->AttachShader(fragmentShader);
	if (TErrorOr<void> linkResult = m_ShaderProgram->Link();
	    linkResult.IsError())
	{
		UM_LOG(Error, "Failed to link ImGui shaders. Reason: {}", linkResult.GetError().GetMessage());
		UM_ASSERT_NOT_REACHED_MSG("Failed to link ImGui shaders");
	}

	// Create the font texture

	m_FontTexture = m_GraphicsDevice->CreateTexture2D();
	m_FontTexture->SetSamplerState(ESamplerState::LinearClamp);
	// TODO If doing this, need to fix assert for no draw command texture in UImGuiRendererGL::Draw
	//io.Fonts->TexID = reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(CastChecked<UTexture2DGL>(m_FontTexture)->GetTextureHandle()));
	UploadFontAtlasToTexture();

	// Create the vertex and index buffers

	m_VertexBuffer = m_GraphicsDevice->CreateVertexBuffer(EVertexBufferUsage::Dynamic);
	m_IndexBuffer = m_GraphicsDevice->CreateIndexBuffer(EIndexBufferUsage::Dynamic);
}

void UImGuiRendererGL::Destroyed()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->SetTexID(nullptr);

	Super::Destroyed();
}

void UImGuiRendererGL::SetupRenderState(const ImDrawData* drawData, int32 framebufferWidth, int32 framebufferHeight)
{
	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_SCISSOR_TEST);

	// Setup viewport, orthographic projection matrix
	// Our visible ImGui space lies from drawData->DisplayPos (top left) to drawData->DisplayPos+data_data->DisplaySize (bottom right)
	// DisplayPos is (0,0) for single viewport apps.
	GL_CHECK(glViewport(0, 0, framebufferWidth, framebufferHeight));

	const FMatrix4 orthoMatrix = FMatrix4::CreateOrthographicOffCenter(
		drawData->DisplayPos.x,
		drawData->DisplayPos.x + drawData->DisplaySize.x,
		drawData->DisplayPos.y + drawData->DisplaySize.y,
		drawData->DisplayPos.y
	);

	m_GraphicsDevice->UseShaderProgram(m_ShaderProgram);
	UM_ENSURE(m_ShaderProgram->SetTexture2D("fontTexture"_sv, m_FontTexture));
	UM_ENSURE(m_ShaderProgram->SetMatrix4("projectionMatrix"_sv, orthoMatrix));
}

void UImGuiRendererGL::UploadFontAtlasToTexture()
{
	ImGuiIO& io = ImGui::GetIO();

	uint8* pixels = nullptr;
	int32 width = 0, height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	if (width > 0 && height > 0)
	{
		m_FontTexture->SetData(width, height, pixels, ETextureFormat::R8G8B8A8_UNORM, EGenerateMipMaps::No);
	}
}