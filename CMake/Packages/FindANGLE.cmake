# Attempts to find Google's ANGLE

if(NOT DepotTools_FOUND)
	message(FATAL_ERROR "ANGLE depends on Google's Depot Tools")
endif()

set(ANGLE_PATH "${PROJECT_SOURCE_DIR}/../ANGLE" CACHE PATH "The path to an existing instance of Google's ANGLE, or where to download it")
set(ANGLE_OUTPUT_DIR "out/Debug" CACHE STRING "The output directory local to ANGLE_PATH where ANGLE was built")
set(ANGLE_ENABLE_DOWNLOAD OFF CACHE BOOL "Whether or not to download Google's ANGLE if it was not found")

if(WIN32)
	set(ANGLE_LIB_EXTENSION_TO_COPY ".dll" CACHE INTERNAL "")
	set(ANGLE_LIB_EXTENSION_TO_LINK ".dll.lib" CACHE INTERNAL "")
	set(ANGLE_LIB_NAMES "libEGL;libGLESv2;libc++;third_party_abseil-cpp_absl;third_party_zlib" CACHE INTERNAL "")
elseif(APPLE)
	set(ANGLE_LIB_EXTENSION_TO_COPY ".dylib" CACHE INTERNAL "")
	set(ANGLE_LIB_EXTENSION_TO_LINK ".dylib" CACHE INTERNAL "")
	set(ANGLE_LIB_NAMES "libEGL;libGLESv2" CACHE INTERNAL "")
else()
	message(FATAL_ERROR "Don't know the expected library extensions for ANGLE")
	message(FATAL_ERROR "Don't know the expected library names for ANGLE")
endif()

mark_as_advanced(ANGLE_LIB_NAMES)

function(angle_download)
	message(FATAL_ERROR "TODO: Need to fetch ANGLE using Google's Depot Tools to `${ANGLE_PATH}'")
endfunction()

function(angle_configure_and_build)
	message(FATAL_ERROR "TODO: Need to configure and build ANGLE to `${ANGLE_PATH}/${ANGLE_OUTPUT_DIR}'")
endfunction()

function(angle_validate)
	# Validate that all of our libraries exist
	foreach(LIB_NAME ${ANGLE_LIB_NAMES})
		set(LIB_PATH "${ANGLE_PATH}/${ANGLE_OUTPUT_DIR}/${LIB_NAME}${ANGLE_LIB_EXTENSION_TO_COPY}")
		if(NOT EXISTS "${LIB_PATH}")
			message(WARNING "Failed to find ANGLE library ${ANGLE_PATH}/${ANGLE_OUTPUT_DIR}/${LIB_NAME}${ANGLE_LIB_EXTENSION_TO_COPY}")
			set(ANGLE_LIBEGL_FOUND OFF PARENT_SCOPE)
			set(ANGLE_LIBGLESV2_FOUND OFF PARENT_SCOPE)
			return()
		endif()
	endforeach()

	set(ANGLE_LIBEGL_FOUND ON PARENT_SCOPE)
	set(ANGLE_LIBEGL_TO_LINK_PATH "${ANGLE_PATH}/${ANGLE_OUTPUT_DIR}/libEGL${ANGLE_LIB_EXTENSION_TO_LINK}" CACHE PATH "")

	set(ANGLE_LIBGLESV2_FOUND ON PARENT_SCOPE)
	set(ANGLE_LIBGLESV2_TO_LINK_PATH "${ANGLE_PATH}/${ANGLE_OUTPUT_DIR}/libGLESv2${ANGLE_LIB_EXTENSION_TO_LINK}" CACHE PATH "")

	if(EXISTS "${ANGLE_PATH}/include/GLES3/gl31.h")
		set(ANGLE_GL31_HEADER_FOUND ON PARENT_SCOPE)
		set(ANGLE_INCLUDE_DIR "${ANGLE_PATH}/include" CACHE PATH "")
	else()
		set(ANGLE_GL31_HEADER_FOUND OFF PARENT_SCOPE)
	endif()
endfunction()

if(NOT EXISTS "${ANGLE_PATH}" AND ${ANGLE_ENABLE_DOWNLOAD})
	angle_download()
	angle_configure_and_build()
	angle_validate()
elseif(EXISTS "${ANGLE_PATH}")
	if(NOT EXISTS "${ANGLE_PATH}/${ANGLE_OUTPUT_DIR}")
		angle_configure_and_build()
	endif()
	angle_validate()
endif()

if(ANGLE_LIBEGL_FOUND AND ANGLE_LIBGLESV2_FOUND AND ANGLE_GL31_HEADER_FOUND)
	set(ANGLE_FOUND ON CACHE BOOL "")

	add_library(google_angle INTERFACE)
	add_library(google::angle ALIAS google_angle)
	target_include_directories(google_angle INTERFACE "${ANGLE_INCLUDE_DIR}")
	target_link_libraries(google_angle INTERFACE "${ANGLE_LIBEGL_TO_LINK_PATH}" "${ANGLE_LIBGLESV2_TO_LINK_PATH}")

	function(angle_copy_libs_post_build TargetName)
		foreach(LIB_NAME ${ANGLE_LIB_NAMES})
			add_custom_command(TARGET ${TargetName}
				POST_BUILD
				COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${ANGLE_PATH}/${ANGLE_OUTPUT_DIR}/${LIB_NAME}${ANGLE_LIB_EXTENSION_TO_COPY}" "$<TARGET_FILE_DIR:${TargetName}>"
				COMMENT [${TargetName}] Copying ${LIB_NAME}${ANGLE_IB_EXTENSION_TO_COPY}...
			)
		endforeach()
	endfunction()
else()
	set(ANGLE_FOUND OFF CACHE BOOL "")
endif()

if (${ANGLE_FOUND})
	message(STATUS "Found ANGLE at \"${ANGLE_PATH}\"")
else()
	message(WARNING "Failed to find ANGLE")
endif()