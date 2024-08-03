# Umbral Engine

My own little playground for creating data structures from scratch, exploring algorithms, and making tiny games. :smile:

## Features

- Custom data structures written from scratch with APIs that, I feel, make them easy and nice to use
  - Array, String, StringView, Span, HashTable, HashMap, Function, LinkedList, Tuple, Variant
- Custom logging infrastructure that logs to files and the console (with colors!)
- Custom "error or value" system inspired by the one in [SerenityOS](https://github.com/SerenityOS/serenity)
- Platform-independent API for files, directories, paths, date-time, and time spans inspired by .NET
- Fairly robust math API inspired by XNA / MonoGame
- Custom smart pointers and memory allocation backed by [mimalloc](https://github.com/microsoft/mimalloc)
- Custom opt-in reflection system (a la Unreal) backed by [tree-sitter-cpp](https://github.com/tree-sitter/tree-sitter-cpp)
- Custom compiler-independent type traits system
- Object-based mark-and-sweep garbage collection system inspired by SerenityOS's
  [original garbage collector](https://www.youtube.com/watch?v=IzB6iTeo8kk) with an emphasis on cleaning up objects in
  reverse creation order to help guarantee resource cleanup
  - For example: a graphics device is guaranteed to be cleaned up *before* its associated window due to needing to
    create the graphics device *after* the window
- Abstracted graphics API inspired by XNA / MonoGame (currently only with OpenGL and partial Vulkan backends but Metal
  and DirectX 12, and possibly DirectX 11, are planned as I eventually get time)

## Building Prerequisites

### vcpkg Triplets

Umbral uses custom vcpkg triplet files. When building, you will need to supply the correct triplet to use. The directory
for the custom triplet files is set up in the `vcpkg-configuration.json`, but it is currently
`<UMBRAL_ROOT>/CMake/Triplets`.

```
> cmake -DVCPKG_TARGET_TRIPLET=arm64-osx-umbral   -DCMAKE_TOOLCHAIN_FILE=...
> cmake -DVCPKG_TARGET_TRIPLET=x64-windows-umbral -DCMAKE_TOOLCHAIN_FILE=...
> cmake -DVCPKG_TARGET_TRIPLET=x64-linux-umbral   -DCMAKE_TOOLCHAIN_FILE=...
etc
```

### Visual Studio 2022

On Windows, Visual Studio 2022 is required to be installed. If there is a newer version of Visual Studio then it has
probably not yet been tested so any build issues are up to you to fix and work with.

### Dependencies

On Mac, you'll need to install the following packages through homebrew to be able to successfully build the vcpkg
dependencies:

```
> brew install autoconf autoconf-archive automake
```

On Ubuntu, the following packages are required:

```
> sudo apt install autoconf autoconf-archive make linux-libc-dev libibus-1.0-dev libasound2-dev libpulse-dev libxi-dev \
                   libgl1-mesa-dev libglu1-mesa-dev mesa-common-dev libxrandr-dev libxxf86vm-dev
```

If your Ubuntu installation is using Wayland (default in 22.04 and onwards), then you'll also need to install the
following packages:

```
> sudo apt install libwayland-dev libxkbcommon-dev libegl1-mesa-dev
```

If your Ubuntu installation is using the Xorg windowing system, then you'll need these packages instead:

```
> sudo apt install libx11-dev libxft-dev libxext-dev
```

Note that you may need to install the above Xorg libraries to satisfy SDL2's build requirements even if you are using a
different window compositor.

### ANGLE

To be able to use Google's ANGLE on Mac, we need to perform a custom build for it because the current version available
on vcpkg (Chromium 5414) does not support creating an OpenGL ES context above version 3.0. The full
[installation instructions](https://github.com/google/angle/blob/main/doc/DevSetup.md) can be found in ANGLE's
repository, but here is what I did:

##### 1. Install Depot Tools

First [install Google's Depot Tools](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up).
The current steps are:
- `git` clone the tools from `https://chromium.googlesource.com/chromium/tools/depot_tools.git` (I cloned them
  into `~/.depot_tools`)
  - Google says to download a specific bundle for Windows if you'd like to
    use ANGLE there for consistency, but it should be fine to clone it
- Add the path you cloned them to your path (in my case: `export PATH=~/.depot_tools:$PATH`)
- Profit

**Important:** on Windows, you'll need to set `DEPOT_TOOLS_WIN_TOOLCHAIN=0` in your environment.

##### 2. Fetch ANGLE

Navigate to a directory where you'd like to host the ANGLE source (`C:\Develop`, `~/Develop`, etc), create a folder
called ANGLE or whatever you'd like, then in a command prompt enter `fetch angle`.

After the fetch is complete, you'll need to set the build directory. The ANGLE docs recommend `out/Debug` so that's
what I did with `gn gen out/Debug`.

##### 3. Configure ANGLE

On Windows, you'll need to make sure you have the Windows SDK installed. Which version depends entirely on ANGLE and can
be found near the top of the file `<ANGLE_ROOT>/build/vs_toolchain.py`. One thing to note, though, is that using the
Visual Studio installer for the Windows SDK will not work with Google's build tools as you also need to make sure you
install "Debugging Tools for Windows." To do that, download the [SDK installer from Microsoft](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)
and run through the steps.

On Mac, we specifically don't want to enable the default OpenGL backend since Apple's OpenGL implementation only
supports up to 4.1 (which is ES 3.0). To remedy this, run `gn args out/Debug` and set the following variables:

```
angle_enable_gl=false
angle_enable_d3d9=false  # Windows only
angle_enable_d3d11=true  # Windows only
angle_enable_metal=true  # Mac only
angle_enable_vulkan=true
is_component_build=false # Only necessary on Windows... maybe
target_cpu="arm64"       # If you have an Apple M* chip, otherwise do "x64"
is_debug=true            # Set to false for Release
use_goma=false           # Necessary unless you're a Google employee
```

Alternatively, just copy the above into the file `<ANGLE_ROOT>/out/Debug/args.gn` and then re-run `gn gen out/Debug`.

##### 4. Build ANGLE

On Mac, you will need to modify the file `src/gpu_info_util/SystemInfo_macos.mm` to define `GL_SILENCE_DEPRECATION` to
`1` at the very beginning of the file.

To build, all you need to do is run `autoninja -C out/Debug` from `<ANGLE_ROOT>`. This may lock up your system on
Windows as it builds, so take note of that.

##### 5. Use ANGLE with Umbral

Once you've successfully built ANGLE, you can go ahead and build Umbral with it. Current CMake scripts attempt to verify
that Google's Depot Tools are also installed when building Umbral so that we can eventually modify the scripts to
download and build ANGLE directly from within CMake (assuming you've got the Depot Tools installed).

On Mac, you will need to make sure that you specify Vulkan as the backend for ANGLE by setting
`ANGLE_DEFAULT_PLATFORM=vulkan` _somewhere_ in your environment before launching anything made with Umbral. This is
because Metal as an ANGLE backend only supports up to OpenGL ES 3.0, and unfortunately it is currently not possible to
build ANGLE without Metal enabled.

##### Updating ANGLE

To update ANGLE, you should just need to navigate to `<ANGLE_ROOT>` and run `gclient sync`.

If you get an error about not being able to find a module named `pkg_resources`, you may need to run the following
command:

```
> python3 -m pip install --upgrade pip setuptools 
```

## Building

After you've made sure you've met all the prerequisites for your platform, you should be able to simply build Umbral
using CMake and specifying the path to your vcpkg installation's toolchain file. Opening Umbral in CLion or Visual
Studio with CMake support installed should do most of the heavy lifting for you.

## Contributing

### Code Style

Umbral uses the [Unreal coding style](https://docs.unrealengine.com/4.26/en-US/ProductionPipelines/DevelopmentSetup/CodingStandard/)
for type names and then (mostly) C# coding style for everything else. If you browse the source code for a little bit
you'll more than likely be able to pick it up fairly easily.

The graphics bits are modelled after Microsoft's XNA 4, the single greatest piece of software I've ever had the pleasure
of programming with.

## Third Party Acknowledgements

> This software uses the FreeImage open source image library. See http://freeimage.sourceforge.net for details. <br/>
> FreeImage is used under the FIPL, version 1.0.

The full FreeImage license text can be found [here](ThirdParty/FreeImage/freeimage-license.txt).

> Portions of this software are copyright &copy; 2023 The FreeType Project (www.freetype.org). <br/>
> All rights reserved.

The full FreeType license text can be found [here](ThirdParty/FreeType/FTL.TXT).

Please see the [read me](ThirdParty/ReadMe.md) file in the ThirdParty directory for more third-party license
information, including mandatory stated changes to certain libraries.

## License

```
Copyright 2024 Richard Selneck

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
```

For full license text, see [License.txt](License.txt). The license applies to the following
files and directories:
- `CMake/*`
- `Engine/*`
  - Excluding `Engine/CoreLib/ThirdParty/*`
  - Excluding `Engine/Engine/ThirdParty/*`
- `EngineContent/Models/*`
- `EngineContent/Shaders/*`
- `EngineContent/Textures/*`
- `MetaTool/*`
- `Projects/*`
- `ShaderCompiler/CMake/*`
- `ShaderCompiler/Include/USL/*`
- `ShaderCompiler/Source/USL/*`
- `ShaderCompiler/CMakeLists.txt`