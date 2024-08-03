# Attempts to find the local tree-sitter repository

set(TREE_SITTER_ROOT_DIR "${PROJECT_SOURCE_DIR}/ThirdParty/tree-sitter" CACHE PATH "")

if(NOT EXISTS "${TREE_SITTER_ROOT_DIR}/lib/include/tree_sitter/api.h")
	message(FATAL_ERROR "Failed to find tree-sitter files; did you initialize and update submodules?")
endif()

set(TreeSitter_FOUND ON)
set(TreeSitter_INCLUDE_DIR "${TREE_SITTER_ROOT_DIR}/lib/include")

add_library(tree-sitter STATIC
	"${TREE_SITTER_ROOT_DIR}/lib/include/tree_sitter/api.h"
	"${TREE_SITTER_ROOT_DIR}/lib/include/tree_sitter/parser.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/unicode/ptypes.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/unicode/umachine.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/unicode/urename.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/unicode/utf.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/unicode/utf8.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/unicode/utf16.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/alloc.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/alloc.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/array.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/atomic.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/clock.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/error_costs.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/get_changed_ranges.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/get_changed_ranges.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/host.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/language.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/language.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/length.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/lexer.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/lexer.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/node.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/parser.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/point.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/query.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/reduce_action.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/reusable_node.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/stack.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/stack.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/subtree.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/subtree.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/tree.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/tree.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/tree_cursor.c"
	"${TREE_SITTER_ROOT_DIR}/lib/src/tree_cursor.h"
	"${TREE_SITTER_ROOT_DIR}/lib/src/unicode.h"
)
add_library(ts::ts ALIAS tree-sitter)
target_include_directories(tree-sitter
	PUBLIC
		"${TREE_SITTER_ROOT_DIR}/lib/include"
	PRIVATE
		"${TREE_SITTER_ROOT_DIR}/lib/src"
)

if(NOT MSVC)
	target_compile_options(tree-sitter PUBLIC -fPIC)
endif()