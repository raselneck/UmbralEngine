#
# @brief Compiles a shader and adds it as a dependency to the given target.
# @param TargetName The name of the target we're compiling shaders for.
#
# @remarks Based on https://stackoverflow.com/a/60472877
#
function(umbral_compile_shaders TargetName TargetDirectory)
	cmake_parse_arguments(PARSE_ARGV 2 arg "" "ENV;FORMAT" "SOURCES")

	# Make the output directory if it does not exist
	if(NOT EXISTS "${TargetDirectory}")
		file(MAKE_DIRECTORY "${TargetDirectory}")
	endif()

	# Create our intermediate directory
	# TODO Use the existing build (not output) directory for the target
	set(IntermediateShaderDir "${CMAKE_BINARY_DIR}/Intermediate/${TargetName}/Shaders")
	file(MAKE_DIRECTORY "${IntermediateShaderDir}")

	foreach(source ${arg_SOURCES})
		get_filename_component(source_NAME "${source}" NAME)
		get_filename_component(source_DIR "${source}" DIRECTORY)

		add_custom_command(
			OUTPUT "${TargetDirectory}/${source_NAME}.spv"
			DEPENDS "${source}"
			DEPFILE "${IntermediateShaderDir}/${source_NAME}.dep"
			COMMAND
			$<TARGET_FILE:UmbralShaderCompiler>
			-I "${source_DIR}"
			"$<$<BOOL:${arg_STD}>:-std=${arg_STD}>"
			"$<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>"
			"$<$<BOOL:${arg_FORMAT}>:-mfmt=${arg_FORMAT}>"
			-MD -MF "${IntermediateShaderDir}/${source_NAME}.dep"
			-o "${TargetDirectory}/${source_NAME}.spv"
			"${source}"
		)
		target_sources(${TargetName} PRIVATE "${TargetDirectory}/${source_NAME}.spv")
	endforeach()

	add_dependencies(${TargetName} UmbralShaderCompiler)
endfunction()