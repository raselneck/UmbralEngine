find_package(GTest CONFIG REQUIRED)

set(UMBRAL_PARSE_LIB_HEADERS
	"Include/Parsing/Parser.h"
	"Include/Parsing/ParseError.h"
	"Include/Parsing/Scanner.h"
	"Include/Parsing/SourceLocation.h"
	"Include/Parsing/Token.h"
	"Include/Parsing/TokenType.h"
)

set(UMBRAL_PARSE_LIB_SOURCES
	"Source/Parsing/Parser.cpp"
	"Source/Parsing/Scanner.cpp"
	"Source/Parsing/SourceLocation.cpp"
	"Source/Parsing/Token.cpp"
)

add_library(UmbralParseLib STATIC
	${UMBRAL_PARSE_LIB_HEADERS}
	${UMBRAL_PARSE_LIB_SOURCES}
)

add_library(umbral::parse_lib ALIAS UmbralParseLib)

target_include_directories(UmbralParseLib
	PUBLIC "Include"
	PRIVATE "Source"
)

target_link_libraries(UmbralParseLib
	PUBLIC
		umbral::core_lib
)

umbral_copy_libs_post_build(UmbralParseLib)
umbral_set_target_properties(UmbralParseLib)

if(WITH_TESTING)
	enable_testing()

	add_executable(UmbralParseLibTests
		"Tests/Main.cpp"
		"Tests/ParseTests.cpp"
		"Tests/ScannerTests.cpp"
	)

	add_executable(umbral::parse_lib::tests ALIAS UmbralParseLibTests)

	target_include_directories(UmbralParseLibTests
		PRIVATE "Source"
	)

	target_link_libraries(UmbralParseLibTests
		PUBLIC
			umbral::core_lib
			umbral::parse_lib
			GTest::gtest
	)

	target_compile_definitions(UmbralParseLib
		PUBLIC -DWITH_TESTING=1
	)

	umbral_copy_libs_post_build(UmbralParseLibTests)
	umbral_set_target_properties(UmbralParseLibTests)

	include(GoogleTest)
	gtest_discover_tests(UmbralParseLibTests)
endif()
