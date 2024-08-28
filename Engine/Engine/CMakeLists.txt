set(UMBRAL_VERSION_MAJOR 0 CACHE INTERNAL "")
set(UMBRAL_VERSION_MINOR 1 CACHE INTERNAL "")
set(UMBRAL_VERSION_PATCH 0 CACHE INTERNAL "")

find_package(assimp CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(freeimage CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Vulkan 1.3.216 REQUIRED)

# Ensure we found glslc when searching for Vulkan
if("${Vulkan_GLSLC_EXECUTABLE}" STREQUAL "")
	message(FATAL_ERROR "Failed to find glslc executable")
endif()

set(UMBRAL_HEADER_FILES
	"Include/Engine/EditorEngine.h"
	"Include/Engine/Engine.h"
	"Include/Engine/EngineViewport.h"
	"Include/Engine/EngineWindow.h"
	"Include/Engine/GameEngine.h"
	"Include/Engine/GameTime.h"
	"Include/Engine/GameViewport.h"
	"Include/Engine/Module.h"
	"Include/Engine/ModuleManager.h"
	"Include/Graphics/BlendFunction.h"
	"Include/Graphics/BlendMode.h"
	"Include/Graphics/BlendState.h"
	"Include/Graphics/ClearOptions.h"
	"Include/Graphics/ColorWriteChannels.h"
	"Include/Graphics/CompareFunction.h"
	"Include/Graphics/CullMode.h"
	"Include/Graphics/DepthFormat.h"
	"Include/Graphics/DepthStencilState.h"
	"Include/Graphics/FillMode.h"
	"Include/Graphics/GraphicsApi.h"
	"Include/Graphics/GraphicsDevice.h"
	"Include/Graphics/GraphicsContextState.h"
	"Include/Graphics/GraphicsResource.h"
	"Include/Graphics/IndexBuffer.h"
	"Include/Graphics/IndexBufferUsage.h"
	"Include/Graphics/IndexElementType.h"
	"Include/Graphics/PrimitiveType.h"
	"Include/Graphics/RasterizerState.h"
	"Include/Graphics/Shader.h"
	"Include/Graphics/ShaderProgram.h"
	"Include/Graphics/ShaderType.h"
	"Include/Graphics/StencilOperation.h"
	"Include/Graphics/Texture.h"
	"Include/Graphics/TextureAddressMode.h"
	"Include/Graphics/TextureFilter.h"
	"Include/Graphics/TextureFormat.h"
	"Include/Graphics/Vertex.h"
	"Include/Graphics/VertexBuffer.h"
	"Include/Graphics/VertexBufferUsage.h"
	"Include/Graphics/VertexDeclaration.h"
	"Include/Input/ButtonState.h"
	"Include/Input/GamePad.h"
	"Include/Input/GamePadAxis.h"
	"Include/Input/GamePadButton.h"
	"Include/Input/Key.h"
	"Include/Input/Keyboard.h"
	"Include/Input/Mouse.h"
	"Include/Input/MouseButton.h"
)

set(UMBRAL_SOURCE_FILES
	"Source/Engine/Application.cpp"
	"Source/Engine/Application.h"
	"Source/Engine/EditorEngine.cpp"
	"Source/Engine/Engine.cpp"
	"Source/Engine/EngineLoop.cpp"
	"Source/Engine/EngineLoop.h"
	"Source/Engine/EngineViewport.cpp"
	"Source/Engine/EngineWindow.cpp"
	"Source/Engine/GameEngine.cpp"
	"Source/Engine/GameTime.cpp"
	"Source/Engine/GameViewport.cpp"
	"Source/Engine/Module.cpp"
	"Source/Engine/ModuleManager.cpp"
	"Source/Engine/VideoDisplay.h"
	"Source/Graphics/GraphicsDevice.cpp"
	"Source/Graphics/GraphicsResource.cpp"
	"Source/Graphics/IndexBuffer.cpp"
	"Source/Graphics/Shader.cpp"
	"Source/Graphics/ShaderProgram.cpp"
	"Source/Graphics/SwapChain.cpp"
	"Source/Graphics/SwapChain.h"
	"Source/Graphics/Texture.cpp"
	"Source/Graphics/Vertex.cpp"
	"Source/Graphics/VertexBuffer.cpp"
	"Source/Graphics/VertexDeclaration.cpp"
	"Source/Input/GamePad.cpp"
	"Source/Input/InputManager.cpp"
	"Source/Input/InputManager.h"
	"Source/Input/Keyboard.cpp"
	"Source/Input/Mouse.cpp"
	"Source/Main/Main.cpp"
)

set(UMBRAL_GLAD_FILES
	"ThirdParty/glad/Include/glad/glad.h"
	"ThirdParty/glad/Include/KHR/khrplatform.h"
	"ThirdParty/glad/Source/glad.cpp"
)

set(UMBRAL_SDL_FILES
	"Source/Engine/SDL/ApplicationSDL.cpp"
	"Source/Engine/SDL/ApplicationSDL.h"
	"Source/Engine/SDL/EngineLoopSDL.cpp"
	"Source/Engine/SDL/EngineLoopSDL.h"
	"Source/Engine/SDL/EngineWindowSDL.cpp"
	"Source/Engine/SDL/EngineWindowSDL.h"
	"Source/Engine/SDL/VideoDisplaySDL.cpp"
	"Source/Engine/SDL/VideoDisplaySDL.h"
	"Source/Input/SDL/InputManagerSDL.cpp"
	"Source/Input/SDL/InputManagerSDL.h"
)

set(UMBRAL_SDL_IMGUI_FILES
	"Source/ImGui/SDL/ImGuiSystemSDL.cpp"
	"Source/ImGui/SDL/ImGuiSystemSDL.h"
)

set(UMBRAL_OPENGL_FILES
	"Source/Graphics/OpenGL/GraphicsDeviceGL.cpp"
	"Source/Graphics/OpenGL/GraphicsDeviceGL.h"
	"Source/Graphics/OpenGL/IndexBufferGL.cpp"
	"Source/Graphics/OpenGL/IndexBufferGL.h"
	"Source/Graphics/OpenGL/SaveBoundResourceScope.h"
	"Source/Graphics/OpenGL/ShaderGL.cpp"
	"Source/Graphics/OpenGL/ShaderGL.h"
	"Source/Graphics/OpenGL/ShaderProgramGL.cpp"
	"Source/Graphics/OpenGL/ShaderProgramGL.h"
	"Source/Graphics/OpenGL/SwapChainGL.cpp"
	"Source/Graphics/OpenGL/SwapChainGL.h"
	"Source/Graphics/OpenGL/Texture2DGL.cpp"
	"Source/Graphics/OpenGL/Texture2DGL.h"
	"Source/Graphics/OpenGL/TextureManagerGL.cpp"
	"Source/Graphics/OpenGL/TextureManagerGL.h"
	"Source/Graphics/OpenGL/UmbralToGL.cpp"
	"Source/Graphics/OpenGL/UmbralToGL.h"
	"Source/Graphics/OpenGL/VertexBufferGL.cpp"
	"Source/Graphics/OpenGL/VertexBufferGL.h"
)

set(UMBRAL_VULKAN_FILES
	"Source/Graphics/Vulkan/GraphicsDeviceVK.cpp"
	"Source/Graphics/Vulkan/GraphicsDeviceVK.h"
	"Source/Graphics/Vulkan/ShaderVK.cpp"
	"Source/Graphics/Vulkan/ShaderVK.h"
	"Source/Graphics/Vulkan/SwapChainVK.cpp"
	"Source/Graphics/Vulkan/SwapChainVK.h"
	"Source/Graphics/Vulkan/UmbralToVK.cpp"
	"Source/Graphics/Vulkan/UmbralToVK.h"
)

set(UMBRAL_BGFX_FILES
	"Source/Graphics/BGFX/AllocatorBGFX.cpp"
	"Source/Graphics/BGFX/AllocatorBGFX.h"
	"Source/Graphics/BGFX/GraphicsDeviceBGFX.cpp"
	"Source/Graphics/BGFX/GraphicsDeviceBGFX.h"
	"Source/Graphics/BGFX/IndexBufferBGFX.cpp"
	"Source/Graphics/BGFX/IndexBufferBGFX.h"
	"Source/Graphics/BGFX/ShaderBGFX.cpp"
	"Source/Graphics/BGFX/ShaderBGFX.h"
	"Source/Graphics/BGFX/ShaderProgramBGFX.cpp"
	"Source/Graphics/BGFX/ShaderProgramBGFX.h"
	"Source/Graphics/BGFX/SwapChainBGFX.cpp"
	"Source/Graphics/BGFX/SwapChainBGFX.h"
	"Source/Graphics/BGFX/UmbralToBGFX.cpp"
	"Source/Graphics/BGFX/UmbralToBGFX.h"
	"Source/Graphics/BGFX/VertexBufferBGFX.cpp"
	"Source/Graphics/BGFX/VertexBufferBGFX.h"
)

set(UMBRAL_IMGUI_FILES
	"Include/ImGui/ImGui.h"
	"Source/ImGui/OpenGL/ImGuiRendererGL.cpp"
	"Source/ImGui/OpenGL/ImGuiRendererGL.h"
	"Source/ImGui/ImGui.cpp"
	"Source/ImGui/ImGuiRenderer.cpp"
	"Source/ImGui/ImGuiRenderer.h"
	"Source/ImGui/ImGuiSystem.cpp"
	"Source/ImGui/ImGuiSystem.h"
	"Source/ImGui/ImGuiViewport.h"
	"ThirdParty/ImGui/Include/imconfig.h"
	"ThirdParty/ImGui/Include/imgui.h"
	"ThirdParty/ImGui/Source/imconfig.cpp"
	"ThirdParty/ImGui/Source/imgui.cpp"
	"ThirdParty/ImGui/Source/imgui_demo.cpp"
	"ThirdParty/ImGui/Source/imgui_draw.cpp"
	"ThirdParty/ImGui/Source/imgui_freetype.cpp"
	"ThirdParty/ImGui/Source/imgui_freetype.h"
	"ThirdParty/ImGui/Source/imgui_internal.h"
	"ThirdParty/ImGui/Source/imgui_tables.cpp"
	"ThirdParty/ImGui/Source/imgui_widgets.cpp"
	"ThirdParty/ImGui/Source/imstb_rectpack.h"
	"ThirdParty/ImGui/Source/imstb_textedit.h"
	"ThirdParty/ImGui/Source/imstb_truetype.h"
)

add_library(UmbralEngine STATIC
	${UMBRAL_HEADER_FILES}
	${UMBRAL_SOURCE_FILES}
	${UMBRAL_OPENGL_FILES}
	${UMBRAL_VULKAN_FILES}
)
add_library(umbral::engine ALIAS UmbralEngine)

if(WITH_ANGLE)
	find_package(DepotTools REQUIRED)
	find_package(ANGLE REQUIRED)

	target_link_libraries(UmbralEngine
		PRIVATE
			google::angle
	)
else()
	set_property(
		TARGET UmbralEngine
		APPEND PROPERTY SOURCES ${UMBRAL_GLAD_FILES}
	)

	target_include_directories(UmbralEngine PRIVATE "ThirdParty/glad/Include")
	target_link_libraries(UmbralEngine PRIVATE OpenGL::GL)
endif()

# Conditionally include the ImGui files
if(WITH_IMGUI)
	set_property(
		TARGET UmbralEngine
		APPEND PROPERTY SOURCES
			${UMBRAL_IMGUI_FILES}
			"${CMAKE_CURRENT_LIST_DIR}/ThirdParty/ImGui/imgui.natvis"
	)

	target_include_directories(UmbralEngine
		PUBLIC
			"ThirdParty/ImGui/Include"
	)
endif()

# Conditionally include the SDL2 files
if(WITH_SDL2)
	find_package(SDL2 CONFIG REQUIRED)

	set_property(
		TARGET UmbralEngine
		APPEND PROPERTY SOURCES ${UMBRAL_SDL_FILES}
	)

	target_link_libraries(UmbralEngine
		PRIVATE
			$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
	)

	if(WITH_IMGUI)
		set_property(
			TARGET UmbralEngine
			APPEND PROPERTY SOURCES ${UMBRAL_SDL_IMGUI_FILES}
		)
	endif()
endif()

target_compile_definitions(UmbralEngine
	PUBLIC
		-DWITH_ANGLE=$<BOOL:${WITH_ANGLE}>
		-DWITH_IMGUI=$<BOOL:${WITH_IMGUI}>
		-DWITH_SDL2=$<BOOL:${WITH_SDL2}>
		-DWITH_MODULE_EMITTED_TYPES=$<BOOL:${WITH_MODULE_EMITTED_TYPES}>
	PRIVATE
		-DUMBRAL_VERSION_MAJOR=${UMBRAL_VERSION_MAJOR}
		-DUMBRAL_VERSION_MINOR=${UMBRAL_VERSION_MINOR}
		-DUMBRAL_VERSION_PATCH=${UMBRAL_VERSION_PATCH}
)

target_include_directories(UmbralEngine
	PUBLIC
		"Include"
	PRIVATE
		"Source"
		"Source/Graphics"
)

target_link_libraries(UmbralEngine
	PRIVATE
		assimp::assimp
		box2d::box2d
		${BULLET_LIBRARIES}
		freeimage::FreeImage
		freeimage::FreeImagePlus
		Freetype::Freetype
		umbral::core_lib
		umbral::object_lib
		umbral::shader_lib
		Vulkan::Vulkan
)

umbral_copy_libs_post_build(UmbralEngine)
umbral_set_target_properties(UmbralEngine)
umbral_enable_reflection(UmbralEngine)

# Build our shaders
set(UMBRAL_SHADER_DIR "${PROJECT_SOURCE_DIR}/EngineContent/Shaders")
umbral_compile_shaders(
	UmbralEngine
	"${UMBRAL_SHADER_DIR}"
	STD 310es
	ENV opengl
	FORMAT bin
	SOURCES
		"${UMBRAL_SHADER_DIR}/ImGui.frag"
		"${UMBRAL_SHADER_DIR}/ImGui.vert"
		"${UMBRAL_SHADER_DIR}/VertexPosition.frag"
		"${UMBRAL_SHADER_DIR}/VertexPosition.vert"
		"${UMBRAL_SHADER_DIR}/VertexPositionColor.frag"
		"${UMBRAL_SHADER_DIR}/VertexPositionColor.vert"
		"${UMBRAL_SHADER_DIR}/VertexPositionTexture.frag"
		"${UMBRAL_SHADER_DIR}/VertexPositionTexture.vert"
)

if(WITH_TESTING)
	enable_testing()

	add_executable(UmbralEngineTests
		"Tests/GoogleTestEngine.cpp"
		"Tests/GoogleTestEngine.h"
		"Tests/Main.cpp"
		"Tests/MetaTests.cpp"
	)

	add_executable(umbral::engine::tests ALIAS UmbralEngineTests)

	target_include_directories(UmbralEngineTests
		PRIVATE "Source"
	)

	target_link_libraries(UmbralEngineTests
		PUBLIC
			umbral::core_lib
			umbral::engine
			GTest::gtest
			#GTest::gtest_main
			#GTest::gmock
			#GTest::gmock_main
	)

	target_compile_definitions(UmbralEngine
		PUBLIC -DWITH_TESTING=1
	)

	umbral_set_target_properties(UmbralEngineTests)
	umbral_enable_reflection(UmbralEngineTests)

	include(GoogleTest)
	gtest_discover_tests(UmbralEngineTests)
endif()