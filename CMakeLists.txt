cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(UmbralEngine
	VERSION 0.0.1
	LANGUAGES C CXX
)

set(CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/CMake/Packages"
	"${CMAKE_MODULE_PATH}"
)

option(WITH_IMGUI "Whether or not to build and include ocornut's Dear ImGui in the build" ON)
option(WITH_TESTING "Whether or not to enable building of tests" ON)

if(WIN32 OR APPLE OR LINUX)
	message(STATUS "Umbral WILL be using SDL")
	set(WITH_SDL2 ON CACHE INTERNAL "Whether or not Umbral will be using SDL" FORCE)
else()
	message(STATUS "Umbral will NOT be using SDL")
	set(WITH_SDL2 OFF CACHE INTERNAL "" FORCE)
endif()

if(APPLE)
	message(STATUS "Umbral WILL be using ANGLE")
	set(WITH_ANGLE ON CACHE INTERNAL "Whether or not Umbral will be using ANGLE" FORCE)
else()
	message(STATUS "Umbral will NOT be using ANGLE")
	set(WITH_ANGLE OFF CACHE INTERNAL "" FORCE)
endif()

# Since we're using PThreads4W(indows), we want to use pthread on other platforms
if(NOT WIN32)
	set(CMAKE_THREAD_PREFER_PTHREAD ON CACHE INTERNAL "" FORCE)
	set(THREADS_PREFER_PTHREAD_FLAG ON CACHE INTERNAL "" FORCE)
endif()

add_subdirectory(CMake)
add_subdirectory(ThirdParty)
add_subdirectory(Engine/CoreLib)
add_subdirectory(Engine/JsonLib)
add_subdirectory(ShaderCompiler)
add_subdirectory(MetaTool)
add_subdirectory(Engine/ObjectLib)
add_subdirectory(Engine/Engine)
add_subdirectory(Projects)