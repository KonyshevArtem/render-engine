# How to Build

This pages lists external dependencies that are required to build the ending, and provides instructions on the build process

## Table of contents

* [Dependencies](#dependencies)
  * [OpenGL](#opengl)
  * [DXC](#dxc)
  * [Environment variables example](#environment-variables-example)
* [How to build](#how-to-build-1)
  * [Using CLion](#using-clion)
  * [Using Terminal](#using-terminal)
  * [Compiling Resources](#compiling-resources)
  * [Running Executable](#running-executable)

## Dependencies

### OpenGL

* OSX
    * Should be installed by default

* Windows
    * Download [GLEW binaries](http://glew.sourceforge.net/index.html)
    * Place DLL files to `C:\Windows\System32` directory
    * Add paths to header files and lib directories to `CMAKE_PREFIX_PATH`

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
set CMAKE_PREFIX_PATH=F:/glew-2.1.0/include;F:/glew-2.1.0/lib/Release/x64;
```

## How to build

First, initialize all git submodules

```
git submodule update --recursive --remote
```

### Using CLion

This is the recommended way to compile

1. Select CMake Profile according to the platform and build configuration:
   * Windows
     * Debug-Win64
     * Release-Win64
   * Mac
     * Debug-Mac-ARM64
     * Release-Mac-ARM64
   * iOS
     * Debug-iOS
     * Release-iOS

2. Compile `ShaderCompiler`, `TextureCompressor` and `RenderEngineLauncher` targets

Note, that `ShaderCompiler` and `TextureCompressor` targets are not available for iOS, they have to be built with MacOS profile 

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