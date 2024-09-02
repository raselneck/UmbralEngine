find_package(CURL REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(ICU REQUIRED COMPONENTS i18n uc data)
find_package(libuv CONFIG REQUIRED)
find_package(libwebsockets CONFIG REQUIRED)
find_package(mimalloc CONFIG REQUIRED)
if(WIN32)
	find_package(PThreads4W REQUIRED)
else()
	find_package(Threads REQUIRED)
endif()

set(UMBRAL_CORE_LIB_HEADERS
	"Include/Containers/Any.h"
	"Include/Containers/Array.h"
	"Include/Containers/Function.h"
	"Include/Containers/HashMap.h"
	"Include/Containers/HashTable.h"
	"Include/Containers/LinkedList.h"
	"Include/Containers/Optional.h"
	"Include/Containers/Pair.h"
	"Include/Containers/Queue.h"
	"Include/Containers/RedBlackTree.h"
	"Include/Containers/Span.h"
	"Include/Containers/Stack.h"
	"Include/Containers/StaticArray.h"
	"Include/Containers/String.h"
	"Include/Containers/StringOrStringView.h"
	"Include/Containers/StringView.h"
	"Include/Containers/Tuple.h"
	"Include/Containers/Variant.h"
	"Include/Engine/Assert.h"
	"Include/Engine/Cast.h"
	"Include/Engine/CommandLine.h"
	"Include/Engine/Console.h"
	"Include/Engine/CoreTypes.h"
	"Include/Engine/Error.h"
	"Include/Engine/Hashing.h"
	"Include/Engine/IntTypes.h"
	"Include/Engine/Logging.h"
	"Include/Engine/MiscMacros.h"
	"Include/Engine/Platform.h"
	"Include/Graphics/Color.h"
	"Include/Graphics/HSV.h"
	"Include/Graphics/Image.h"
	"Include/Graphics/LinearColor.h"
	"Include/HAL/BinaryStreamReader.h"
	"Include/HAL/BinaryStreamWriter.h"
	"Include/HAL/DateTime.h"
	"Include/HAL/Directory.h"
	"Include/HAL/EventLoop.h"
	"Include/HAL/File.h"
	"Include/HAL/FileStream.h"
	"Include/HAL/FileSystem.h"
	"Include/HAL/Path.h"
	"Include/HAL/TextStreamWriter.h"
	"Include/HAL/TimeSpan.h"
	"Include/Main/Main.h"
	"Include/Math/Math.h"
	"Include/Math/Matrix3.h"
	"Include/Math/Matrix4.h"
	"Include/Math/Point.h"
	"Include/Math/Quaternion.h"
	"Include/Math/Rectangle.h"
	"Include/Math/Rotator.h"
	"Include/Math/Size.h"
	"Include/Math/Vector2.h"
	"Include/Math/Vector3.h"
	"Include/Math/Vector4.h"
	"Include/Memory/AlignedStorage.h"
	"Include/Memory/EnabledSharedFromThis.h"
	"Include/Memory/Memory.h"
	"Include/Memory/SharedPtr.h"
	"Include/Memory/SharedResourceBlock.h"
	"Include/Memory/SmallBufferStorage.h"
	"Include/Memory/UniquePtr.h"
	"Include/Memory/WeakPtr.h"
	"Include/Main/Main.h"
	"Include/Meta/ArrayTypeInfo.h"
	"Include/Meta/AttributeInfo.h"
	"Include/Meta/ClassInfo.h"
	"Include/Meta/EnumInfo.h"
	"Include/Meta/FunctionInfo.h"
	"Include/Meta/MetaAttributes.h"
	"Include/Meta/MetaMacros.h"
	"Include/Meta/PointerTypeInfo.h"
	"Include/Meta/PropertyInfo.h"
	"Include/Meta/StructInfo.h"
	"Include/Meta/SubclassOf.h"
	"Include/Meta/TypeInfo.h"
	"Include/Misc/AtExit.h"
	"Include/Misc/Badge.h"
	"Include/Misc/CString.h"
	"Include/Misc/DoOnce.h"
	"Include/Misc/EnumMacros.h"
	"Include/Misc/NumericBase.h"
	"Include/Misc/SourceLocation.h"
	"Include/Misc/StringBuilder.h"
	"Include/Misc/StringFormatting.h"
	"Include/Misc/StringParsing.h"
	"Include/Misc/Unicode.h"
	"Include/Misc/Version.h"
	"Include/Regex/Regex.h"
	"Include/Templates/AndNotOr.h"
	"Include/Templates/AssignIfTrue.h"
	"Include/Templates/BulkOperations.h"
	"Include/Templates/CharTraits.h"
	"Include/Templates/ComparisonTraits.h"
	"Include/Templates/Declval.h"
	"Include/Templates/Decay.h"
	"Include/Templates/Fill.h"
	"Include/Templates/IntegralConstant.h"
	"Include/Templates/IntegerSequence.h"
	"Include/Templates/IsAbstract.h"
	"Include/Templates/IsArray.h"
	"Include/Templates/IsAssignable.h"
	"Include/Templates/IsBaseOf.h"
	"Include/Templates/IsCallable.h"
	"Include/Templates/IsChar.h"
	"Include/Templates/IsClass.h"
	"Include/Templates/IsConstructible.h"
	"Include/Templates/IsConstVolatile.h"
	"Include/Templates/IsConvertible.h"
	"Include/Templates/IsEnum.h"
	"Include/Templates/IsFloat.h"
	"Include/Templates/IsFunction.h"
	"Include/Templates/IsInt.h"
	"Include/Templates/IsHashable.h"
	"Include/Templates/IsMemberPointer.h"
	"Include/Templates/IsMetaType.h"
	"Include/Templates/IsPOD.h"
	"Include/Templates/IsPointer.h"
	"Include/Templates/IsReference.h"
	"Include/Templates/IsSame.h"
	"Include/Templates/IsTypeComplete.h"
	"Include/Templates/IsUnion.h"
	"Include/Templates/IsVoid.h"
	"Include/Templates/IsZeroConstructible.h"
	"Include/Templates/MinMax.h"
	"Include/Templates/NumericLimits.h"
	"Include/Templates/Select.h"
	"Include/Templates/StringTraits.h"
	"Include/Templates/TypeTraits.h"
	"Include/Templates/UnderlyingType.h"
	"Include/Templates/VariadicTraits.h"
	"Include/Threading/AsyncTask.h"
	"Include/Threading/LockGuard.h"
	"Include/Threading/Mutex.h"
	"Include/Threading/Promise.h"
	"Include/Threading/Thread.h"
	"Include/Threading/ThreadPool.h"
	"ThirdParty/tiny-regex-c/Include/re.h"
)

set(UMBRAL_CORE_LIB_SOURCES
	"Source/Containers/Any.cpp"
	"Source/Containers/InternalString.cpp"
	"Source/Containers/InternalString.h"
	"Source/Containers/String.cpp"
	"Source/Containers/StringOrStringView.cpp"
	"Source/Containers/StringView.cpp"
	"Source/Engine/Assert.cpp"
	"Source/Engine/CommandLine.cpp"
	"Source/Engine/Console.cpp"
	"Source/Engine/Hashing.cpp"
	"Source/Engine/InternalLogging.cpp"
	"Source/Engine/InternalLogging.h"
	"Source/Engine/Internationalization.cpp"
	"Source/Engine/Internationalization.h"
	"Source/Engine/Logging/DynamicLoggerInstance.cpp"
	"Source/Engine/Logging/DynamicLoggerInstance.h"
	"Source/Engine/Logging/FileLogListener.cpp"
	"Source/Engine/Logging/FileLogListener.h"
	"Source/Engine/Logging/LogListener.h"
	"Source/Engine/Logging/StdLogListener.cpp"
	"Source/Engine/Logging/StdLogListener.h"
	"Source/Engine/Logging.cpp"
	"Source/Graphics/Color.cpp"
	"Source/Graphics/HSV.cpp"
	"Source/Graphics/Image.cpp"
	"Source/Graphics/LinearColor.cpp"
	"Source/HAL/BinaryStreamReader.cpp"
	"Source/HAL/BinaryStreamWriter.cpp"
	"Source/HAL/DateTime.cpp"
	"Source/HAL/Directory.cpp"
	"Source/HAL/EventLoop.cpp"
	"Source/HAL/File.cpp"
	"Source/HAL/FileStream.cpp"
	"Source/HAL/FileSystem.cpp"
	"Source/HAL/InternalTime.cpp"
	"Source/HAL/InternalTime.h"
	"Source/HAL/Path.cpp"
	"Source/HAL/TextStreamWriter.cpp"
	"Source/HAL/TimePoint.cpp"
	"Source/HAL/Timer.cpp"
	"Source/HAL/TimeSpan.cpp"
	"Source/Main/Main.cpp"
	"Source/Math/Math.cpp"
	"Source/Math/Matrix3.cpp"
	"Source/Math/Matrix4.cpp"
	"Source/Math/Quaternion.cpp"
	"Source/Math/Rotator.cpp"
	"Source/Math/Vector2.cpp"
	"Source/Math/Vector3.cpp"
	"Source/Math/Vector4.cpp"
	"Source/Memory/Memory.cpp"
	"Source/Memory/SmallBufferStorage.cpp"
	"Source/Meta/ArrayTypeInfo.cpp"
	"Source/Meta/AttributeInfo.cpp"
	"Source/Meta/ClassInfo.cpp"
	"Source/Meta/EnumInfo.cpp"
	"Source/Meta/FunctionInfo.cpp"
	"Source/Meta/PointerTypeInfo.cpp"
	"Source/Meta/PropertyInfo.cpp"
	"Source/Meta/StructInfo.cpp"
	"Source/Meta/TypeInfo.cpp"
	"Source/Misc/CString.cpp"
	"Source/Misc/InternalUnicode.cpp"
	"Source/Misc/InternalUnicode.h"
	"Source/Misc/SourceLocation.cpp"
	"Source/Misc/StringBuilder.cpp"
	"Source/Misc/StringFormatting.cpp"
	"Source/Misc/StringParsing.cpp"
	"Source/Misc/Unicode.cpp"
	"Source/Misc/Version.cpp"
	"Source/Regex/Regex.cpp"
	"Source/Threading/LockGuard.cpp"
	"Source/Threading/Mutex.cpp"
	"Source/Threading/Thread.cpp"
	"Source/Threading/ThreadPool.cpp"
	"ThirdParty/tiny-regex-c/Source/re.c"
)

if(WIN32)
	list(APPEND UMBRAL_CORE_LIB_SOURCES
		"Source/Engine/Windows/WindowsConsole.cpp"
		"Source/Engine/Windows/WindowsConsole.h"
		"Source/HAL/Windows/WindowsFileStream.cpp"
		"Source/HAL/Windows/WindowsFileStream.h"
		"Source/HAL/Windows/WindowsFileSystem.cpp"
		"Source/HAL/Windows/WindowsFileSystem.h"
		"Source/HAL/Windows/WindowsTime.cpp"
		"Source/HAL/Windows/WindowsTime.h"
	)
elseif(APPLE)
	list(APPEND UMBRAL_CORE_LIB_SOURCES
		"Source/Engine/Posix/PosixConsole.cpp"
		"Source/Engine/Posix/PosixConsole.h"
		"Source/HAL/Apple/AppleFileStream.cpp"
		"Source/HAL/Apple/AppleFileStream.h"
		"Source/HAL/Apple/AppleFileSystem.cpp"
		"Source/HAL/Apple/AppleFileSystem.h"
		"Source/HAL/Apple/AppleTime.cpp"
		"Source/HAL/Apple/AppleTime.h"
	)
else()
	list(APPEND UMBRAL_CORE_LIB_SOURCES
		"Source/Engine/Posix/PosixConsole.cpp"
		"Source/Engine/Posix/PosixConsole.h"
		"Source/HAL/Linux/LinuxFileStream.cpp"
		"Source/HAL/Linux/LinuxFileStream.h"
		"Source/HAL/Linux/LinuxFileSystem.cpp"
		"Source/HAL/Linux/LinuxFileSystem.h"
		"Source/HAL/Linux/LinuxTime.cpp"
		"Source/HAL/Linux/LinuxTime.h"
	)
endif()

add_library(UmbralCoreLib STATIC
	${UMBRAL_CORE_LIB_HEADERS}
	${UMBRAL_CORE_LIB_SOURCES}
)

add_library(umbral::core_lib ALIAS UmbralCoreLib)

if(WITH_SDL2)
	find_package(SDL2 CONFIG REQUIRED)

	target_link_libraries(UmbralCoreLib
		PRIVATE
			$<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
			$<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
	)
endif()

target_include_directories(UmbralCoreLib
	PUBLIC
		"Include"
	PRIVATE
		"Source"
		"ThirdParty/tiny-regex-c/Include"
)

target_link_libraries(UmbralCoreLib
	PRIVATE
		CURL::libcurl
		# ICU order for linking here is VERY important
		ICU::i18n
		ICU::uc
		ICU::data
		mimalloc-static
		nothings::stb
		tinycthread
		libuv::uv
		websockets_shared
)

if(WIN32)
	target_link_libraries(UmbralCoreLib PRIVATE PThreads4W::PThreads4W)
else()
	target_link_libraries(UmbralCoreLib PRIVATE Threads::Threads)
endif()

umbral_copy_libs_post_build(UmbralCoreLib)
umbral_set_target_properties(UmbralCoreLib)

target_compile_definitions(UmbralCoreLib
	PUBLIC
		-DUMBRAL_DEBUG=$<CONFIG:Debug>
		-DUMBRAL_RELEASE=$<CONFIG:Release>
	PRIVATE
		-DUMBRAL_CONTENT_DIR="${PROJECT_SOURCE_DIR}/EngineContent"
)

if(WITH_TESTING)
	enable_testing()

	add_executable(UmbralCoreLibTests
		"Tests/AnyTests.cpp"
		"Tests/ArrayTests.cpp"
		"Tests/FileTests.cpp"
		"Tests/FunctionTests.cpp"
		"Tests/HashMapTests.cpp"
		"Tests/HashTableTests.cpp"
		"Tests/InternationalizationTests.cpp"
		"Tests/LargeTypes.cpp"
		"Tests/LargeTypes.h"
		"Tests/LinkedListTests.cpp"
		"Tests/Main.cpp"
		"Tests/MathTests.cpp"
		"Tests/MemoryTests.cpp"
		"Tests/MiscTests.cpp"
		"Tests/PathTests.cpp"
		"Tests/RegexTests.cpp"
		"Tests/SharedPtrTests.cpp"
		"Tests/StaticArrayTests.cpp"
		"Tests/StringTests.cpp"
		"Tests/ThreadTests.cpp"
		"Tests/TupleTests.cpp"
		"Tests/TypeTraitTests.cpp"
		"Tests/UniquePtrTests.cpp"
		"Tests/VariantTests.cpp"
	)

	add_executable(umbral::core_lib::tests ALIAS UmbralCoreLibTests)

	target_include_directories(UmbralCoreLibTests
		PRIVATE
			"Source"
	)

	target_link_libraries(UmbralCoreLibTests
		PUBLIC
			umbral::core_lib
			GTest::gtest
			#GTest::gtest_main
			#GTest::gmock
			#GTest::gmock_main
	)

	target_compile_definitions(UmbralCoreLib
		PUBLIC
			-DWITH_TESTING=1
	)

	umbral_set_target_properties(UmbralCoreLibTests)

	include(GoogleTest)
	gtest_discover_tests(UmbralCoreLibTests)
endif()