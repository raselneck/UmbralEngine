#
# This is the main testbed for testing Umbral Engine features, but will eventually be a spiritual successor
# to my C# + XNA voxel engine Kyoob.
# https://github.com/raselneck/kyoob
#

add_executable(Bocks
	"Include/Bocks/BocksEngine.h"
	"Include/Bocks/BocksViewport.h"
	"Source/Bocks/BocksEngine.cpp"
	"Source/Bocks/BocksViewport.cpp"
	"Source/Main.cpp"
)
add_executable(umbral::bocks ALIAS Bocks)

target_include_directories(Bocks
	PUBLIC "Include"
	PRIVATE "Source"
)

target_link_libraries(Bocks
	PUBLIC
		umbral::core_lib
		umbral::object_lib
		umbral::engine
)

umbral_copy_libs_post_build(Bocks)
umbral_set_target_properties(Bocks)
umbral_enable_reflection(Bocks)