find_package(glslang CONFIG REQUIRED)

set(SHADER_LIB_HEADERS
	"Include/shaderc/env.h"
	"Include/shaderc/shaderc.h"
	"Include/shaderc/shaderc.hpp"
	"Include/shaderc/status.h"
	"Include/shaderc/visibility.h"
	"Include/shaderc_util/args.h"
	"Include/shaderc_util/compiler.h"
	"Include/shaderc_util/counting_includer.h"
	"Include/shaderc_util/exceptions.h"
	"Include/shaderc_util/file_finder.h"
	"Include/shaderc_util/format.h"
	"Include/shaderc_util/io_shaderc.h"
	"Include/shaderc_util/message.h"
	"Include/shaderc_util/mutex.h"
	"Include/shaderc_util/resources.h"
	"Include/shaderc_util/shader_stage.h"
	"Include/shaderc_util/spirv_tools_wrapper.h"
	"Include/shaderc_util/string_piece.h"
	"Include/shaderc_util/universal_unistd.h"
	"Include/shaderc_util/version_profile.h"
	"Include/USL/Conversion.h"
)

set(SHADER_LIB_SOURCES
	"Source/glslc/dependency_info.cpp"
	"Source/glslc/dependency_info.h"
	"Source/glslc/file.cpp"
	"Source/glslc/file.h"
	"Source/glslc/file_compiler.cpp"
	"Source/glslc/file_compiler.h"
	"Source/glslc/file_includer.cpp"
	"Source/glslc/file_includer.h"
	"Source/glslc/resource_parse.cpp"
	"Source/glslc/resource_parse.h"
	"Source/glslc/shader_stage.cpp"
	"Source/glslc/shader_stage.h"
	"Source/shaderc/shaderc.cpp"
	"Source/shaderc/shaderc_private.h"
	"Source/shaderc_util/args.cpp"
	"Source/shaderc_util/compiler.cpp"
	"Source/shaderc_util/file_finder.cpp"
	"Source/shaderc_util/io_shaderc.cpp"
	"Source/shaderc_util/message.cpp"
	"Source/shaderc_util/resources.cpp"
	"Source/shaderc_util/shader_stage.cpp"
	"Source/shaderc_util/spirv_tools_wrapper.cpp"
	"Source/shaderc_util/version_profile.cpp"
	"Source/USL/Conversion.cpp"
)

###############################################################################
# Shader Library
###############################################################################

add_library(UmbralShaderLib STATIC
	${SHADER_LIB_HEADERS}
	${SHADER_LIB_SOURCES}
)
add_library(umbral::shader_lib ALIAS UmbralShaderLib)

target_include_directories(UmbralShaderLib
	PUBLIC
		"Include"
	PRIVATE
		"Source"
		${SPIRV_HEADERS_INCLUDE_DIRS}
)

target_link_libraries(UmbralShaderLib
	PUBLIC
		SPIRV-Headers
		SPIRV-Tools-static
		SPIRV-Tools-opt
		spirv-cross-core
		spirv-cross-glsl
		glslang::glslang
		glslang::glslang-default-resource-limits
		glslang::SPIRV
		glslang::SPVRemapper
		${CMAKE_THREAD_LIBS_INIT}
		umbral::core_lib
)

target_compile_definitions(UmbralShaderLib
	PRIVATE
		ENABLE_HLSL
		SHADERC_IMPLEMENTATION
)

umbral_set_target_properties(UmbralShaderLib)

###############################################################################
# Shader Compiler
###############################################################################

add_executable(UmbralShaderCompiler
	${SHADER_LIB_HEADERS}
	${SHADER_LIB_SOURCES}
	"Source/glslc/main.cpp"
)
add_executable(umbral::shader_compiler ALIAS UmbralShaderCompiler)

target_include_directories(UmbralShaderCompiler
	PUBLIC
		"Include"
	PRIVATE
		"Source"
		${SPIRV_HEADERS_INCLUDE_DIRS}
)

target_link_libraries(UmbralShaderCompiler
	PUBLIC
		glslang::glslang
		glslang::glslang-default-resource-limits
		glslang::SPIRV
		glslang::SPVRemapper
		spirv-cross-core
		spirv-cross-glsl
		SPIRV-Tools-static
		SPIRV-Tools-opt
		${CMAKE_THREAD_LIBS_INIT}
		umbral::core_lib
)

target_compile_definitions(UmbralShaderCompiler
	PRIVATE
		ENABLE_HLSL
		SHADERC_IMPLEMENTATION
)

umbral_set_target_properties(UmbralShaderCompiler)

include(CMake/UmbralCompileShaders.cmake)