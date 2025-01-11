#ifndef RENDER_ENGINE_OPENGL_HELPERS_H
#define RENDER_ENGINE_OPENGL_HELPERS_H

#ifdef RENDER_BACKEND_OPENGL

#include "enums/shader_type.h"
#include "enums/texture_type.h"
#include "enums/texture_internal_format.h"
#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "enums/vertex_attribute_data_type.h"
#include "enums/texture_wrap_mode.h"
#include "enums/texture_filtering_mode.h"
#include "enums/framebuffer_attachment.h"
#include "enums/depth_function.h"
#include "enums/cull_face_orientation.h"
#include "enums/cull_face.h"
#include "enums/blend_factor.h"
#include "enums/texture_data_type.h"
#include "enums/cubemap_face.h"

#include <string>

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
#if __has_include("GLES3/gl32.h")
#include <GLES3/gl32.h>
#endif
#if __has_include("GLES2/gl2ext.h")
#include <GLES2/gl2ext.h>
#endif
#if __has_include("GLES/egl.h")
#include <GLES/egl.h>
#define REQUIRE_BINDINGS_INIT
#endif

#ifdef REQUIRE_BINDINGS_INIT

#define GL_TIMESTAMP GL_TIMESTAMP_EXT
#define GL_MAP_PERSISTENT_BIT GL_MAP_PERSISTENT_BIT_EXT
#define GL_MAP_COHERENT_BIT GL_MAP_COHERENT_BIT_EXT

void glQueryCounter(GLuint id, GLenum target);
void glGetQueryObjectui64v(GLuint id, GLenum pname, GLuint64* params);
void glBufferStorage(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags);

#endif

namespace OpenGLHelpers
{
    void InitBindings();

    std::string GetShaderTypeName(ShaderType shaderType);
    GLuint ToShaderType(ShaderType shaderType);
    GLenum ToPrimitiveType(PrimitiveType primitiveType);
    GLenum ToIndicesDataType(IndicesDataType dataType);
    GLenum ToVertexAttributeDataType(VertexAttributeDataType dataType);
    GLenum ToTextureType(TextureType textureType);
    GLenum ToTextureInternalFormat(TextureInternalFormat format, bool isLinear);
    TextureInternalFormat FromTextureInternalFormat(GLenum format, bool& outIsLinear);
    GLenum ToTextureFormat(TextureInternalFormat format);
    GLenum ToTextureDataType(TextureInternalFormat format);
    GLenum ToTextureUnit(int binding);
    GLint ToTextureWrapMode(TextureWrapMode wrapMode);
    GLenum ToTextureTarget(TextureType type, CubemapFace cubemapFace);
    void ToTextureFilteringMode(TextureFilteringMode filteringMode, GLint &outMinFilter, GLint &outMagFilter);
    GLenum ToFramebufferAttachment(FramebufferAttachment attachment);
    GLenum ToDepthCompareFunction(DepthFunction function);
    GLenum ToCullFaceOrientation(CullFaceOrientation orientation);
    GLenum ToCullFace(CullFace face);
    GLenum ToBlendFactor(BlendFactor factor);
    bool IsTexture(GLenum uniformType);
    TextureDataType FromTextureDataType(GLenum textureDataType);
}

#endif //RENDER_BACKEND_OPENGL
#endif //RENDER_ENGINE_OPENGL_HELPERS_H
