# OpenGL study project

Simple project to study how rendering works in games and how
game engines work

## Implemented features

### Render Paths
Only forward rendering path is implemented for now

### Lighting
* Lambert lighting model for vertex lit objects
* Blinn-Phong lighting model for fragment lit objects

### Light sources
* Support for single directional light with shadows
* Support for several point lights
* Support for several spot lights with shadows

### Shader features
* Albedo textures
* Normal maps
* Custom shader preprocessor
* Alpha blending and alpha clipping

### Other
* Skybox with cubemap
* Billboard renderer
* Opaque and transparent passes with different sorting and clear flags

## Requirements

### OpenGL

* OSX
  * Should be installed by default

* Windows
  * Download GLEW binaries from http://glew.sourceforge.net/index.html
  * Place DLL files in `System32` and `SysWOW64` directories
  * Add paths to header and libs directories to `CMAKE_PREFIX_PATH`

### Boost

* OSX
  * Installed with `brew install boost` command

* Windows
  * Download sources from https://www.boost.org/users/download/
  * Build binaries from sources using https://www.boost.org/doc/libs/1_49_0/more/getting_started/windows.html#or-build-binaries-from-source (Section 5.2 Simplified build from source)
  * Add path to boost directory to `CMAKE_PREFIX_PATH`

### Qt
* Download installer from https://www.qt.io/download and install
* Add path to Qt to `CMAKE_PREFIX_PATH`
* On Windows add path to Qt binaries to `PATH`

### Environment variables example

```
set PATH=%PATH%;D:\Qt\6.2.3\msvc2019_64\bin;
set CMAKE_PREFIX_PATH=F:/glew-2.1.0/include;F:/glew-2.1.0/lib/Release/x64;F:/boost_1_78_0;D:/Qt/6.2.3/msvc2019_64;
```

## External Tools

* [lodepng](https://github.com/lvandeve/lodepng)
* [OpenFBX](https://github.com/nem0/OpenFBX)

## How to build

Run this terminal commands inside source directory

```
cmake -S . -B build
```

Directory `build` will be created.

```
make -C build
```

Executable will be built to run application. Folder
`resources` will be copied before build.
Resources from that folder will be loaded at runtime.
It should always be in the same directory as the executable.
