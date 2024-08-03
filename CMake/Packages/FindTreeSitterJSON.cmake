# Attempts to find the local tree-sitter-json repository

set(TREE_SITTER_JSON_ROOT_DIR "${PROJECT_SOURCE_DIR}/ThirdParty/tree-sitter-json" CACHE PATH "")

if(NOT EXISTS "${TREE_SITTER_JSON_ROOT_DIR}/src/parser.c")
	message(FATAL_ERROR "Failed to find tree-sitter-json files; did you initialize and update submodules?")
endif()

set(TreeSitterJSON_FOUND ON)

add_library(tree-sitter-json STATIC
	"${TREE_SITTER_JSON_ROOT_DIR}/src/parser.c"
)
add_library(ts::json ALIAS tree-sitter-json)
target_include_directories(tree-sitter-json PRIVATE "${TREE_SITTER_JSON_ROOT_DIR}/src")

if(NOT MSVC)
	target_compile_options(tree-sitter-json PUBLIC -fPIC)
endif()