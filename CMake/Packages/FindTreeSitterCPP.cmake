# Attempts to find the local tree-sitter-cpp repository

set(TREE_SITTER_CPP_ROOT_DIR "${PROJECT_SOURCE_DIR}/ThirdParty/tree-sitter-cpp" CACHE PATH "")

if(NOT EXISTS "${TREE_SITTER_CPP_ROOT_DIR}/src/parser.c")
	message(FATAL_ERROR "Failed to find tree-sitter-cpp files; did you initialize and update submodules?")
endif()

set(TreeSitterCPP_FOUND ON)

add_library(tree-sitter-cpp STATIC
	"${TREE_SITTER_CPP_ROOT_DIR}/src/parser.c"
	"${TREE_SITTER_CPP_ROOT_DIR}/src/scanner.c"
)
add_library(ts::cpp ALIAS tree-sitter-cpp)
target_include_directories(tree-sitter-cpp PRIVATE "${TREE_SITTER_CPP_ROOT_DIR}/src")

if(NOT MSVC)
	target_compile_options(tree-sitter-cpp PUBLIC -fPIC)
endif()