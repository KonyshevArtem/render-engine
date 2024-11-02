# How to Build

This pages lists external dependencies that are required to build the ending, and provides instructions on the build process

## Table of contents

* [Dependencies](#dependencies)
  * [OpenGL](#opengl)
  * [Boost](#boost)
    * [Boost link issue](#boost-link-issue)
  * [Environment variables example](#environment-variables-example)
* [How to build](#how-to-build-1)

## Dependencies

### OpenGL

* OSX
    * Should be installed by default

* Windows
    * Download [GLEW binaries](http://glew.sourceforge.net/index.html)
    * Place DLL files to `C:\Windows\System32` directory
    * Add paths to header files and lib directories to `CMAKE_PREFIX_PATH`

### Boost

* OSX
    * Installed with `brew install boost` command

* Windows
    * Download [sources](https://www.boost.org/users/download/)
    * For Visual Studio toolchain
      * Build binaries from sources using [guide](https://www.boost.org/doc/libs/1_49_0/more/getting_started/windows.html#or-build-binaries-from-source) (Section 5.2 Simplified build from source)
    * For MinGW toolchain
      * Follow this [guide](https://gist.github.com/zrsmithson/0b72e0cb58d0cb946fc48b5c88511da8)
      * Don't forget to add `--with-json` argument and remove `--build-type=complete` to only build Boost::json, otherwise it will take 5 hours
    * Add path to boost include and lib directories to `CMAKE_PREFIX_PATH`

#### Boost link issue

CMake might not set architecture tag for boost, and would not be able to find libs. In that case, add line `set(Boost_ARCHITECTURE -x64)` to [CMakeLists](../CMakeLists.txt) file

In case of any problems, add `set(Boost_DEBUG ON)` line to [CMakeLists](../CMakeLists.txt) file. It will provide more into about the problem

### DXC

* OSX
  * No additional actions needed, library is already provided

* Windows
  * Static lib is already provided, but DLL must be provided as well
  * Download a release from [DXC GitHub](https://github.com/microsoft/DirectXShaderCompiler)
  * Place DLL files to `C:\Windows\System32`

### Environment variables example

Here is an example of how `CMAKE_PREFIX_PATH` environment variable should look in order for cmake to find all dependencies

```
set CMAKE_PREFIX_PATH=F:/glew-2.1.0/include;F:/glew-2.1.0/lib/Release/x64;F:/boost_1_78_0;F:/boost_1_78_0/stage/lib;
```

## How to build

First, initialize all git submodules

```
git submodule update --recursive --remote
```

### Using CLion

This is the recommended way to compile

Compile `ShaderCompiler`, `TextureCompressor` and `RenderEngineWindow`/`RenderEngineApple` targets 
based on selected platform

### Using Terminal

Alternatively, run these terminal commands inside source directory

```
cmake -S . -B cmake-build-release
```

Directory `cmake-build-release` will be created.

```
make -C cmake-build-release
```

This will compile all targets, so there is no need to specify like with CLion

### Compiling Resources

After compiling required targets, run `recompile_shaders.sh` and `reimport_textures.sh` scripts.

They should generate `resources/shaders/output` and `resources/textures/output` folders with compressed texture binaries
and shaders translated for target graphics API.

### Running Executable

On Windows, just launch .exe in `cmake-build-release/launchers/windows_launcher`

On Mac, open XCode project in `launchers/apple_launcher/xcode` and press Start