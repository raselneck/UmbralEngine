#
# This is an implementation of the Lox language, which is a part of the Crafting Interpreters book.
# https://craftinginterpreters.com/
#

add_executable(Lox
	"Include/Lox/Error.h"
	"Include/Lox/Expressions.h"
	"Include/Lox/Parser.h"
	"Include/Lox/Scanner.h"
	"Include/Lox/Scope.h"
	"Include/Lox/SourceLocation.h"
	"Include/Lox/Statements.h"
	"Include/Lox/Token.h"
	"Include/Lox/TokenType.h"
	"Source/Expressions.cpp"
	"Source/Main.cpp"
	"Source/Parser.cpp"
	"Source/Scanner.cpp"
	"Source/Scope.cpp"
	"Source/SourceLocation.cpp"
	"Source/Statements.cpp"
	"Source/Token.cpp"
	"Source/Value.cpp"
)
add_executable(umbral::lox ALIAS Lox)

target_include_directories(Lox
	PUBLIC "Include"
	PRIVATE "Source"
)

target_link_libraries(Lox
	PUBLIC
		umbral::core_lib
)

umbral_copy_libs_post_build(Lox)
umbral_set_target_properties(Lox)