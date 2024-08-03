#
# Determine the current compiler
#
if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
	set(UMBRAL_COMPILER_APPLE_CLANG TRUE CACHE INTERNAL "")
	set(UMBRAL_COMPILER_CLANG TRUE CACHE INTERNAL "")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(UMBRAL_COMPILER_CLANG TRUE CACHE INTERNAL "")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(UMBRAL_COMPILER_GCC TRUE CACHE INTERNAL "")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(UMBRAL_COMPILER_MSVC TRUE CACHE INTERNAL "")
else()
	message(FATAL_ERROR "The current compiler (`${CMAKE_CXX_COMPILER_ID}') is unsupported.")
endif()

#
# @brief Assigns source group values to files in a passed in list
# @remark From https://stackoverflow.com/a/31423421.
#
function(umbral_assign_source_group)
	foreach(Source IN ITEMS ${ARGN})
		if (IS_ABSOLUTE "${Source}")
			file(RELATIVE_PATH RelativeSource "." "${Source}")
		else()
			set(RelativeSource "${Source}")
		endif()
		get_filename_component(SourcePath "${RelativeSource}" PATH)
		string(REPLACE "." "\\" MsvcSourcePath "${SourcePath}")
		source_group("${MsvcSourcePath}" FILES "${Source}")
	endforeach()
endfunction()

#
# @brief Copies necessary DLLs to the build directory of the given target
# @param TargetName The name of the target.
#
function(umbral_copy_libs_post_build TargetName)
	if(WITH_ANGLE AND COMMAND angle_copy_libs_post_build)
		angle_copy_libs_post_build(${TargetName})
	endif()

	umbral_copy_icu_data_post_build(${TargetName})
endfunction()

#
# @brief Sets target properties for a target.
# @param TargetName The name of the target we're configuring.
#
function(umbral_set_target_properties TargetName)
	set_target_properties(${TargetName}
		PROPERTIES
			CXX_EXTENSIONS OFF
			CXX_STANDARD 20
			CXX_STANDARD_REQUIRED ON
	)

	set_property(
		TARGET ${TargetName}
		APPEND PROPERTY SOURCES "${PROJECT_SOURCE_DIR}/Umbral.natvis"
	)

	if(UMBRAL_COMPILER_MSVC)
		if(MSVC_VERSION GREATER_EQUAL 1914)
			target_compile_options(${TargetName}
				PRIVATE
					"/Zc:__cplusplus"
					# TODO Re-enable the conformant preprocessor when BX/BGFX fixes its no-longer-relevant MSVC workarounds
					#"/Zc:preprocessor"
			)
		endif()

		# https://learn.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level?view=msvc-170
		# https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/c5105?view=msvc-170
		target_compile_options(${TargetName} PRIVATE /W4 /WX /wd5105)
	elseif(UMBRAL_COMPILER_CLANG OR UMBRAL_COMPILER_GCC)
		# There may be more warnings we wish to enable, but this should catch many
		target_compile_options(${TargetName} PRIVATE -Werror -Wall -Wextra -Wpedantic -Wno-missing-braces -Wno-newline-eof)

		# HACK Figure out how to fix the variadic macro expansion including a comma when just logging a string
		if(NOT UMBRAL_COMPILER_GCC)
			target_compile_options(${TargetName} PRIVATE -Wno-gnu-zero-variadic-macro-arguments)
		endif()
	endif()
endfunction()

# TODO Define umbral_add_library

#
# @brief Adds an executable that links to the Umbral engine.
# @param TargetName The name of the target.
# @param HEADERS The header files to reference when compiling the executable.
# @param SOURCES The source files to reference when compiling the executable.
#
function(umbral_add_executable TargetName)
	# FIXME Make this function work. In theory we should be able to do something like:
	#		umbral_add_executable(Bocks
	#			SOURCES
	#				"Source/Main.cpp"
	#		)
	#		add_executable(umbral::bocks ALIAS Bocks)
	# However, it doesn't seem like the SOURCES variable is being properly passed in
	message(FATAL_ERROR "-- [Umbral] umbral_add_executable does not currently work :(")

	cmake_parse_arguments(arg "" "" "HEADERS;SOURCES")

	message("[${TargetName}] HEADERS = \"${arg_HEADERS}\"")
	message("[${TargetName}] SOURCES = \"${arg_SOURCES}\"")
	message("[${TargetName}] UNPARSED_ARGUMENTS = \"${arg_UNPARSED_ARGUMENTS}\"")
	message("[${TargetName}] KEYWORDS_MISSING_VALUES = \"${arg_KEYWORDS_MISSING_VALUES}\"")

	add_executable(${TargetName} ${arg_HEADERS} ${arg_SOURCES})
	target_link_libraries(${TargetName} PUBLIC umbral::engine)

	umbral_set_target_properties(${TargetName})
	umbral_enable_reflection(${TargetName})

	shaderc_copy_lib_on_build(${TargetName})
endfunction()

# TODO Do we need to differentiate between games and executables? If so, then define umbral_add_game