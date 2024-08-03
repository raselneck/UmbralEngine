# DearImGui files currently based on commit deff9ab509341f264addbd3c8ada533678591905
# https://github.com/mattconte/tlsf/tree/deff9ab509341f264addbd3c8ada533678591905

set(TLSF_INCLUDE_FILES "Include/tlsf.h")
set(TLSF_SOURCE_FILES "Source/tlsf.c")

add_library(tlsf STATIC ${TLSF_INCLUDE_FILES} ${TLSF_SOURCE_FILES})
add_library(mattconte::tlsf ALIAS tlsf)
target_include_directories(tlsf PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>")