#ifndef RENDER_ENGINE_OPENGL_HELPERS_H
#define RENDER_ENGINE_OPENGL_HELPERS_H

#include "enums/shader_type.h"
#include "enums/texture_type.h"
#include "enums/texture_internal_format.h"
#include "enums/buffer_bind_target.h"
#include "enums/buffer_usage_hint.h"
#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "enums/vertex_attribute_data_type.h"
#include "enums/texture_wrap_mode.h"
#include "enums/texture_filtering_mode.h"
#include "enums/framebuffer_attachment.h"

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

namespace OpenGLHelpers
{
    std::string GetShaderTypeName(ShaderType shaderType);
    GLuint ToShaderType(ShaderType shaderType);
    GLenum ToBufferBindTarget(BufferBindTarget bindTarget);
    GLenum ToBufferUsageHint(BufferUsageHint usageHint);
    GLenum ToPrimitiveType(PrimitiveType primitiveType);
    GLenum ToIndicesDataType(IndicesDataType dataType);
    GLenum ToVertexAttributeDataType(VertexAttributeDataType dataType);
    GLenum ToTextureType(TextureType textureType);
    GLenum ToTextureInternalFormat(TextureInternalFormat format);
    TextureInternalFormat FromTextureInternalFormat(GLenum format);
    GLenum ToTextureFormat(TextureInternalFormat format);
    GLenum ToTextureDataType(TextureInternalFormat format);
    GLenum ToTextureUnit(int binding);
    GLint ToTextureWrapMode(TextureWrapMode wrapMode);
    GLenum ToTextureTarget(TextureType type, int slice);
    void ToTextureFilteringMode(TextureFilteringMode filteringMode, GLint &outMinFilter, GLint &outMagFilter);
    GLenum ToFramebufferAttachment(FramebufferAttachment attachment);
}

#endif //RENDER_ENGINE_OPENGL_HELPERS_H
