# How to Build

This pages lists external dependencies that are required to build the ending, and provides instructions on the build process

## Table of contents

* [Dependencies](#dependencies)
  * [Visual Studio 17 2022](#visual-studio-17-2022)
  * [XCode](#xcode)
  * [Android Studio](#android-studio)
  * [CMake](#cmake)
  * [Ninja](#ninja)
  * [OpenGL](#opengl)
  * [DXC](#dxc)
* [How to build](#how-to-build-1)
  * [Building Executable](#building-executable)
    * [Using Build Script](#using-build-script) 
    * [Using CLion](#using-clion)
  * [Compiling Resources](#compiling-resources)

## Dependencies

### Visual Studio 17 2022

Required for compiling Windows builds. Can be downloaded from official website.

### XCode

Required for compiling Mac and iOS builds. Can be downloaded from AppStore.

Additionally, set `DEPLOYMENT_TARGET` to your currently installed MacOS and iOS SDK versions in:
* `.idea/cmake.xml` if you are building from CLion
* `build_scripts/build_executable.sh` if you are building using scripts

Installed SDK versions can be checked by running `xcodebuild -showsdks`

### Android Studio

Required for compiling Android builds. Can be downloaded from official website.

* Android SDK and NDK must also be installed
* Set `JAVA_HOME` environment variable
  * Example: `setx JAVA_HOME C:\Program Files\Android\Android Studio\jbr`

### CMake

Required for building. Comes packaged with CLion or can be downloaded from official website.

* Add CMake bin path to `PATH` environment variable
  * Example: `setx PATH %PATH%;C:\Program Files\JetBrains\CLion 2023.3.6\bin\cmake\win\x64\bin`

### Ninja

Required for compiling Android builds. Comes packaged with CLion or can be downloaded from [official website](https://ninja-build.org/).

* Add Ninja bin path to `PATH` environment variable
  * Example: `setx PATH %PATH%;C:\Program Files\JetBrains\CLion 2023.3.6\bin\ninja\win\x64`

### OpenGL

GLEW is required for Windows builds.

  * Download [GLEW binaries](http://glew.sourceforge.net/index.html)
  * Place DLL files to `C:\Windows\System32` directory
  * Add paths to header files and lib directories to `CMAKE_PREFIX_PATH`
    * Example: `setx CMAKE_PREFIX_PATH %CMAKE_PREFIX_PATH%;C:\Workspace\glew-2.1.0-win32\glew-2.1.0\include;C:\Workspace\glew-2.1.0-win32\glew-2.1.0\lib\Release\x64;`

### DXC

Required for compiling shaders for all platforms.

* OSX
  * No additional actions needed, library is already provided

* Windows
  * Static lib is already provided, but DLL must be provided as well
  * Download a release from [DXC GitHub](https://github.com/microsoft/DirectXShaderCompiler)
  * Place DLL files to `C:\Windows\System32`

## How to build

First, initialize all git submodules

```
git submodule update --recursive --remote
```

### Building Executable

#### Using Build Script

After installing all dependencies for required platform, run `build_scripts/build_executable.sh` script.

This scripts will compile `TextureCompressor`, `ShaderCompiler` and `RenderEngineLauncher` targets and build resources inside `build_resources` directory
with models, compressed textures and shaders translated for target graphics API.

Executable can be found at:
* Windows - `cmake-build-release-win64/launchers/windows_launcher/Release`
* Android - `launchers/android_launcher/AndroidStudio/app/build/outputs/apk/release`

For Mac/iOS - open XCode project at `launchers/apple_launcher/xcode`, select Mac or iOS target and press Start

#### Using CLion

1. Select CMake Profile according to the platform and build configuration:
   * Windows
     * Debug-Win64
     * Release-Win64
   * Android
     * Debug-Android-ARM64
     * RelWithDebInfo-Android-ARM64
   * Mac
     * Debug-Mac-ARM64
     * Release-Mac-ARM64
   * iOS
     * Debug-iOS
     * Release-iOS

2. Compile `ShaderCompiler`, `TextureCompressor` and `RenderEngineLauncher` targets
   * Note, that `ShaderCompiler` and `TextureCompressor` targets are not available for iOS and Android, they have to be built with MacOS or Windows profile

3. Run executable
   * On Windows - launch executable in `cmake-build-release-win64/launchers/windows_launcher/Release`
   * On Android - open Android Studio project in `launchers/android_launcher/AndroidStudio`, attach device and press Start
   * On Mac - open XCode project in `launcher/apple_launcher/xcode`, select Mac or iOS target and press Start

### Compiling Resources

To manually recompile resources, run `build_scripts/build_resources.sh` script. They will automatically get included into new build when running from CLion, Android Studio or XCode