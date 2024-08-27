#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_H

#if __has_include("GL/glew.h")
#include <GL/glew.h>
#define REQUIRE_GLEW_INIT
#endif
#if __has_include("OpenGL/glu.h")
#include <OpenGL/glu.h>
#endif
#if __has_include("OpenGL/gl3.h")
#include <OpenGL/gl3.h>
#endif
#if __has_include("OpenGL/gl3ext.h")
#include <OpenGL/gl3ext.h>
#endif

#define GRAPHICS_BACKEND_TYPE_ENUM   GLenum

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_H
