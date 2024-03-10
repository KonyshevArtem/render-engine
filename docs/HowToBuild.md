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
    * Place DLL files in `C:\Windows\System32` and `C:\Windows\SysWOW64` directories
    * Add paths to header files and lib directories to `CMAKE_PREFIX_PATH`

### Boost

* OSX
    * Installed with `brew install boost` command

* Windows
    * Download [sources](https://www.boost.org/users/download/)
    * Build binaries from sources using [guide](https://www.boost.org/doc/libs/1_49_0/more/getting_started/windows.html#or-build-binaries-from-source) (Section 5.2 Simplified build from source)
    * Add path to boost include and lib directories to `CMAKE_PREFIX_PATH`

#### Boost link issue

CMake might not set architecture tag for boost, and would not be able to find libs. In that case, add line `set(Boost_ARCHITECTURE -x64)` to [CMakeLists](../CMakeLists.txt) file

In case of any problems, add `set(Boost_DEBUG ON)` line to [CMakeLists](../CMakeLists.txt) file. It will provide more into about the problem

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

Then run this terminal commands inside source directory

```
cmake -S . -B build
```

Directory `build` will be created.

```
make -C build
```

After that, if everything is set up correctly, executable should be built. 

Folder `resources` should be copied during the build process as well.
Resources from that folder will be loaded at runtime, so it should always be in the same directory as the executable.