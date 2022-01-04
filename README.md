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
* Support for single directional light
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

* OpenGL
* GLUT
* Boost

OpenGL and GLUT should be installed by default on OSX. Boost can
be installed with `brew install boost`

## External Tools

* [lodepng](https://github.com/lvandeve/lodepng)
* [OpenFBX](https://github.com/nem0/OpenFBX)

## How to build

Run this terminal commands inside source directory

`cmake -S . -B build`

Directory `build` will be created.

`make -C build`

Executable will be built to run application. Folder
`resources` will be copied before build.
Resources from that folder will be loaded at runtime.
It should always be in the same directory as the executable.
