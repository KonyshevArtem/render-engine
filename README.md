# OpenGL study project

Simple project to study how rendering works in games and how
game engines work

### Implemented features

* Forward rendering
* Lambert lighting model for vertex lit objects
* Blinn-Phong lighting model for fragment lit objects
* Support for single directional light
* Support for several point lights and spot lights
* Casting shadows from all spot lights in scene
* Albedo textures
* Shader introspection for obtaining all uniform locations
to generalize work with uniforms
* Shader preprocessor that implements `#include` feature
* Skybox with cubemap
* Normal mapping

### Requirements

* OpenGL
* GLUT
* Boost

OpenGL and GLUT should be installed by default on OSX. Boost can
be installed with `brew install boost`

### External Tools

* [lodepng](https://github.com/lvandeve/lodepng)
* [OpenFBX](https://github.com/nem0/OpenFBX)

### How to build

Run this terminal commands inside source directory

`cmake -S . -B build`

Directory `build` will be created.

`make -C build`

Executable will be built to run application. Folder
`resources` will be copied before build.
Resources from that folder will be loaded at runtime.
It should always be in the same directory as the executable.