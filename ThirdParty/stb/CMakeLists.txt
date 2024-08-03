# STB files currently based on commit b42009b3b9d4ca35bc703f5310eedc74f584be58
# https://github.com/nothings/stb/tree/b42009b3b9d4ca35bc703f5310eedc74f584be58

set(STB_INCLUDE_FILES
	"Include/stb_dxt.h"
	"Include/stb_herringbone_wang_tile.h"
	"Include/stb_image.h"
	"Include/stb_image_resize.h"
	"Include/stb_image_write.h"
	"Include/stb_rect_pack.h"
	"Include/stb_truetype.h"
)

add_library(stb INTERFACE)
add_library(nothings::stb ALIAS stb)
target_include_directories(stb INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>")