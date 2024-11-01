#ifdef RENDER_BACKEND_OPENGL

#include "graphics_backend_api_opengl.h"
#include "enums/texture_data_type.h"
#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
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
#include "helpers/opengl_helpers.h"
#include "debug.h"

#include <type_traits>
#include <stdexcept>

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

GLuint s_Framebuffers[2];
GLbitfield s_ClearFlags[static_cast<int>(FramebufferAttachment::MAX)];

void LogError(GLenum errorCode, const std::string& line, const std::string &file, int lineNumber)
{
    std::string errorString = "Unknown error";
    switch (errorCode)
    {
        case GL_INVALID_ENUM:
            errorString = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            errorString = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            errorString = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            errorString = "GL_OUT_OF_MEMORY";
            break;
        case GL_STACK_UNDERFLOW:
            errorString = "GL_STACK_UNDERFLOW";
            break;
        case GL_STACK_OVERFLOW:
            errorString = "GL_STACK_OVERFLOW";
            break;
    }

    Debug::LogErrorFormat("[Graphics Backend] %1%\n%4%\n%2%:%3%", {errorString, file, std::to_string(lineNumber), line});
}

#ifdef RENDER_ENGINE_EDITOR
#define CHECK_GRAPHICS_BACKEND_FUNC(backendFunction)               \
    backendFunction;                                               \
    {                                                              \
        auto error = glGetError();                                 \
        if (error != GL_NO_ERROR)                                            \
            LogError(error, #backendFunction, __FILE__, __LINE__); \
    }
#else
#define CHECK_GRAPHICS_BACKEND_FUNC(backendFunction) backendFunction;
#endif

template<typename T>
constexpr auto Cast(T value) -> typename std::underlying_type<T>::type
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}

template<typename T>
constexpr auto Cast(T *values) -> typename std::underlying_type<T>::type*
{
    return reinterpret_cast<typename std::underlying_type<T>::type*>(values);
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
    CHECK_GRAPHICS_BACKEND_FUNC(glGetIntegerv(GL_NUM_EXTENSIONS, &extensionsCount))
    for (int i = 0; i < extensionsCount; ++i)
    {
        const unsigned char *ext = CHECK_GRAPHICS_BACKEND_FUNC(glGetStringi(GL_EXTENSIONS, i))
        m_Extensions.insert(std::string(reinterpret_cast<const char *>(ext)));
    }

    CHECK_GRAPHICS_BACKEND_FUNC(glGenFramebuffers(2, &s_Framebuffers[0]))

    CHECK_GRAPHICS_BACKEND_FUNC(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS))
    CHECK_GRAPHICS_BACKEND_FUNC(glEnable(GL_DEPTH_TEST))

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

GraphicsBackendTexture GraphicsBackendOpenGL::CreateTexture(int width, int height, int depth, TextureType type, TextureInternalFormat format, int mipLevels, bool isLinear, bool isRenderTarget)
{
    GraphicsBackendTexture texture{};
    CHECK_GRAPHICS_BACKEND_FUNC(glGenTextures(1, reinterpret_cast<GLuint *>(&texture.Texture)))

    GLenum textureType = OpenGLHelpers::ToTextureType(type);
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(textureType, texture.Texture))
    CHECK_GRAPHICS_BACKEND_FUNC(glTexParameteri(textureType, GL_TEXTURE_BASE_LEVEL, 0))
    CHECK_GRAPHICS_BACKEND_FUNC(glTexParameteri(textureType, GL_TEXTURE_MAX_LEVEL, mipLevels - 1))

    texture.Type = type;
    texture.Format = format;
    texture.IsLinear = isLinear;

    if (isRenderTarget)
    {
        for (int i = 0; i < mipLevels; ++i)
        {
            UploadImagePixels(texture, i, CubemapFace::POSITIVE_X, width / (i + 1), height / (i + 1), depth, 0, nullptr);
        }
    }

    return texture;
}

GraphicsBackendSampler GraphicsBackendOpenGL::CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod)
{
    GraphicsBackendSampler sampler{};
    CHECK_GRAPHICS_BACKEND_FUNC(glGenSamplers(1, reinterpret_cast<GLuint *>(&sampler.Sampler)))

    GLint wrap = OpenGLHelpers::ToTextureWrapMode(wrapMode);
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameteri(sampler.Sampler, GL_TEXTURE_WRAP_S, wrap))
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameteri(sampler.Sampler, GL_TEXTURE_WRAP_T, wrap))
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameteri(sampler.Sampler, GL_TEXTURE_WRAP_R, wrap))

    GLint minFilter, magFilter;
    OpenGLHelpers::ToTextureFilteringMode(filteringMode, minFilter, magFilter);
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameteri(sampler.Sampler, GL_TEXTURE_MIN_FILTER, minFilter))
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameteri(sampler.Sampler, GL_TEXTURE_MAG_FILTER, magFilter))

    if (borderColor != nullptr)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameterfv(sampler.Sampler, GL_TEXTURE_BORDER_COLOR, borderColor))
    }

    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameteri(sampler.Sampler, GL_TEXTURE_MIN_LOD, minLod))

    return sampler;
}

void GraphicsBackendOpenGL::DeleteTexture(const GraphicsBackendTexture &texture)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteTextures(1, reinterpret_cast<const GLuint *>(&texture.Texture)))
}

void GraphicsBackendOpenGL::DeleteSampler(const GraphicsBackendSampler &sampler)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteSamplers(1, reinterpret_cast<const GLuint *>(&sampler.Sampler)))
}

void GraphicsBackendOpenGL::BindTexture(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendTexture &texture)
{
    auto binding = bindings.VertexIndex >= 0 ? bindings.VertexIndex : bindings.FragmentIndex;
    CHECK_GRAPHICS_BACKEND_FUNC(glActiveTexture(OpenGLHelpers::ToTextureUnit(binding)))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(OpenGLHelpers::ToTextureType(texture.Type), texture.Texture))
    CHECK_GRAPHICS_BACKEND_FUNC(glUniform1i(binding, binding))
}

void GraphicsBackendOpenGL::BindSampler(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendSampler &sampler)
{
    auto binding = bindings.VertexIndex >= 0 ? bindings.VertexIndex : bindings.FragmentIndex;
    CHECK_GRAPHICS_BACKEND_FUNC(glActiveTexture(OpenGLHelpers::ToTextureUnit(binding)))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindSampler(binding, sampler.Sampler))
}

void GraphicsBackendOpenGL::GenerateMipmaps(const GraphicsBackendTexture &texture)
{
    GLenum textureType = OpenGLHelpers::ToTextureType(texture.Type);
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(textureType, texture.Texture))
    CHECK_GRAPHICS_BACKEND_FUNC(glGenerateMipmap(textureType))
}

void GraphicsBackendOpenGL::UploadImagePixels(const GraphicsBackendTexture &texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void *pixelsData)
{
    GLenum type = OpenGLHelpers::ToTextureType(texture.Type);
    GLenum target = OpenGLHelpers::ToTextureTarget(texture.Type, cubemapFace);
    GLenum internalFormat = OpenGLHelpers::ToTextureInternalFormat(texture.Format, texture.IsLinear);
    bool isImage3D = texture.Type == TextureType::TEXTURE_2D_ARRAY;

    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(type, texture.Texture))
    if (IsCompressedTextureFormat(texture.Format) && imageSize != 0)
    {
        if (isImage3D)
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glCompressedTexImage3D(target, level, internalFormat, width, height, depth, 0, imageSize, pixelsData))
        }
        else
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glCompressedTexImage2D(target, level, internalFormat, width, height, 0, imageSize, pixelsData))
        }
    }
    else
    {
        GLenum pixelFormat = OpenGLHelpers::ToTextureFormat(texture.Format);
        GLenum dataType = OpenGLHelpers::ToTextureDataType(texture.Format);
        if (isImage3D)
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glTexImage3D(target, level, internalFormat, width, height, depth, 0, pixelFormat, dataType, pixelsData))
        }
        else
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glTexImage2D(target, level, internalFormat, width, height, 0, pixelFormat, dataType, pixelsData))
        }
    }
}

void AttachTextureToFramebuffer(GLenum framebuffer, GLenum attachment, TextureType type, GLuint texture, int level, int layer)
{
    if (type == TextureType::TEXTURE_2D)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glFramebufferTexture(framebuffer, attachment, texture, level))
    }
    else
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glFramebufferTextureLayer(framebuffer, attachment, texture, level, layer))
    }
}

void GraphicsBackendOpenGL::AttachRenderTarget(const GraphicsBackendRenderTargetDescriptor &descriptor)
{
    if (descriptor.IsBackbuffer)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0))
        return;
    }

    CHECK_GRAPHICS_BACKEND_FUNC(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_Framebuffers[0]))

    GLenum glAttachment = OpenGLHelpers::ToFramebufferAttachment(descriptor.Attachment);
    AttachTextureToFramebuffer(GL_DRAW_FRAMEBUFFER, glAttachment, descriptor.Texture.Type, descriptor.Texture.Texture, descriptor.Level, descriptor.Layer);

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

GraphicsBackendBuffer GraphicsBackendOpenGL::CreateBuffer(int size, BufferUsageHint usageHint)
{
    GLuint glBuffer;
    CHECK_GRAPHICS_BACKEND_FUNC(glGenBuffers(1, &glBuffer))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(GL_SHADER_STORAGE_BUFFER, glBuffer))
    CHECK_GRAPHICS_BACKEND_FUNC(glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, OpenGLHelpers::ToBufferUsageHint(usageHint)))

    GraphicsBackendBuffer buffer{};
    buffer.Buffer = static_cast<uint64_t>(glBuffer);
    buffer.Size = size;
    return buffer;
}

void GraphicsBackendOpenGL::DeleteBuffer(const GraphicsBackendBuffer &buffer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteBuffers(1, reinterpret_cast<const GLuint *>(&buffer.Buffer)))
}

void BindBuffer_Internal(GLenum bindTarget, const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
    auto binding = bindings.VertexIndex >= 0 ? bindings.VertexIndex : bindings.FragmentIndex;
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(bindTarget, buffer.Buffer))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBufferRange(bindTarget, binding, buffer.Buffer, offset, size))
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

void GraphicsBackendOpenGL::SetBufferData(GraphicsBackendBuffer &buffer, long offset, long size, const void *data)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(GL_UNIFORM_BUFFER, buffer.Buffer))
    void *contents = CHECK_GRAPHICS_BACKEND_FUNC(glMapBufferRange(GL_UNIFORM_BUFFER, offset, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT))
    memcpy(contents, data, size);
    CHECK_GRAPHICS_BACKEND_FUNC(glUnmapBuffer(GL_UNIFORM_BUFFER))
}

void GraphicsBackendOpenGL::CopyBufferSubData(GraphicsBackendBuffer source, GraphicsBackendBuffer destination, int sourceOffset, int destinationOffset, int size)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(GL_COPY_READ_BUFFER, source.Buffer))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(GL_COPY_WRITE_BUFFER, destination.Buffer))
    CHECK_GRAPHICS_BACKEND_FUNC(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, sourceOffset, destinationOffset, size))
}

uint64_t GraphicsBackendOpenGL::GetMaxConstantBufferSize()
{
    static uint64_t size = 0;

    if (size == 0)
    {
        int tempSize;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &tempSize))
        size = tempSize;
    }

    return size;
}

int GraphicsBackendOpenGL::GetConstantBufferOffsetAlignment()
{
    static int alignment = 0;

    if (alignment == 0)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment))
    }

    return alignment;
}

GraphicsBackendGeometry GraphicsBackendOpenGL::CreateGeometry(const GraphicsBackendBuffer &vertexBuffer, const GraphicsBackendBuffer &indexBuffer, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes)
{
    GraphicsBackendGeometry geometry{};
    geometry.VertexBuffer = vertexBuffer;
    geometry.IndexBuffer = indexBuffer;

    CHECK_GRAPHICS_BACKEND_FUNC(glGenVertexArrays(1, reinterpret_cast<GLuint *>(&geometry.VertexArrayObject)))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindVertexArray(geometry.VertexArrayObject))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.Buffer))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.Buffer))

    for (const auto &descriptor : vertexAttributes)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glEnableVertexAttribArray(descriptor.Index))
        CHECK_GRAPHICS_BACKEND_FUNC(glVertexAttribPointer(descriptor.Index, descriptor.Dimensions, OpenGLHelpers::ToVertexAttributeDataType(descriptor.DataType), descriptor.IsNormalized ? GL_TRUE : GL_FALSE, descriptor.Stride, reinterpret_cast<const void *>(descriptor.Offset)))
    }

    return geometry;
}

void GraphicsBackendOpenGL::DeleteGeometry(const GraphicsBackendGeometry &geometry)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteVertexArrays(1, reinterpret_cast<const GLuint *>(&geometry.VertexArrayObject)))
    DeleteBuffer(geometry.VertexBuffer);
    DeleteBuffer(geometry.IndexBuffer);
}

void GraphicsBackendOpenGL::EnableVertexAttributeArray(int index)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glEnableVertexAttribArray(index))
}

void GraphicsBackendOpenGL::DisableVertexAttributeArray(int index)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDisableVertexAttribArray(index))
}

void GraphicsBackendOpenGL::SetVertexAttributePointer(int index, int size, VertexAttributeDataType dataType, bool normalized, int stride, const void *pointer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glVertexAttribPointer(index, size, Cast(dataType), normalized ? GL_TRUE : GL_FALSE, stride, pointer))
}

void GraphicsBackendOpenGL::SetVertexAttributeDivisor(int index, int divisor)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glVertexAttribDivisor(index, divisor))
}

void GraphicsBackendOpenGL::SetCullFace(CullFace cullFace)
{
    if (cullFace == CullFace::NONE)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glDisable(GL_CULL_FACE))
    }
    else
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glEnable(GL_CULL_FACE))
        CHECK_GRAPHICS_BACKEND_FUNC(glCullFace(OpenGLHelpers::ToCullFace(cullFace)))
    }
}

void GraphicsBackendOpenGL::SetCullFaceOrientation(CullFaceOrientation orientation)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glFrontFace(OpenGLHelpers::ToCullFaceOrientation(orientation)))
}

void GraphicsBackendOpenGL::SetViewport(int x, int y, int width, int height, float near, float far)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glViewport(x, y, width, height))
    CHECK_GRAPHICS_BACKEND_FUNC(glDepthRange(near, far))
}

GraphicsBackendShaderObject GraphicsBackendOpenGL::CompileShader(ShaderType shaderType, const std::string &source)
{
    GraphicsBackendShaderObject shaderObject{};
    shaderObject.ShaderObject = CHECK_GRAPHICS_BACKEND_FUNC(glCreateShader(OpenGLHelpers::ToShaderType(shaderType)))

    auto *sourceChar = source.c_str();
    CHECK_GRAPHICS_BACKEND_FUNC(glShaderSource(shaderObject.ShaderObject, 1, &sourceChar, nullptr))
    CHECK_GRAPHICS_BACKEND_FUNC(glCompileShader(shaderObject.ShaderObject))

    int isCompiled;
    CHECK_GRAPHICS_BACKEND_FUNC(glGetShaderiv(shaderObject.ShaderObject, GL_COMPILE_STATUS, &isCompiled))
    if (!isCompiled)
    {
        int infoLogLength;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetShaderiv(shaderObject.ShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength));

        std::string logMsg(infoLogLength + 1, ' ');
        CHECK_GRAPHICS_BACKEND_FUNC(glGetShaderInfoLog(shaderObject.ShaderObject, infoLogLength, nullptr, &logMsg[0]))

        throw std::runtime_error(OpenGLHelpers::GetShaderTypeName(shaderType) + " shader compilation failed with errors:\n" + logMsg);
    }

    return shaderObject;
}

GraphicsBackendProgram GraphicsBackendOpenGL::CreateProgram(const std::vector<GraphicsBackendShaderObject> &shaders, const GraphicsBackendColorAttachmentDescriptor &colorAttachmentDescriptor, TextureInternalFormat depthFormat,
                                                            const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes)
{
    GraphicsBackendProgram program{};
    program.Program = CHECK_GRAPHICS_BACKEND_FUNC(glCreateProgram())

    for (auto &shader : shaders)
    {
        bool isShader = CHECK_GRAPHICS_BACKEND_FUNC(glIsShader(shader.ShaderObject))
        if (isShader)
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glAttachShader(program.Program, shader.ShaderObject))
        }
    }

    CHECK_GRAPHICS_BACKEND_FUNC(glLinkProgram(program.Program))

    for (auto &shader : shaders)
    {
        bool isShader = CHECK_GRAPHICS_BACKEND_FUNC(glIsShader(shader.ShaderObject))
        if (isShader)
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glDetachShader(program.Program, shader.ShaderObject))
        }
    }

    int isLinked;
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramiv(program.Program, GL_LINK_STATUS, &isLinked))
    if (!isLinked)
    {
        int infoLogLength;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramiv(program.Program, GL_INFO_LOG_LENGTH, &infoLogLength))

        std::string logMsg(infoLogLength + 1, ' ');
        CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramInfoLog(program.Program, infoLogLength, nullptr, &logMsg[0]))

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
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteShader(shader.ShaderObject))
}

void GraphicsBackendOpenGL::DeleteProgram(GraphicsBackendProgram program)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteProgram(program.Program))

    auto blendState = reinterpret_cast<BlendState*>(program.BlendState);
    delete blendState;
}

bool GraphicsBackendOpenGL::RequireStrictPSODescriptor()
{
    return false;
}

void GraphicsBackendOpenGL::UseProgram(GraphicsBackendProgram program)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glUseProgram(program.Program))

    auto blendState = reinterpret_cast<BlendState*>(program.BlendState);
    if (blendState)
    {
        if (blendState->Enabled)
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glEnable(GL_BLEND))
            CHECK_GRAPHICS_BACKEND_FUNC(glBlendFunc(blendState->SourceFactor, blendState->DestinationFactor))
        }
        else
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glDisable(GL_BLEND))
        }
    }
}

void GraphicsBackendOpenGL::SetClearColor(float r, float g, float b, float a)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glClearColor(r, g, b, a))
}

void GraphicsBackendOpenGL::SetClearDepth(double depth)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glClearDepth(depth))
}

void GraphicsBackendOpenGL::DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int count)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindVertexArray(geometry.VertexArrayObject))
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawArrays(OpenGLHelpers::ToPrimitiveType(primitiveType), firstIndex, count))
}

void GraphicsBackendOpenGL::DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindVertexArray(geometry.VertexArrayObject))
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawArraysInstanced(OpenGLHelpers::ToPrimitiveType(primitiveType), firstIndex, indicesCount, instanceCount))
}

void GraphicsBackendOpenGL::DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindVertexArray(geometry.VertexArrayObject))
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawElements(OpenGLHelpers::ToPrimitiveType(primitiveType), elementsCount, OpenGLHelpers::ToIndicesDataType(dataType), nullptr))
}

void GraphicsBackendOpenGL::DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindVertexArray(geometry.VertexArrayObject))
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawElementsInstanced(OpenGLHelpers::ToPrimitiveType(primitiveType), elementsCount, OpenGLHelpers::ToIndicesDataType(dataType), nullptr, instanceCount))
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

    CHECK_GRAPHICS_BACKEND_FUNC(glBindFramebuffer(GL_READ_FRAMEBUFFER, s_Framebuffers[0]))
    AttachTextureToFramebuffer(GL_READ_FRAMEBUFFER, glAttachment, source.Type, source.Texture, 0, 0);

    GLuint destinationFramebuffer = destinationDescriptor.IsBackbuffer ? 0 : s_Framebuffers[1];
    CHECK_GRAPHICS_BACKEND_FUNC(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destinationFramebuffer))
    if (!destinationDescriptor.IsBackbuffer)
    {
        AttachTextureToFramebuffer(GL_DRAW_FRAMEBUFFER, glAttachment, destinationDescriptor.Texture.Type, destinationDescriptor.Texture.Texture, 0, 0);
    }

    CHECK_GRAPHICS_BACKEND_FUNC(glBlitFramebuffer(sourceX, sourceX, sourceX + width, sourceY + height, destinationX, destinationY, destinationX + width, destinationY + height, mask, GL_NEAREST))
}

void GraphicsBackendOpenGL::PushDebugGroup(const std::string &name, int id)
{
#ifdef GL_KHR_debug
    CHECK_GRAPHICS_BACKEND_FUNC(glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, id, -1, name.c_str()))
#endif
}

void GraphicsBackendOpenGL::PopDebugGroup()
{
#ifdef GL_KHR_debug
    CHECK_GRAPHICS_BACKEND_FUNC(glPopDebugGroup())
#endif
}

void GraphicsBackendOpenGL::BeginRenderPass()
{
    GLbitfield clearFlag = 0;

    int maxAttachments = static_cast<int>(FramebufferAttachment::MAX);
    for (int i = 0; i < maxAttachments; ++i)
    {
        clearFlag |= s_ClearFlags[i];
    }

    if (clearFlag != 0)
    {
        if ((clearFlag & (GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) != 0)
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glDepthMask(GL_TRUE))
        }
        CHECK_GRAPHICS_BACKEND_FUNC(glClear(clearFlag))
    }
}

void GraphicsBackendOpenGL::EndRenderPass()
{
    ResetClearFlags();
}

void GraphicsBackendOpenGL::BeginCopyPass()
{
}

void GraphicsBackendOpenGL::EndCopyPass()
{
}

GraphicsBackendDepthStencilState GraphicsBackendOpenGL::CreateDepthStencilState(bool depthWrite, DepthFunction depthFunction)
{
    auto glState = new DepthStencilState();
    glState->DepthFunction = OpenGLHelpers::ToDepthCompareFunction(depthFunction);
    glState->DepthWrite = depthWrite ? GL_TRUE : GL_FALSE;

    GraphicsBackendDepthStencilState state;
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
        CHECK_GRAPHICS_BACKEND_FUNC(glDepthFunc(glState->DepthFunction))
        CHECK_GRAPHICS_BACKEND_FUNC(glDepthMask(glState->DepthWrite))
    }
}

#endif // RENDER_BACKEND_OPENGL