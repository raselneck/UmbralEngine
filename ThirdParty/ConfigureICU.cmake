set(UMBRAL_ICU_VERSION_MAJOR 73 CACHE INTERNAL "")
set(UMBRAL_ICU_VERSION_MINOR 1 CACHE INTERNAL "")
set(UMBRAL_ICU_DATA_ARCHIVE_NAME "icu4c-${UMBRAL_ICU_VERSION_MAJOR}_${UMBRAL_ICU_VERSION_MINOR}-data-bin-l.zip" CACHE INTERNAL "")
set(UMBRAL_ICU_DATA_FILE_NAME "icudt${UMBRAL_ICU_VERSION_MAJOR}l.dat" CACHE INTERNAL "")

#
# @brief Downloads the ICU data ZIP file and extracts it.
#
function(umbral_download_icu_data)
	if(UMBRAL_ICU_IS_DOWNLOADED)
		return()
	endif()

	set(ICU_DATA_FILE_URL "https://github.com/unicode-org/icu/releases/download/release-${UMBRAL_ICU_VERSION_MAJOR}-${UMBRAL_ICU_VERSION_MINOR}/${UMBRAL_ICU_DATA_ARCHIVE_NAME}")

	message(STATUS "Downloading ICU resource archive...")
	file(DOWNLOAD
		"${ICU_DATA_FILE_URL}"
		"${CMAKE_BINARY_DIR}/ICU/${UMBRAL_ICU_DATA_ARCHIVE_NAME}"
		STATUS ICU_DOWNLOAD_STATUS
		EXPECTED_HASH SHA512=41948aecd3eeb907866c2dec532bde55aed03c45e92668ea8d53ca21cd6fb50b0131e118586245e7a0bd7b728c3f619524437d4ab75b360e2d123a8a8b11d584
	)

	list(GET ICU_DOWNLOAD_STATUS 0 ICU_DOWNLOAD_CODE)
	list(GET ICU_DOWNLOAD_STATUS 1 ICU_DOWNLOAD_TEXT)

	if(ICU_DOWNLOAD_CODE EQUAL 0)
		set(UMBRAL_ICU_IS_DOWNLOADED ON CACHE BOOL "")
	else()
		message(WARNING "[Umbral] Failed to download ICU data; reason: ${ICU_DOWNLOAD_TEXT}")
	endif()
endfunction()

#
# @brief Extracts the downloaded ICU data.
#
function(umbral_extract_icu_data)
	if(NOT EXISTS "${CMAKE_BINARY_DIR}/ICU/${UMBRAL_ICU_DATA_ARCHIVE_NAME}")
		message(WARNING "[Umbral] Cannot extract ICU data if it doesn't exist")
		return()
	endif()

	message(STATUS "Extracting ICU common data...")
	file(ARCHIVE_EXTRACT
		INPUT "${CMAKE_BINARY_DIR}/ICU/${UMBRAL_ICU_DATA_ARCHIVE_NAME}"
		DESTINATION "${CMAKE_BINARY_DIR}/ICU/"
	)

	if(EXISTS "${CMAKE_BINARY_DIR}/ICU/${UMBRAL_ICU_DATA_FILE_NAME}")
		set(UMBRAL_ICU_IS_EXTRACTED ON CACHE BOOL "")
	endif()
endfunction()

umbral_download_icu_data()
umbral_extract_icu_data()

#
# @brief Copies necessary ICU data post-build for a target.
# @param TargetName The name of the target.
#
function(umbral_copy_icu_data_post_build TargetName)
	add_custom_command(
		TARGET ${TargetName}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${TargetName}>/ICU"
		COMMAND ${CMAKE_COMMAND} -E copy -t "$<TARGET_FILE_DIR:${TargetName}>/ICU" "${CMAKE_BINARY_DIR}/ICU/${UMBRAL_ICU_DATA_FILE_NAME}"
	)
endfunction()