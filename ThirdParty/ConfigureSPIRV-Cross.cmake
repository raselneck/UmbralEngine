set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS    OFF CACHE BOOL "Instead of throwing exceptions assert" FORCE)
set(SPIRV_CROSS_SHARED                      OFF CACHE BOOL "Build the C API as a single shared library" FORCE)
set(SPIRV_CROSS_STATIC                      ON  CACHE BOOL "Build the C and C++ API as static libraries" FORCE)
set(SPIRV_CROSS_CLI                         OFF CACHE BOOL "Build the CLI binary. Requires SPIRV_CROSS_STATIC" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS                OFF CACHE BOOL "Enable SPIRV-Cross tests" FORCE)

set(SPIRV_CROSS_ENABLE_GLSL                 ON  CACHE BOOL "Enable GLSL support" FORCE)
set(SPIRV_CROSS_ENABLE_HLSL                 ON  CACHE BOOL "Enable HLSL target support" FORCE)
set(SPIRV_CROSS_ENABLE_MSL                  ON  CACHE BOOL "Enable MSL target support" FORCE)
set(SPIRV_CROSS_ENABLE_CPP                  ON  CACHE BOOL "Enable C++ target support" FORCE)
set(SPIRV_CROSS_ENABLE_REFLECT              OFF CACHE BOOL "Enable JSON reflection target support" FORCE)
set(SPIRV_CROSS_ENABLE_C_API                OFF CACHE BOOL "Enable C API wrapper support in static library" FORCE)
set(SPIRV_CROSS_ENABLE_UTIL                 ON  CACHE BOOL "Enable util module support" FORCE)

set(SPIRV_CROSS_SANITIZE_ADDRESS            OFF CACHE BOOL "Sanitize address" FORCE)
set(SPIRV_CROSS_SANITIZE_MEMORY             OFF CACHE BOOL "Sanitize memory" FORCE)
set(SPIRV_CROSS_SANITIZE_THREADS            OFF CACHE BOOL "Sanitize threads" FORCE)
set(SPIRV_CROSS_SANITIZE_UNDEFINED          OFF CACHE BOOL "Sanitize undefined" FORCE)

set(SPIRV_CROSS_NAMESPACE_OVERRIDE          ""  CACHE STRING "Override the namespace used in the C++ API" FORCE)
set(SPIRV_CROSS_FORCE_STL_TYPES             OFF CACHE BOOL "Force use of STL types instead of STL replacements in certain places. Might reduce performance" FORCE)

set(SPIRV_CROSS_SKIP_INSTALL                OFF CACHE BOOL "Skips installation targets" FORCE)

set(SPIRV_CROSS_WERROR                      ON  CACHE BOOL "Fail build on warnings" FORCE)
set(SPIRV_CROSS_MISC_WARNINGS               ON  CACHE BOOL "Misc warnings useful for Travis runs" FORCE)

set(SPIRV_CROSS_FORCE_PIC                   ON  CACHE BOOL "Force position-independent code for all targets" FORCE)