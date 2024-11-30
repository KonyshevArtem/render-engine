#ifdef RENDER_BACKEND_OPENGL

#include "graphics_backend_api_opengl.h"
#include "enums/texture_data_type.h"
#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "enums/fence_type.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_geometry.h"
#include "types/graphics_backend_texture_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "types/graphics_backend_render_target_descriptor.h"
#include "types/graphics_backend_depth_stencil_state.h"
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "types/graphics_backend_fence.h"
#include "helpers/opengl_helpers.h"
#include "debug.h"

#include <type_traits>
#include <stdexcept>
#include <cassert>
#include <array>

struct DepthStencilState
{
    GLboolean DepthWrite;
    GLenum DepthFunction;
};

struct BlendState
{
    GLenum SourceFactor;
    GLenum DestinationFactor;
    bool Enabled;
};

struct DebugMessageType
{
    GLenum Type;
    bool Enabled;
};

struct BufferData
{
    uint64_t GLBuffer;
    uint8_t* Data;
};

GLuint s_Framebuffers[2];
GLbitfield s_ClearFlags[static_cast<int>(FramebufferAttachment::MAX)];
uint64_t s_DebugGroupId = 0;

std::array s_DebugMessageTypes =
{
    DebugMessageType{GL_DEBUG_TYPE_ERROR, true},
    DebugMessageType{GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, true},
    DebugMessageType{GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, true},
    DebugMessageType{GL_DEBUG_TYPE_PORTABILITY, true},
    DebugMessageType{GL_DEBUG_TYPE_PERFORMANCE, true},
    DebugMessageType{GL_DEBUG_TYPE_MARKER, false},
    DebugMessageType{GL_DEBUG_TYPE_PUSH_GROUP, false},
    DebugMessageType{GL_DEBUG_TYPE_POP_GROUP, false},
    DebugMessageType{GL_DEBUG_TYPE_OTHER, false}
};

void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    auto GetSourceName = [source]() -> std::string
    {
        switch (source)
        {
            case GL_DEBUG_SOURCE_API:
                return "OpenGL API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION:
                return "Application";
            case GL_DEBUG_SOURCE_OTHER:
                return "Other";
        }
    };

    auto GetTypeName = [type]() -> std::string
    {
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:
                return "Error";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                return "Undefined Behaviour";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                return "Deprecated Behaviour";
            case GL_DEBUG_TYPE_PORTABILITY:
                return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE:
                return "Performance";
        }
    };

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            Debug::LogErrorFormat("[GraphicsBackend] [{}] [{}] {}", GetSourceName(), GetTypeName(), std::string(message, length));
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        case GL_DEBUG_TYPE_PORTABILITY:
        case GL_DEBUG_TYPE_PERFORMANCE:
            Debug::LogWarningFormat("[GraphicsBackend] [{}] [{}] {}", GetSourceName(), GetTypeName(), std::string(message, length));
            break;
    }
}

void ResetClearFlags()
{
    int max = static_cast<int>(FramebufferAttachment::MAX);
    for (int i = 0; i < max; ++i)
    {
        s_ClearFlags[i] = 0;
    }
}

void GraphicsBackendOpenGL::Init(void *data)
{
#ifdef REQUIRE_GLEW_INIT
    auto result = glewInit();
    if (result != GLEW_OK)
        throw;
#endif

    int extensionsCount;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionsCount);
    for (int i = 0; i < extensionsCount; ++i)
    {
        const unsigned char *ext = glGetStringi(GL_EXTENSIONS, i);
        m_Extensions.insert(std::string(reinterpret_cast<const char *>(ext)));
    }

    glGenFramebuffers(2, &s_Framebuffers[0]);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_DEPTH_TEST);

#if RENDER_ENGINE_EDITOR
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(DebugMessageCallback, nullptr);

    for (const DebugMessageType& messageType : s_DebugMessageTypes)
    {
        glDebugMessageControl(GL_DONT_CARE, messageType.Type, GL_DONT_CARE, 0, nullptr, messageType.Enabled ? GL_TRUE : GL_FALSE);
    }
#endif

    ResetClearFlags();
}

const std::string &GraphicsBackendOpenGL::GetGLSLVersionString()
{
    static std::string glslVersion = "#version " + std::to_string(OPENGL_MAJOR_VERSION * 100 + OPENGL_MINOR_VERSION * 10);
    return glslVersion;
}

GraphicsBackendName GraphicsBackendOpenGL::GetName()
{
    return GraphicsBackendName::OPENGL;
}

void GraphicsBackendOpenGL::InitNewFrame(void *data)
{
}

GraphicsBackendTexture GraphicsBackendOpenGL::CreateTexture(int width, int height, int depth, TextureType type, TextureInternalFormat format, int mipLevels, bool isLinear, bool isRenderTarget, const std::string& name)
{
    GraphicsBackendTexture texture{};
    glGenTextures(1, reinterpret_cast<GLuint *>(&texture.Texture));

    const GLenum textureType = OpenGLHelpers::ToTextureType(type);
    glBindTexture(textureType, texture.Texture);
    glTexParameteri(textureType, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(textureType, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);
    if (!name.empty())
    {
        glObjectLabel(GL_TEXTURE, texture.Texture, name.length(), name.c_str());
    }

    const GLenum internalFormat = OpenGLHelpers::ToTextureInternalFormat(format, isLinear);
    if (IsTexture3D(type))
    {
        glTexStorage3D(textureType, mipLevels, internalFormat, width, height, depth);
    }
    else
    {
        glTexStorage2D(textureType, mipLevels, internalFormat, width, height);
    }

    texture.Type = type;
    texture.Format = format;
    texture.IsLinear = isLinear;
    return texture;
}

GraphicsBackendSampler GraphicsBackendOpenGL::CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod, const std::string& name)
{
    GraphicsBackendSampler sampler{};
    glGenSamplers(1, reinterpret_cast<GLuint *>(&sampler.Sampler));

    GLint wrap = OpenGLHelpers::ToTextureWrapMode(wrapMode);
    glSamplerParameteri(sampler.Sampler, GL_TEXTURE_WRAP_S, wrap);
    glSamplerParameteri(sampler.Sampler, GL_TEXTURE_WRAP_T, wrap);
    glSamplerParameteri(sampler.Sampler, GL_TEXTURE_WRAP_R, wrap);

    GLint minFilter, magFilter;
    OpenGLHelpers::ToTextureFilteringMode(filteringMode, minFilter, magFilter);
    glSamplerParameteri(sampler.Sampler, GL_TEXTURE_MIN_FILTER, minFilter);
    glSamplerParameteri(sampler.Sampler, GL_TEXTURE_MAG_FILTER, magFilter);

    if (borderColor != nullptr)
    {
        glSamplerParameterfv(sampler.Sampler, GL_TEXTURE_BORDER_COLOR, borderColor);
    }

    glSamplerParameteri(sampler.Sampler, GL_TEXTURE_MIN_LOD, minLod);

    if (!name.empty())
    {
        glObjectLabel(GL_SAMPLER, sampler.Sampler, name.length(), name.c_str());
    }

    return sampler;
}

void GraphicsBackendOpenGL::DeleteTexture(const GraphicsBackendTexture &texture)
{
    glDeleteTextures(1, reinterpret_cast<const GLuint *>(&texture.Texture));
}

void GraphicsBackendOpenGL::DeleteSampler(const GraphicsBackendSampler &sampler)
{
    glDeleteSamplers(1, reinterpret_cast<const GLuint *>(&sampler.Sampler));
}

void GraphicsBackendOpenGL::BindTexture(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendTexture &texture)
{
    auto binding = bindings.VertexIndex >= 0 ? bindings.VertexIndex : bindings.FragmentIndex;
    glActiveTexture(OpenGLHelpers::ToTextureUnit(binding));
    glBindTexture(OpenGLHelpers::ToTextureType(texture.Type), texture.Texture);
    glUniform1i(binding, binding);
}

void GraphicsBackendOpenGL::BindSampler(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendSampler &sampler)
{
    auto binding = bindings.VertexIndex >= 0 ? bindings.VertexIndex : bindings.FragmentIndex;
    glActiveTexture(OpenGLHelpers::ToTextureUnit(binding));
    glBindSampler(binding, sampler.Sampler);
}

void GraphicsBackendOpenGL::GenerateMipmaps(const GraphicsBackendTexture &texture)
{
    GLenum textureType = OpenGLHelpers::ToTextureType(texture.Type);
    glBindTexture(textureType, texture.Texture);
    glGenerateMipmap(textureType);
}

void GraphicsBackendOpenGL::UploadImagePixels(const GraphicsBackendTexture &texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void *pixelsData)
{
    const GLenum type = OpenGLHelpers::ToTextureType(texture.Type);
    const GLenum target = OpenGLHelpers::ToTextureTarget(texture.Type, cubemapFace);
    const GLenum internalFormat = OpenGLHelpers::ToTextureInternalFormat(texture.Format, texture.IsLinear);
    const bool isTexture3D = IsTexture3D(texture.Type);

    glBindTexture(type, texture.Texture);
    if (IsCompressedTextureFormat(texture.Format) && imageSize != 0)
    {
        if (isTexture3D)
        {
            glCompressedTexSubImage3D(target, level, 0, 0, 0, width, height, depth, internalFormat, imageSize, pixelsData);
        }
        else
        {
            glCompressedTexSubImage2D(target, level, 0, 0, width, height, internalFormat, imageSize, pixelsData);
        }
    }
    else
    {
        const GLenum pixelFormat = OpenGLHelpers::ToTextureFormat(texture.Format);
        const GLenum dataType = OpenGLHelpers::ToTextureDataType(texture.Format);
        if (isTexture3D)
        {
            glTexSubImage3D(target, level, 0, 0, 0, width, height, depth, pixelFormat, dataType, pixelsData);
        }
        else
        {
            glTexSubImage2D(target, level, 0, 0, width, height, pixelFormat, dataType, pixelsData);
        }
    }
}

void AttachTextureToFramebuffer(GLenum framebuffer, GLenum attachment, TextureType type, GLuint texture, int level, int layer)
{
    if (type == TextureType::TEXTURE_2D)
    {
        glFramebufferTexture(framebuffer, attachment, texture, level);
    }
    else
    {
        glFramebufferTextureLayer(framebuffer, attachment, texture, level, layer);
    }
}

void GraphicsBackendOpenGL::AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor)
{
    if (descriptor.IsBackbuffer)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_Framebuffers[0]);

    GLenum glAttachment = OpenGLHelpers::ToFramebufferAttachment(descriptor.Attachment);
    AttachTextureToFramebuffer(GL_DRAW_FRAMEBUFFER, glAttachment, descriptor.Texture.Type, descriptor.Texture.Texture, descriptor.Level, descriptor.Layer);

    if (descriptor.Attachment == FramebufferAttachment::DEPTH_ATTACHMENT)
        AttachTextureToFramebuffer(GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, descriptor.Texture.Type, 0, 0, 0);

    if (descriptor.LoadAction == LoadAction::CLEAR)
    {
        int attachmentIndex = static_cast<int>(descriptor.Attachment);
        if (descriptor.Attachment == FramebufferAttachment::DEPTH_ATTACHMENT)
        {
            s_ClearFlags[attachmentIndex] = GL_DEPTH_BUFFER_BIT;
        }
        else if (descriptor.Attachment == FramebufferAttachment::STENCIL_ATTACHMENT)
        {
            s_ClearFlags[attachmentIndex] = GL_STENCIL_BUFFER_BIT;
        }
        else if (descriptor.Attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT)
        {
            s_ClearFlags[attachmentIndex] = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
        }
        else
        {
            s_ClearFlags[attachmentIndex] = GL_COLOR_BUFFER_BIT;
        }
    }
}

TextureInternalFormat GraphicsBackendOpenGL::GetRenderTargetFormat(FramebufferAttachment attachment, bool* outIsLinear)
{
    // Not implemented. Currently used only for compiling PSO, but OpenGL does not require render target format
    return TextureInternalFormat::RGBA8;
}

GraphicsBackendBuffer GraphicsBackendOpenGL::CreateBuffer(int size, const std::string& name, bool allowCPUWrites, const void* data)
{
    const GLbitfield bufferFlags = allowCPUWrites ? GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT : 0;

    GLuint glBuffer;
    glGenBuffers(1, &glBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, glBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, data, bufferFlags);
    if (!name.empty())
    {
        glObjectLabel(GL_BUFFER, glBuffer, name.length(), name.c_str());
    }

    BufferData* bufferData = new BufferData();
    bufferData->GLBuffer = glBuffer;
    bufferData->Data = allowCPUWrites ? static_cast<uint8_t*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, bufferFlags)) : nullptr;

    GraphicsBackendBuffer buffer{};
    buffer.Buffer = reinterpret_cast<uint64_t>(bufferData);
    buffer.Size = size;
    return buffer;
}

void GraphicsBackendOpenGL::DeleteBuffer(const GraphicsBackendBuffer &buffer)
{
    const BufferData* bufferData = reinterpret_cast<BufferData*>(buffer.Buffer);
    if (bufferData->Data)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, bufferData->GLBuffer);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }

    glDeleteBuffers(1, reinterpret_cast<const GLuint *>(&buffer.Buffer));
    delete bufferData;
}

void BindBuffer_Internal(GLenum bindTarget, const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
    const BufferData* bufferData = reinterpret_cast<BufferData*>(buffer.Buffer);

    auto binding = bindings.VertexIndex >= 0 ? bindings.VertexIndex : bindings.FragmentIndex;
    glBindBuffer(bindTarget, bufferData->GLBuffer);
    glBindBufferRange(bindTarget, binding, bufferData->GLBuffer, offset, size);
}

void GraphicsBackendOpenGL::BindBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
#if GL_ARB_shader_storage_buffer_object
    BindBuffer_Internal(GL_SHADER_STORAGE_BUFFER, buffer, bindings, offset, size);
#endif
}

void GraphicsBackendOpenGL::BindConstantBuffer(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
    BindBuffer_Internal(GL_UNIFORM_BUFFER, buffer, bindings, offset, size);
}

void GraphicsBackendOpenGL::SetBufferData(const GraphicsBackendBuffer& buffer, long offset, long size, const void *data)
{
    const BufferData* bufferData = reinterpret_cast<BufferData*>(buffer.Buffer);
    assert(bufferData->Data);
    memcpy(bufferData->Data + offset, data, size);
}

void GraphicsBackendOpenGL::CopyBufferSubData(const GraphicsBackendBuffer& source, const GraphicsBackendBuffer& destination, int sourceOffset, int destinationOffset, int size)
{
    const BufferData* sourceBufferData = reinterpret_cast<BufferData*>(source.Buffer);
    const BufferData* destinationBufferData = reinterpret_cast<BufferData*>(destination.Buffer);

    glBindBuffer(GL_COPY_READ_BUFFER, sourceBufferData->GLBuffer);
    glBindBuffer(GL_COPY_WRITE_BUFFER, destinationBufferData->GLBuffer);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, sourceOffset, destinationOffset, size);
}

uint64_t GraphicsBackendOpenGL::GetMaxConstantBufferSize()
{
    static uint64_t size = 0;

    if (size == 0)
    {
        int tempSize;
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &tempSize);
        size = tempSize;
    }

    return size;
}

int GraphicsBackendOpenGL::GetConstantBufferOffsetAlignment()
{
    static int alignment = 0;

    if (alignment == 0)
    {
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    }

    return alignment;
}

GraphicsBackendGeometry GraphicsBackendOpenGL::CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, const std::string& name)
{
    GraphicsBackendGeometry geometry{};
    geometry.VertexBuffer = vertexBuffer;
    geometry.IndexBuffer = indexBuffer;

    const BufferData* vertexBufferData = reinterpret_cast<BufferData*>(vertexBuffer.Buffer);
    const BufferData* indexBufferData = reinterpret_cast<BufferData*>(indexBuffer.Buffer);

    glGenVertexArrays(1, reinterpret_cast<GLuint *>(&geometry.VertexArrayObject));
    glBindVertexArray(geometry.VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData->GLBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferData->GLBuffer);
    if (!name.empty())
    {
        glObjectLabel(GL_VERTEX_ARRAY, geometry.VertexArrayObject, name.length(), name.c_str());
    }

    for (const auto &descriptor : vertexAttributes)
    {
        glEnableVertexAttribArray(descriptor.Index);
        glVertexAttribPointer(descriptor.Index, descriptor.Dimensions, OpenGLHelpers::ToVertexAttributeDataType(descriptor.DataType), descriptor.IsNormalized ? GL_TRUE : GL_FALSE, descriptor.Stride, reinterpret_cast<const void *>(descriptor.Offset));
    }

    return geometry;
}

void GraphicsBackendOpenGL::DeleteGeometry(const GraphicsBackendGeometry &geometry)
{
    glDeleteVertexArrays(1, reinterpret_cast<const GLuint *>(&geometry.VertexArrayObject));
    DeleteBuffer(geometry.VertexBuffer);
    DeleteBuffer(geometry.IndexBuffer);
}

void GraphicsBackendOpenGL::SetCullFace(CullFace cullFace)
{
    if (cullFace == CullFace::NONE)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glCullFace(OpenGLHelpers::ToCullFace(cullFace));
    }
}

void GraphicsBackendOpenGL::SetCullFaceOrientation(CullFaceOrientation orientation)
{
    glFrontFace(OpenGLHelpers::ToCullFaceOrientation(orientation));
}

void GraphicsBackendOpenGL::SetViewport(int x, int y, int width, int height, float near, float far)
{
    glViewport(x, y, width, height);
    glDepthRange(near, far);
}

GraphicsBackendShaderObject GraphicsBackendOpenGL::CompileShader(ShaderType shaderType, const std::string& source, const std::string& name)
{
    GraphicsBackendShaderObject shaderObject{};
    shaderObject.ShaderObject = glCreateShader(OpenGLHelpers::ToShaderType(shaderType));

    auto *sourceChar = source.c_str();
    glShaderSource(shaderObject.ShaderObject, 1, &sourceChar, nullptr);
    glCompileShader(shaderObject.ShaderObject);

    if (!name.empty())
    {
        glObjectLabel(GL_SHADER, shaderObject.ShaderObject, name.length(), name.c_str());
    }

    int isCompiled;
    glGetShaderiv(shaderObject.ShaderObject, GL_COMPILE_STATUS, &isCompiled);
    if (!isCompiled)
    {
        int infoLogLength;
        glGetShaderiv(shaderObject.ShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::string logMsg(infoLogLength + 1, ' ');
        glGetShaderInfoLog(shaderObject.ShaderObject, infoLogLength, nullptr, &logMsg[0]);

        throw std::runtime_error(OpenGLHelpers::GetShaderTypeName(shaderType) + " shader compilation failed with errors:\n" + logMsg);
    }

    return shaderObject;
}

GraphicsBackendProgram GraphicsBackendOpenGL::CreateProgram(const std::vector<GraphicsBackendShaderObject> &shaders, const GraphicsBackendColorAttachmentDescriptor &colorAttachmentDescriptor, TextureInternalFormat depthFormat,
                                                            const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, const std::string& name)
{
    GraphicsBackendProgram program{};
    program.Program = glCreateProgram();

    for (auto &shader : shaders)
    {
        bool isShader = glIsShader(shader.ShaderObject);
        if (isShader)
        {
            glAttachShader(program.Program, shader.ShaderObject);
        }
    }

    glLinkProgram(program.Program);

    for (auto &shader : shaders)
    {
        bool isShader = glIsShader(shader.ShaderObject);
        if (isShader)
        {
            glDetachShader(program.Program, shader.ShaderObject);
        }
    }

    if (!name.empty())
    {
        glObjectLabel(GL_PROGRAM, program.Program, name.length(), name.c_str());
    }

    int isLinked;
    glGetProgramiv(program.Program, GL_LINK_STATUS, &isLinked);
    if (!isLinked)
    {
        int infoLogLength;
        glGetProgramiv(program.Program, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::string logMsg(infoLogLength + 1, ' ');
        glGetProgramInfoLog(program.Program, infoLogLength, nullptr, &logMsg[0]);

        throw std::runtime_error("Link failed with error:\n" + logMsg);
    }

    auto blendState = new BlendState();
    blendState->SourceFactor = OpenGLHelpers::ToBlendFactor(colorAttachmentDescriptor.SourceFactor);
    blendState->DestinationFactor = OpenGLHelpers::ToBlendFactor(colorAttachmentDescriptor.DestinationFactor);
    blendState->Enabled = colorAttachmentDescriptor.BlendingEnabled;
    program.BlendState = reinterpret_cast<uint64_t>(blendState);

    return program;
}

void GraphicsBackendOpenGL::DeleteShader(GraphicsBackendShaderObject shader)
{
    glDeleteShader(shader.ShaderObject);
}

void GraphicsBackendOpenGL::DeleteProgram(GraphicsBackendProgram program)
{
    glDeleteProgram(program.Program);

    auto blendState = reinterpret_cast<BlendState*>(program.BlendState);
    delete blendState;
}

bool GraphicsBackendOpenGL::RequireStrictPSODescriptor()
{
    return false;
}

void GraphicsBackendOpenGL::UseProgram(GraphicsBackendProgram program)
{
    glUseProgram(program.Program);

    auto blendState = reinterpret_cast<BlendState*>(program.BlendState);
    if (blendState)
    {
        if (blendState->Enabled)
        {
            glEnable(GL_BLEND);
            glBlendFunc(blendState->SourceFactor, blendState->DestinationFactor);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }
}

void GraphicsBackendOpenGL::SetClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void GraphicsBackendOpenGL::SetClearDepth(double depth)
{
    glClearDepth(depth);
}

void GraphicsBackendOpenGL::DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int count)
{
    glBindVertexArray(geometry.VertexArrayObject);
    glDrawArrays(OpenGLHelpers::ToPrimitiveType(primitiveType), firstIndex, count);
}

void GraphicsBackendOpenGL::DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    glBindVertexArray(geometry.VertexArrayObject);
    glDrawArraysInstanced(OpenGLHelpers::ToPrimitiveType(primitiveType), firstIndex, indicesCount, instanceCount);
}

void GraphicsBackendOpenGL::DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType)
{
    glBindVertexArray(geometry.VertexArrayObject);
    glDrawElements(OpenGLHelpers::ToPrimitiveType(primitiveType), elementsCount, OpenGLHelpers::ToIndicesDataType(dataType), nullptr);
}

void GraphicsBackendOpenGL::DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount)
{
    glBindVertexArray(geometry.VertexArrayObject);
    glDrawElementsInstanced(OpenGLHelpers::ToPrimitiveType(primitiveType), elementsCount, OpenGLHelpers::ToIndicesDataType(dataType), nullptr, instanceCount);
}

void GraphicsBackendOpenGL::CopyTextureToTexture(const GraphicsBackendTexture &source, const GraphicsBackendRenderTargetDescriptor &destinationDescriptor, unsigned int sourceX, unsigned int sourceY, unsigned int destinationX, unsigned int destinationY, unsigned int width, unsigned int height)
{
    if (destinationDescriptor.Texture.Texture == 0 && !destinationDescriptor.IsBackbuffer)
        return;

    GLenum glAttachment = OpenGLHelpers::ToFramebufferAttachment(destinationDescriptor.Attachment);

    bool isDepth = glAttachment == GL_DEPTH_ATTACHMENT || glAttachment == GL_DEPTH_STENCIL_ATTACHMENT;
    bool isStencil = glAttachment == GL_STENCIL_ATTACHMENT || glAttachment == GL_DEPTH_STENCIL_ATTACHMENT;

    GLenum mask = 0;
    if (isDepth || isStencil)
    {
        if (isDepth)
        {
            mask |= GL_DEPTH_BUFFER_BIT;
        }
        if (isStencil)
        {
            mask |= GL_STENCIL_BUFFER_BIT;
        }
    }
    else
    {
        mask |= GL_COLOR_BUFFER_BIT;
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, s_Framebuffers[0]);
    AttachTextureToFramebuffer(GL_READ_FRAMEBUFFER, glAttachment, source.Type, source.Texture, 0, 0);

    GLuint destinationFramebuffer = destinationDescriptor.IsBackbuffer ? 0 : s_Framebuffers[1];
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destinationFramebuffer);
    if (!destinationDescriptor.IsBackbuffer)
    {
        AttachTextureToFramebuffer(GL_DRAW_FRAMEBUFFER, glAttachment, destinationDescriptor.Texture.Type, destinationDescriptor.Texture.Texture, 0, 0);
    }

    glBlitFramebuffer(sourceX, sourceX, sourceX + width, sourceY + height, destinationX, destinationY, destinationX + width, destinationY + height, mask, GL_NEAREST);
}

void GraphicsBackendOpenGL::PushDebugGroup(const std::string& name)
{
#ifdef GL_KHR_debug
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, s_DebugGroupId++, -1, name.c_str());
#endif
}

void GraphicsBackendOpenGL::PopDebugGroup()
{
#ifdef GL_KHR_debug
    assert(s_DebugGroupId > 0);
    glPopDebugGroup();
    --s_DebugGroupId;
#endif
}

void GraphicsBackendOpenGL::BeginRenderPass(const std::string& name)
{
    GLbitfield clearFlag = 0;

    int maxAttachments = static_cast<int>(FramebufferAttachment::MAX);
    for (int i = 0; i < maxAttachments; ++i)
    {
        clearFlag |= s_ClearFlags[i];
    }

    PushDebugGroup(name);

    if (clearFlag != 0)
    {
        if ((clearFlag & (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) != 0)
        {
            glDepthMask(GL_TRUE);
        }
        glClear(clearFlag);
    }
}

void GraphicsBackendOpenGL::EndRenderPass()
{
    ResetClearFlags();
    PopDebugGroup();
}

void GraphicsBackendOpenGL::BeginCopyPass(const std::string& name)
{
    PushDebugGroup(name);
}

void GraphicsBackendOpenGL::EndCopyPass()
{
    PopDebugGroup();
}

GraphicsBackendDepthStencilState GraphicsBackendOpenGL::CreateDepthStencilState(bool depthWrite, DepthFunction depthFunction, const std::string& name)
{
    auto glState = new DepthStencilState();
    glState->DepthFunction = OpenGLHelpers::ToDepthCompareFunction(depthFunction);
    glState->DepthWrite = depthWrite ? GL_TRUE : GL_FALSE;

    GraphicsBackendDepthStencilState state{};
    state.m_State = reinterpret_cast<uint64_t>(glState);
    return state;
}

void GraphicsBackendOpenGL::DeleteDepthStencilState(const GraphicsBackendDepthStencilState& state)
{
    auto glState = reinterpret_cast<DepthStencilState*>(state.m_State);
    delete glState;
}

void GraphicsBackendOpenGL::SetDepthStencilState(const GraphicsBackendDepthStencilState& state)
{
    auto glState = reinterpret_cast<DepthStencilState*>(state.m_State);
    if (glState)
    {
        glDepthFunc(glState->DepthFunction);
        glDepthMask(glState->DepthWrite);
    }
}

GraphicsBackendFence GraphicsBackendOpenGL::InsertFence(FenceType fenceType, const std::string& name)
{
    return GraphicsBackendFence{};
}

void GraphicsBackendOpenGL::WaitForFence(const GraphicsBackendFence& fence)
{
}

#endif // RENDER_BACKEND_OPENGL