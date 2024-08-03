# Windows doesn't set the system name
#set(VCPKG_CMAKE_SYSTEM_NAME Windows)
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)

if(${PORT} MATCHES "icu|mimalloc|sdl2|spirv-*|zlib")
	set(VCPKG_LIBRARY_LINKAGE static)
endif()

message(STATUS "[Umbral] Using ${VCPKG_LIBRARY_LINKAGE} linkage for port \"${PORT}\"")