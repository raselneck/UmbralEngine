### Third-Party Library Etiquette

Only libraries that have licenses compatible with Umbral's licensing are used.

Any modifications to third-part libraries **MUST** be surrounded with `MODIFICATIONS BEGIN` and `MODIFICATIONS END` comments.

### Third-Party Library Info

| Library Name        | Version, Git Commit, or Git Branch                                                                                                   | License                                                                                                                            |
|---------------------|--------------------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------|
| assimp              | 5.3.1 (via vcpkg)                                                                                                                    | [BSD 3-Clause](https://github.com/assimp/assimp/blob/v5.3.1/LICENSE)                                                               |
| box2d               | 2.4.1 (via vcpkg)                                                                                                                    | [MIT](https://github.com/erincatto/box2d/blob/v2.4.1/LICENSE)                                                                      |
| bullet3             | 3.25 (via vcpkg)                                                                                                                     | [zlib](https://github.com/bulletphysics/bullet3/blob/3.25/LICENSE.txt)                                                             |
| CURL                | 8.3.0 (via vcpkg)                                                                                                                    | [CURL (MIT/X Inspired)](https://github.com/curl/curl/blob/curl-8_3_0/COPYING)                                                      |
| Dear ImGui[^1]      | [v1.90.6-docking](https://github.com/ocornut/imgui/tree/v1.90.6-docking)                                                             | [MIT](https://github.com/ocornut/imgui/blob/v1.90.6-docking/LICENSE.txt)                                                           |
| FreeImage           | 3.18.0 (via vcpkg)                                                                                                                   | [FreeImage Public License Version 1.0](https://freeimage.sourceforge.io/freeimage-license.txt)                                     |
| FreeType            | 2.13.2 (via vcpkg)                                                                                                                   | [FreeType License](https://gitlab.freedesktop.org/freetype/freetype/-/blob/VER-2-13-2/docs/FTL.TXT?ref_type=tags)                  |
| glslang             | 13.0.0 (via vcpkg)                                                                                                                   | [&lt;various&gt;](https://github.com/KhronosGroup/glslang/blob/13.0.0/LICENSE.txt)                                                 |
| icu                 | 73.1 (via vcpkg)                                                                                                                     | [Unicode-3.0](https://github.com/unicode-org/icu/blob/release-73-1/icu4c/LICENSE)                                                  |
| mimalloc            | 2.1.1 (via vcpkg)                                                                                                                    | [MIT](https://github.com/microsoft/mimalloc/blob/v2.1.1/LICENSE)                                                                   | 
| SDL2                | 2.28.3 (via vcpkg)                                                                                                                   | [zlib](https://github.com/libsdl-org/SDL/blob/release-2.28.3/LICENSE.txt)                                                          |
| shaderc + glslc[^2] | [2022.2](https://github.com/google/shaderc/tree/v2022.2)                                                                             | [Apache License 2.0](https://github.com/google/shaderc/blob/v2022.2/LICENSE)                                                       |
| SPIRV-Cross         | [sdk-1.3.216](https://github.com/KhronosGroup/SPIRV-Cross/tree/sdk-1.3.216)                                                          | [Apache License 2.0](https://github.com/KhronosGroup/SPIRV-Cross/blob/sdk-1.3.216/LICENSE)                                         |
| SPIRV-Headers       | [sdk-1.3.216](https://github.com/KhronosGroup/SPIRV-Headers/tree/sdk-1.3.216)                                                        | [MIT (?)](https://github.com/KhronosGroup/SPIRV-Headers/blob/sdk-1.3.216/LICENSE)[^3]                                              |
| SPIRV-Tools         | [sdk-1.3.216](https://github.com/KhronosGroup/SPIRV-Tools/tree/sdk-1.3.216)                                                          | [Apache License 2.0](https://github.com/KhronosGroup/SPIRV-Tools/blob/sdk-1.3.216/LICENSE)                                         |
| stb                 | [b42009b3b9d4ca35bc703f5310eedc74f584be58](https://github.com/nothings/stb/tree/b42009b3b9d4ca35bc703f5310eedc74f584be58)            | [MIT or Public Domain](https://github.com/nothings/stb/blob/b42009b3b9d4ca35bc703f5310eedc74f584be58/LICENSE)                      |
| TinyCThread         | [6957fc8383d6c7db25b60b8c849b29caab1caaee](https://github.com/tinycthread/tinycthread/tree/6957fc8383d6c7db25b60b8c849b29caab1caaee) | [Simplified BSD 3-Clause](https://github.com/tinycthread/tinycthread/blob/6957fc8383d6c7db25b60b8c849b29caab1caaee/README.txt)[^3] |
| tiny-regex-c        | [2d306a5a71128853d18292e8bb85c8e745fbc9d0](https://github.com/kokke/tiny-regex-c/commit/2d306a5a71128853d18292e8bb85c8e745fbc9d0)    | [Public Domain / The Unlicense](https://github.com/kokke/tiny-regex-c/blob/2d306a5a71128853d18292e8bb85c8e745fbc9d0/LICENSE)       |
| TLSF                | [deff9ab509341f264addbd3c8ada533678591905](https://github.com/mattconte/tlsf/tree/deff9ab509341f264addbd3c8ada533678591905)          | ["BSD License"](https://github.com/mattconte/tlsf/tree/deff9ab509341f264addbd3c8ada533678591905)[^3]                               |
| tree-sitter         | [v0.20.8](https://github.com/tree-sitter/tree-sitter/tree/v0.20.8)                                                                   | [MIT](https://github.com/tree-sitter/tree-sitter/blob/v0.20.8/LICENSE)                                                             |
| tree-sitter-cpp     | [v0.20.3-2-ga90f170](https://github.com/tree-sitter/tree-sitter-cpp/tree/a90f170f92d5d70e7c2d4183c146e61ba5f3a457)                   | [MIT](https://github.com/tree-sitter/tree-sitter-cpp/blob/a90f170f92d5d70e7c2d4183c146e61ba5f3a457/LICENSE)                        |
| tree-sitter-json    | [v0.20.1](https://github.com/tree-sitter/tree-sitter-json/tree/v0.20.1)                                                              | [MIT](https://github.com/tree-sitter/tree-sitter-json/blob/v0.20.1/LICENSE)                                                        |

[^1]: Dear ImGui source located in `Engine/Engine/ThirdParty`. The source there is organized in the Umbral Engine style
  instead of a direct one-to-one source mapping from GitHub.

[^2]: libshaderc, libshaderc_util, and glslc are all integrated directly into the shader compiler and library. Apparently
  the best way to learn how to use glslang was to study shaderc, but I have not had the time or motivation to do that
  learning yet.

[^3]: Unsure of the actual name of the license.

### Third-Party Library Changes

#### SPIRV-Cross

Added support for emitting default float and int precision statements to vertex shaders. The NVIDIA driver on my desktop
apparently requires this when the generated GLSL specifies `#version 310 es`. See `spirv_glsl.hpp` and `spirv_glsl.cpp`.

#### SPIRV-Tools

Commented out the defaulted copy constructor of `StructuredControlState` as it is unnecessary and caused a compilation
error on Mac. See `merge_return_pass.h`.

Fixed and simplified constructor of `ScalarReplacementPass` to use safe C functions. See `scalar_replacement_pass.h`.