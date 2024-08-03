set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/Modules;${CMAKE_MODULE_PATH}")

# Q: Why not find the vcpkg installed packages here?
# A: Well, that's a fun little issue. For some reason, including those packages outside of the CMake list file
#    that they're actually used in doesn't cache any information about them (include directory, library name,
#    etc.) Because of that, we need to put the `find_package` calls for those in the list file where they're used.

include(ConfigureICU.cmake)

include(ConfigureSPIRV-Headers.cmake)
add_subdirectory(SPIRV-Headers)

include(ConfigureSPIRV-Tools.cmake)
add_subdirectory(SPIRV-Tools)

include(ConfigureSPIRV-Cross.cmake)
add_subdirectory(SPIRV-Cross)

add_subdirectory(stb)
add_subdirectory(TinyCThread)
add_subdirectory(TLSF)

find_package(TreeSitter REQUIRED)
find_package(TreeSitterCPP REQUIRED)
find_package(TreeSitterJSON REQUIRED)