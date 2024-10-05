find_package(GTest CONFIG REQUIRED)

set(UMBRAL_CORE_LIB_HEADERS
	"Include/JSON/JsonArray.h"
	"Include/JSON/JsonObject.h"
	"Include/JSON/JsonParser.h"
	"Include/JSON/JsonValue.h"
	"Include/JSON/JsonValueType.h"
)

set(UMBRAL_JSON_LIB_SOURCES
	"Source/JSON/JsonArray.cpp"
	"Source/JSON/JsonObject.cpp"
	"Source/JSON/JsonParser.cpp"
	"Source/JSON/JsonScanner.cpp"
	"Source/JSON/JsonScanner.h"
	"Source/JSON/JsonValue.cpp"
)

add_library(UmbralJsonLib STATIC
	${UMBRAL_JSON_LIB_HEADERS}
	${UMBRAL_JSON_LIB_SOURCES}
)

add_library(umbral::json_lib ALIAS UmbralJsonLib)

target_include_directories(UmbralJsonLib
	PUBLIC "Include"
	PRIVATE "Source"
)

target_link_libraries(UmbralJsonLib
	PUBLIC
		ts::ts
		ts::json
		umbral::core_lib
		umbral::parse_lib
)

umbral_set_target_properties(UmbralJsonLib)

if(WITH_TESTING)
	enable_testing()

	add_executable(UmbralJsonLibTests
		"Tests/Main.cpp"
		"Tests/ParseTests.cpp"
	)

	add_executable(umbral::json_lib::tests ALIAS UmbralJsonLibTests)

	target_include_directories(UmbralJsonLibTests
		PRIVATE "Source"
	)

	target_link_libraries(UmbralJsonLibTests
		PUBLIC
			umbral::core_lib
			umbral::json_lib
			GTest::gtest
			#GTest::gtest_main
			#GTest::gmock
			#GTest::gmock_main
	)

	target_compile_definitions(UmbralJsonLib
		PUBLIC -DWITH_TESTING=1
	)

	umbral_copy_libs_post_build(UmbralJsonLibTests)
	umbral_set_target_properties(UmbralJsonLibTests)

	include(GoogleTest)
	gtest_discover_tests(UmbralJsonLibTests)
endif()
