find_package(GTest CONFIG REQUIRED)

set(UMBRAL_OBJECT_LIB_HEADERS
	"Include/Object/Object.h"
	"Include/Object/ObjectCreationContext.h"
	"Include/Object/ObjectHeap.h"
	"Include/Object/ObjectHeapVisitor.h"
	"Include/Object/ObjectPtr.h"
	"Include/Object/WeakObjectPtr.h"
	"Include/Templates/CanVisitReferencedObjects.h"
)

set(UMBRAL_OBJECT_LIB_SOURCES
	"Source/Object/Object.cpp"
	"Source/Object/ObjectCreationContext.cpp"
	"Source/Object/ObjectHeader.cpp"
	"Source/Object/ObjectHeader.h"
	"Source/Object/ObjectHeap.cpp"
	"Source/Object/ObjectHeapBlock.cpp"
	"Source/Object/ObjectHeapBlock.h"
	"Source/Object/ObjectHeapVisitor.cpp"
	"Source/Object/ObjectPtr.cpp"
	"Source/Object/WeakObjectPtr.cpp"
)

add_library(UmbralObjectLib STATIC
	${UMBRAL_OBJECT_LIB_HEADERS}
	${UMBRAL_OBJECT_LIB_SOURCES}
)

add_library(umbral::object_lib ALIAS UmbralObjectLib)

target_include_directories(UmbralObjectLib
	PUBLIC "Include"
	PRIVATE "Source"
)

target_link_libraries(UmbralObjectLib
	PUBLIC
		umbral::core_lib
)

umbral_copy_libs_post_build(UmbralObjectLib)
umbral_set_target_properties(UmbralObjectLib)
umbral_enable_reflection(UmbralObjectLib)

if(WITH_TESTING)
	enable_testing()

	add_executable(UmbralObjectLibTests
		"Tests/Main.cpp"
		"Tests/MultipleObjectClasses.cpp"
		"Tests/MultipleObjectClasses.h"
		"Tests/ObjectTests.cpp"
	)

	add_executable(umbral::object_lib::tests ALIAS UmbralObjectLibTests)

	target_include_directories(UmbralObjectLibTests
		PRIVATE "Source"
	)

	target_link_libraries(UmbralObjectLibTests
		PUBLIC
			umbral::core_lib
			umbral::object_lib
			GTest::gtest
	)

	target_compile_definitions(UmbralObjectLib
		PUBLIC -DWITH_TESTING=1
	)

	umbral_copy_libs_post_build(UmbralObjectLibTests)
	umbral_set_target_properties(UmbralObjectLibTests)

	include(GoogleTest)
	gtest_discover_tests(UmbralObjectLibTests)
endif()