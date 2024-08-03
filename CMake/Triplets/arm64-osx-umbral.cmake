set(VCPKG_CMAKE_SYSTEM_NAME Darwin)
set(VCPKG_OSX_ARCHITECTURES arm64)
set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE static)

if(${PORT} MATCHES "icu|mimalloc|sdl2|spirv-*|zlib")
	set(VCPKG_LIBRARY_LINKAGE static)
else()
	set(VCPKG_LIBRARY_LINKAGE dynamic)
endif()

message(STATUS "[Umbral] Using ${VCPKG_LIBRARY_LINKAGE} linkage for port \"${PORT}\"")