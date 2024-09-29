#
# The Umbral Meta Tool is what scans source files for specific macro tags and then
# generates reflection information based on those tags.
#

set(META_TOOL_HEADERS
	"Include/MetaTool/AttributeInfo.h"
	"Include/MetaTool/ClassInfo.h"
	"Include/MetaTool/EnumInfo.h"
	"Include/MetaTool/EnumUtilities.h"
	"Include/MetaTool/FunctionInfo.h"
	"Include/MetaTool/PropertyInfo.h"
)

set(META_TOOL_SOURCES
	"Source/MetaTool/HeaderFileGenerator.cpp"
	"Source/MetaTool/HeaderFileGenerator.h"
	"Source/MetaTool/HeaderFileParser.cpp"
	"Source/MetaTool/HeaderFileParser.h"
	"Source/MetaTool/MetaMacroNames.h"
	"Source/MetaTool/PropertyInfo.cpp"
	"Source/MetaTool/SourceFileGenerator.cpp"
	"Source/MetaTool/SourceFileGenerator.h"
	"Source/MetaTool/StructInfo.cpp"
	"Source/Main.cpp"
)

add_executable(UmbralMetaTool
	${META_TOOL_HEADERS}
	${META_TOOL_SOURCES}
)
add_executable(umbral::meta_tool ALIAS UmbralMetaTool)

target_include_directories(UmbralMetaTool
	PUBLIC "Include"
	PRIVATE "Source"
)

target_link_libraries(UmbralMetaTool
	PUBLIC
		ts::ts
		ts::cpp
		umbral::core_lib
		umbral::parse_lib
)

target_compile_definitions(UmbralMetaTool
	PRIVATE
		-DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
		-DWITH_MODULE_EMITTED_TYPES=$<BOOL:${WITH_MODULE_EMITTED_TYPES}>
)

umbral_copy_libs_post_build(UmbralMetaTool)
umbral_set_target_properties(UmbralMetaTool)

# The following is based off of https://github.com/fabian-jung/tsmp/blob/main/cmake/enable_reflection.cmake

#
# Adds reflection support to the target with the given name. This will automatically be called when creating targets
# using umbral_add_library, umbral_add_app, and umbral_add_game.
# @param TargetName The name of the target to add reflection support to.
#
function(umbral_enable_reflection TargetName)
	message(STATUS "[Umbral] Requesting to add reflection support for target ${TargetName}")
	add_dependencies(${TargetName} umbral::meta_tool)

	set(TargetReflectionDirectory "${CMAKE_BINARY_DIR}/Intermediate/${TargetName}/Reflection")
	set(TargetReflectionSource "${TargetReflectionDirectory}/${TargetName}.Generated.cpp")

	# Touch the header and source files so that we can be sure they exist
	file(MAKE_DIRECTORY "${TargetReflectionDirectory}")
	if(NOT EXISTS "${TargetReflectionSource}")
		file(TOUCH "${TargetReflectionSource}")
	endif()

	# Print the absolute path to all of the headers to a file
	get_target_property(TargetSourcePaths ${TargetName} SOURCES)
	set(TargetSourceHeaders)
	foreach(SourcePath ${TargetSourcePaths})
		get_filename_component(SourceExtension "${SourcePath}" LAST_EXT)
		if(NOT "${SourceExtension}" STREQUAL ".h")
			continue()
		endif()
		if(NOT IS_ABSOLUTE ${SourcePath})
			set(SourcePath ${CMAKE_CURRENT_LIST_DIR}/${SourcePath})
		endif()
		list(APPEND TargetSourceHeaders ${SourcePath})
	endforeach()
	file(WRITE "${TargetReflectionDirectory}/${TargetName}Headers.txt" "${TargetSourceHeaders}")

	# Define the command that will run the header tool for the target
	add_custom_command(
		OUTPUT ${TargetReflectionSource}
		COMMAND $<TARGET_FILE:UmbralMetaTool> "${TargetName}" "${CMAKE_CURRENT_LIST_DIR}" "${TargetReflectionDirectory}"
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		DEPENDS umbral::meta_tool
		DEPENDS "${TargetSourcePaths}"
		VERBATIM
	)

	# Define the target for the header tool's generated files
	# TODO We might not need to explicitly depend on the header file if the amalgamated source includes them all
	add_custom_target(
		${TargetName}_Reflection
		DEPENDS ${TargetReflectionSource}
		DEPENDS umbral::meta_tool
	)

	# Ensure all of the reflection files are generated before the target and include them when building
	add_dependencies(${TargetName} ${TargetName}_Reflection)
	target_include_directories(${TargetName}
		PUBLIC
			${TargetReflectionDirectory}
	)
	set_property(
		TARGET
			${TargetName}
		APPEND PROPERTY SOURCES
			"${TargetReflectionSource}"
	)
endfunction()