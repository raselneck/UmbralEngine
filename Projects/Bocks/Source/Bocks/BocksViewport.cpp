#include "Bocks/BocksViewport.h"
#include "Engine/ContentManager.h"
#include "Engine/Engine.h"
#include "Engine/EngineWindow.h"
#include "Engine/Logging.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/Image.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/Vertex.h"
#include "Graphics/VertexBuffer.h"
#include "ImGui/ImGui.h"
#include "HAL/Directory.h"
#include "HAL/Path.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#define MESH_POSITION_COLOR             1
#define MESH_POSITION_COLOR_NORMAL      2
#define MESH_POSITION_TEXTURE           3
#define MESH_POSITION_TEXTURE_NORMAL    4
#define MESH_FROM_CERBERUS_MESH         5
#define MESH_MODE                       MESH_FROM_CERBERUS_MESH

void UBocksViewport::Draw(const FGameTime& gameTime)
{
	Super::Draw(gameTime);

	if (m_VertexBuffer.IsNull())
	{
		return;
	}

	const TObjectPtr<UGraphicsDevice> graphicsDevice = GetGraphicsDevice();
	graphicsDevice->Clear(ELinearColor::Eigengrau, 1.0f);
	graphicsDevice->UseShaderProgram(m_Program);

	if (m_IndexBuffer.IsValid())
	{
		graphicsDevice->BindIndexBuffer(m_IndexBuffer);
		graphicsDevice->BindVertexBuffer(m_VertexBuffer);
		graphicsDevice->DrawIndexedVertices(EPrimitiveType::TriangleList);
	}
	else
	{
		graphicsDevice->BindVertexBuffer(m_VertexBuffer);
		graphicsDevice->DrawVertices(EPrimitiveType::TriangleList);
	}

	ImGui::ShowDemoWindow(&m_ShowDemoWindow);
}

void UBocksViewport::Update(const FGameTime& gameTime)
{
	Super::Update(gameTime);

	if (WasKeyPressed(EKey::Escape))
	{
		GetWindow()->Close();
		return;
	}

	if (WasKeyPressed(EKey::V))
	{
		TObjectPtr<UEngine> engine = GetEngine();
		(void)engine->CreateViewport<UBocksViewport>("Child Viewport"_s, 1280, 720);
	}

	const float totalTime = gameTime.GetTotalSeconds();
#if MESH_MODE == MESH_FROM_CERBERUS_MESH
	const FMatrix4 worldMatrix = FMatrix4::CreateTranslation(0.0f, 65.0f, 1.0f) *
	                             FMatrix4::CreateFromAxisAngle(FVector3::Up + FVector3::Right, totalTime) *
	                             FMatrix4::CreateScale(0.01f, 0.01f, 0.01f);
#else
	const FMatrix4 worldMatrix = FMatrix4::CreateScale(0.01f, 0.01f, 0.01f) * FMatrix4::CreateFromAxisAngle(FVector3::Up + FVector3::Right, totalTime * 0.8f);
#endif
	(void)m_Program->SetMatrix4("worldMatrix"_sv, worldMatrix);
}

void UBocksViewport::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	const TObjectPtr<UGraphicsDevice> graphicsDevice = GetGraphicsDevice();
	UM_ENSURE(graphicsDevice.IsValid());

#if MESH_MODE == MESH_POSITION_COLOR
	InitializePositionColorShaderProgram(graphicsDevice);
	InitializePositionColorCube(graphicsDevice);
#elif MESH_MODE == MESH_POSITION_TEXTURE
	InitializePositionTextureShaderProgram(graphicsDevice);
	InitializePositionTextureCube(graphicsDevice);
#elif MESH_MODE == MESH_FROM_CERBERUS_MESH
	TObjectPtr<UContentManager> contentManager = GetContentManager();
	TObjectPtr<UStaticMesh> cerberusMesh = contentManager->Load<UStaticMesh>("Cerberus/Cerberus_LP.FBX"_sv);
	if (cerberusMesh.IsNull())
	{
		UM_LOG(Fatal, "Failed to load Cerberus mesh");
	}

	m_VertexBuffer = cerberusMesh->GetVertexBuffer();
	m_IndexBuffer = cerberusMesh->GetIndexBuffer();

	TObjectPtr<UShader> vertexShader = graphicsDevice->CreateShader(EShaderType::Vertex);
	const FString vertexShaderPath = FDirectory::GetContentFilePath("Shaders"_sv, "VertexPositionNormalTexture.vert.spv"_sv);
	if (TErrorOr<void> loadResult = vertexShader->LoadFromFile(vertexShaderPath, EShaderFileType::Binary);
	    loadResult.IsError())
	{
		UM_LOG(Fatal, "Failed to load vertex shader \"{}\". Reason: {}", FPath::GetBaseFileName(vertexShaderPath), loadResult.GetError().GetMessage());
	}

	TObjectPtr<UShader> fragmentShader = graphicsDevice->CreateShader(EShaderType::Fragment);
	const FString fragmentShaderPath = FDirectory::GetContentFilePath("Shaders"_sv, "VertexPositionNormalTexture.frag.spv"_sv);
	if (TErrorOr<void> loadResult = fragmentShader->LoadFromFile(fragmentShaderPath, EShaderFileType::Binary);
	    loadResult.IsError())
	{
		UM_LOG(Fatal, "Failed to load fragment shader \"{}\". Reason: {}", FPath::GetBaseFileName(fragmentShaderPath), loadResult.GetError().GetMessage());
	}

	m_Program = graphicsDevice->CreateShaderProgram();
	(void)m_Program->AttachShader(MoveTemp(vertexShader));
	(void)m_Program->AttachShader(MoveTemp(fragmentShader));

	if (TErrorOr<void> linkResult = m_Program->Link();
	    linkResult.IsError())
	{
		UM_LOG(Fatal, "Failed to link shaders. Reason: {}", linkResult.GetError().GetMessage());
	}

	FImage textureImage;
	const FString texturePath = FDirectory::GetContentFilePath("Cerberus"_sv, "Textures"_sv, "Cerberus_A.png"_sv);
	if (TErrorOr<void> loadResult = textureImage.LoadFromFile(texturePath);
	    loadResult.IsError())
	{
		UM_LOG(Fatal, "Failed to load texture \"{}\". Reason: {}", FPath::GetBaseFileName(texturePath), loadResult.GetError().GetMessage());
	}

	m_Texture = graphicsDevice->CreateTexture2D();
	m_Texture->SetDataFromImage(textureImage, EGenerateMipMaps::Yes);
	(void)m_Program->SetTexture2D("diffuseTexture"_sv, m_Texture);
#endif

	UM_ENSURE(m_Program.IsValid());

	const FMatrix4 projectionMatrix = FMatrix4::CreatePerspectiveFieldOfView(FMath::ToRadians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f);
	const FMatrix4 viewMatrix = FMatrix4::CreateLookAt({ 0.0f, 0.0f, -0.45f },
	                                                   FVector3::Zero,
	                                                   FVector3::Up);

	UM_ENSURE(m_Program->SetMatrix4("projectionMatrix"_sv, projectionMatrix));
	UM_ENSURE(m_Program->SetMatrix4("viewMatrix"_sv, viewMatrix));
	UM_ENSURE(m_Program->SetMatrix4("worldMatrix"_sv, FMatrix4::Identity));
}

TErrorOr<void> UBocksViewport::InitializePositionColorCube(const TObjectPtr<UGraphicsDevice>& graphicsDevice)
{
#define VPC(x, y, z, color) FVertexPositionColor { FVector3 { (x), (y), (z) }, (color) }
	const TStaticArray<FVertexPositionColor, 8> vertices
	{{
		VPC(-0.5f, -0.5f, -0.5f, EColor::Red),      // 0 left bottom front
		VPC(-0.5f, -0.5f, 0.5f, EColor::Blue),      // 1 left bottom back
		VPC(-0.5f, 0.5f, -0.5f, EColor::Green),     // 2 left top front
		VPC(-0.5f, 0.5f, 0.5f, EColor::Yellow),     // 3 left top back
		VPC(0.5f, -0.5f, -0.5f, EColor::Magenta),   // 4 right bottom front
		VPC(0.5f, -0.5f, 0.5f, EColor::Cyan),       // 5 right bottom back
		VPC(0.5f, 0.5f, -0.5f, EColor::Brown),      // 6 right top front
		VPC(0.5f, 0.5f, 0.5f, EColor::White),       // 7 right top back
	}};
#undef VPC

	const TStaticArray<uint16, 36> indices
	{{ // Front face
		2, 6, 4,
		4, 0, 2,
		// Right face
		6, 7, 5,
		5, 4, 6,
		// Back face
		7, 3, 1,
		1, 5, 7,
		// Left face
		3, 2, 0,
		0, 1, 3,
		// Top face
		3, 7, 6,
		6, 2, 3,
		// Bottom face
		0, 4, 5,
		5, 1, 0
	}};

	m_VertexBuffer = graphicsDevice->CreateVertexBuffer(EVertexBufferUsage::Static);
	m_VertexBuffer->SetData(vertices);

	m_IndexBuffer = graphicsDevice->CreateIndexBuffer(EIndexBufferUsage::Static);
	m_IndexBuffer->SetData(indices);

	return {};
}

TErrorOr<void> UBocksViewport::InitializePositionTextureCube(const TObjectPtr<UGraphicsDevice>& graphicsDevice)
{
#define VPT(x, y, z, u, v) FVertexPositionTexture { FVector3 { (x), (y), (z) }, FVector2 { (u), (v) } }
	const TStaticArray<FVertexPositionTexture, 24> vertices
	{{
 		// Each face's vertices are listed as top left, top right, bottom right, bottom left

		// Front face
		VPT(-0.5f,  0.5f, -0.5f, 0.0f, 1.0f),
		VPT( 0.5f,  0.5f, -0.5f, 1.0f, 1.0f),
		VPT( 0.5f, -0.5f, -0.5f, 1.0f, 0.0f),
		VPT(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f),
		// Right face
		VPT( 0.5f,  0.5f, -0.5f, 0.0f, 1.0f),
		VPT( 0.5f,  0.5f,  0.5f, 1.0f, 1.0f),
		VPT( 0.5f, -0.5f,  0.5f, 1.0f, 0.0f),
		VPT( 0.5f, -0.5f, -0.5f, 0.0f, 0.0f),
		// Back face
		VPT( 0.5f,  0.5f,  0.5f, 0.0f, 1.0f),
		VPT(-0.5f,  0.5f,  0.5f, 1.0f, 1.0f),
		VPT(-0.5f, -0.5f,  0.5f, 1.0f, 0.0f),
		VPT( 0.5f, -0.5f,  0.5f, 0.0f, 0.0f),
		// Left face
		VPT(-0.5f,  0.5f,  0.5f, 0.0f, 1.0f),
		VPT(-0.5f,  0.5f, -0.5f, 1.0f, 1.0f),
		VPT(-0.5f, -0.5f, -0.5f, 1.0f, 0.0f),
		VPT(-0.5f, -0.5f,  0.5f, 0.0f, 0.0f),
		// Top face
		VPT(-0.5f,  0.5f,  0.5f, 0.0f, 1.0f),
		VPT( 0.5f,  0.5f,  0.5f, 1.0f, 1.0f),
		VPT( 0.5f,  0.5f, -0.5f, 1.0f, 0.0f),
		VPT(-0.5f,  0.5f, -0.5f, 0.0f, 0.0f),
		// Bottom face
		VPT(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f),
		VPT( 0.5f, -0.5f, -0.5f, 1.0f, 1.0f),
		VPT( 0.5f, -0.5f,  0.5f, 1.0f, 0.0f),
		VPT(-0.5f, -0.5f,  0.5f, 0.0f, 0.0f)
	}};
#undef VPT

	const TStaticArray<uint16, 36> indices
	{{ // Front face
		0, 1, 2,
		2, 3, 0,
		// Right face
		4, 5, 6,
		6, 7, 4,
		// Back face
		8, 9, 10,
		10, 11, 8,
		// Left face
		12, 13, 14,
		14, 15, 12,
		// Top face
		16, 17, 18,
		18, 19, 16,
		// Bottom face
		20, 21, 22,
		22, 23, 20
	}};

	m_VertexBuffer = graphicsDevice->CreateVertexBuffer(EVertexBufferUsage::Static);
	m_VertexBuffer->SetData(vertices);

	m_IndexBuffer = graphicsDevice->CreateIndexBuffer(EIndexBufferUsage::Static);
	m_IndexBuffer->SetData(indices);

	return {};
}

TErrorOr<void> UBocksViewport::InitializePositionColorShaderProgram(const TObjectPtr<UGraphicsDevice>& graphicsDevice)
{
	TObjectPtr<UShader> vertexShader = graphicsDevice->CreateShader(EShaderType::Vertex);
	const FString vertexShaderPath = FDirectory::GetContentFilePath("Shaders"_sv, "VertexPositionColor.vert.spv"_sv);
	if (TErrorOr<void> loadResult = vertexShader->LoadFromFile(vertexShaderPath, EShaderFileType::Binary);
	    loadResult.IsError())
	{
		return MAKE_ERROR("Failed to load vertex shader \"{}\". Reason: {}", FPath::GetBaseFileName(vertexShaderPath), loadResult.GetError().GetMessage());
	}

	TObjectPtr<UShader> fragmentShader = graphicsDevice->CreateShader(EShaderType::Fragment);
	const FString fragmentShaderPath = FDirectory::GetContentFilePath("Shaders"_sv, "VertexPositionColor.frag.spv"_sv);
	if (TErrorOr<void> loadResult = fragmentShader->LoadFromFile(fragmentShaderPath, EShaderFileType::Binary);
	    loadResult.IsError())
	{
		return MAKE_ERROR("Failed to load fragment shader \"{}\". Reason: {}", FPath::GetBaseFileName(fragmentShaderPath), loadResult.GetError().GetMessage());
	}

	m_Program = graphicsDevice->CreateShaderProgram();
	(void)m_Program->AttachShader(MoveTemp(vertexShader));
	(void)m_Program->AttachShader(MoveTemp(fragmentShader));

	if (TErrorOr<void> linkResult = m_Program->Link();
	    linkResult.IsError())
	{
		return MAKE_ERROR("Failed to link Bocks shaders. Reason: {}", linkResult.GetError().GetMessage());
	}

	return {};
}

TErrorOr<void> UBocksViewport::InitializePositionTextureShaderProgram(const TObjectPtr<UGraphicsDevice>& graphicsDevice)
{
	TObjectPtr<UShader> vertexShader = graphicsDevice->CreateShader(EShaderType::Vertex);
	const FString vertexShaderPath = FDirectory::GetContentFilePath("Shaders"_sv, "VertexPositionTexture.vert.spv"_sv);
	if (TErrorOr<void> loadResult = vertexShader->LoadFromFile(vertexShaderPath, EShaderFileType::Binary);
	    loadResult.IsError())
	{
		return MAKE_ERROR("Failed to load vertex shader \"{}\". Reason: {}", FPath::GetBaseFileName(vertexShaderPath), loadResult.GetError().GetMessage());
	}

	TObjectPtr<UShader> fragmentShader = graphicsDevice->CreateShader(EShaderType::Fragment);
	const FString fragmentShaderPath = FDirectory::GetContentFilePath("Shaders"_sv, "VertexPositionTexture.frag.spv"_sv);
	if (TErrorOr<void> loadResult = fragmentShader->LoadFromFile(fragmentShaderPath, EShaderFileType::Binary);
	    loadResult.IsError())
	{
		return MAKE_ERROR("Failed to load fragment shader \"{}\". Reason: {}", FPath::GetBaseFileName(fragmentShaderPath), loadResult.GetError().GetMessage());
	}

	m_Program = graphicsDevice->CreateShaderProgram();
	(void)m_Program->AttachShader(MoveTemp(vertexShader));
	(void)m_Program->AttachShader(MoveTemp(fragmentShader));

	if (TErrorOr<void> linkResult = m_Program->Link();
	    linkResult.IsError())
	{
		return MAKE_ERROR("Failed to link Bocks shaders. Reason: {}", linkResult.GetError().GetMessage());
	}

	FImage textureImage;
	const FString texturePath = FDirectory::GetContentFilePath("Textures"_sv, "Face.png"_sv);
	if (TErrorOr<void> loadResult = textureImage.LoadFromFile(texturePath);
	    loadResult.IsError())
	{
		return MAKE_ERROR("Failed to load texture \"{}\". Reason: {}", FPath::GetBaseFileName(texturePath), loadResult.GetError().GetMessage());
	}

	m_Texture = graphicsDevice->CreateTexture2D();
	m_Texture->SetDataFromImage(textureImage, EGenerateMipMaps::Yes);
	(void)m_Program->SetTexture2D("diffuseTexture"_sv, m_Texture);

	return {};
}