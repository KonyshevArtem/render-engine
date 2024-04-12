#include "graphics_backend_api_opengl.h"
#include "graphics_backend_debug.h"
#include "enums/uniform_data_type.h"
#include "enums/primitive_type.h"
#include "enums/indices_data_type.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_framebuffer.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_geometry.h"
#include "types/graphics_backend_uniform_info.h"
#include "types/graphics_backend_buffer_info.h"
#include "helpers/opengl_helpers.h"

#include <set>
#include <type_traits>

GraphicsBackendTexture GraphicsBackendTexture::NONE = GraphicsBackendTexture();
GraphicsBackendFramebuffer GraphicsBackendFramebuffer::NONE = GraphicsBackendFramebuffer();

std::set<std::string> extensions;

#ifdef RENDER_ENGINE_EDITOR
#define CHECK_GRAPHICS_BACKEND_FUNC(backendFunction)                   \
    backendFunction;                                                   \
    {                                                                  \
        auto error = GraphicsBackendOpenGL::GetError();                \
        if (error != 0)                                                \
            GraphicsBackendDebug::LogError(error, __FILE__, __LINE__); \
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

void GraphicsBackendOpenGL::Init(void *device)
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
        extensions.insert(std::string(reinterpret_cast<const char *>(ext)));
    }
}

int GraphicsBackendOpenGL::GetMajorVersion()
{
    return OPENGL_MAJOR_VERSION;
}

int GraphicsBackendOpenGL::GetMinorVersion()
{
    return OPENGL_MINOR_VERSION;
}

const std::string &GraphicsBackendOpenGL::GetGLSLVersionString()
{
    static std::string glslVersion = "#version " + std::to_string(GetMajorVersion() * 100 + GetMinorVersion() * 10);
    return glslVersion;
}

const std::string &GraphicsBackendOpenGL::GetShadingLanguageDirective()
{
    static std::string languageDirective = GetGLSLVersionString() + "\n#define OPENGL_SHADER\n";
    return languageDirective;
}

GraphicsBackendName GraphicsBackendOpenGL::GetName()
{
    return GraphicsBackendName::OPENGL;
}

void GraphicsBackendOpenGL::PlatformDependentSetup(void *commandBufferPtr, void *backbufferDescriptor)
{
}

GraphicsBackendTexture GraphicsBackendOpenGL::CreateTexture(int width, int height, TextureType type, TextureInternalFormat format, int mipLevels)
{
    GraphicsBackendTexture texture{};
    CHECK_GRAPHICS_BACKEND_FUNC(glGenTextures(1, reinterpret_cast<GLuint *>(&texture.Texture)))

    GLenum textureType = OpenGLHelpers::ToTextureType(type);
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(textureType, texture.Texture))
    CHECK_GRAPHICS_BACKEND_FUNC(glTexParameteri(textureType, GL_TEXTURE_BASE_LEVEL, 0))
    CHECK_GRAPHICS_BACKEND_FUNC(glTexParameteri(textureType, GL_TEXTURE_MAX_LEVEL, mipLevels - 1))

    texture.Type = type;
    texture.Format = format;
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

void GraphicsBackendOpenGL::BindTexture(const GraphicsBackendResourceBindings &bindings, int uniformLocation, const GraphicsBackendTexture &texture)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glActiveTexture(OpenGLHelpers::ToTextureUnit(bindings.FragmentIndex)))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(OpenGLHelpers::ToTextureType(texture.Type), texture.Texture))
    CHECK_GRAPHICS_BACKEND_FUNC(glUniform1i(uniformLocation, bindings.FragmentIndex))
}

void GraphicsBackendOpenGL::BindSampler(const GraphicsBackendResourceBindings &bindings, const GraphicsBackendSampler &sampler)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glActiveTexture(OpenGLHelpers::ToTextureUnit(bindings.FragmentIndex)))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindSampler(bindings.FragmentIndex, sampler.Sampler))
}

void GraphicsBackendOpenGL::GenerateMipmaps(const GraphicsBackendTexture &texture)
{
    GLenum textureType = OpenGLHelpers::ToTextureType(texture.Type);
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(textureType, texture.Texture))
    CHECK_GRAPHICS_BACKEND_FUNC(glGenerateMipmap(textureType))
}

void GraphicsBackendOpenGL::UploadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, int width, int height, int depth, int imageSize, const void *pixelsData)
{
    GLenum type = OpenGLHelpers::ToTextureType(texture.Type);
    GLenum target = OpenGLHelpers::ToTextureTarget(texture.Type, slice);
    GLenum internalFormat = OpenGLHelpers::ToTextureInternalFormat(texture.Format);
    bool isImage3D = texture.Type == TextureType::TEXTURE_2D_ARRAY;

    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(type, texture.Texture))
    if (OpenGLHelpers::IsCompressedTextureFormat(texture.Format) && imageSize != 0)
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

void GraphicsBackendOpenGL::DownloadImagePixels(const GraphicsBackendTexture &texture, int level, int slice, void *outPixels)
{
    GLenum type = OpenGLHelpers::ToTextureType(texture.Type);
    GLenum target = OpenGLHelpers::ToTextureTarget(texture.Type, slice);

    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(type, texture.Texture))
    if (OpenGLHelpers::IsCompressedTextureFormat(texture.Format))
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glGetCompressedTexImage(target, level, outPixels))
    }
    else
    {
        GLenum pixelFormat = OpenGLHelpers::ToTextureFormat(texture.Format);
        GLenum dataType = OpenGLHelpers::ToTextureDataType(texture.Format);
        CHECK_GRAPHICS_BACKEND_FUNC(glGetTexImage(target, level, pixelFormat, dataType, outPixels))
    }
}

TextureInternalFormat GraphicsBackendOpenGL::GetTextureFormat(const GraphicsBackendTexture &texture)
{
    GLenum type = OpenGLHelpers::ToTextureType(texture.Type);
    GLenum target = OpenGLHelpers::ToTextureTarget(texture.Type, 0);

    GLint internalFormat;
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(type, texture.Texture))
    CHECK_GRAPHICS_BACKEND_FUNC(glGetTexLevelParameteriv(target, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat))
    return OpenGLHelpers::FromTextureInternalFormat(internalFormat);
}

int GraphicsBackendOpenGL::GetTextureSize(const GraphicsBackendTexture &texture, int level, int slice)
{
    GLenum type = OpenGLHelpers::ToTextureType(texture.Type);
    GLenum target = OpenGLHelpers::ToTextureTarget(texture.Type, 0);

    int size = 0;
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(type, texture.Texture))
    if (OpenGLHelpers::IsCompressedTextureFormat(texture.Format))
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glGetTexLevelParameteriv(target, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &size))
    }

    return size;
}

void GraphicsBackendOpenGL::GenerateFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenFramebuffers(count, reinterpret_cast<GLuint *>(framebuffersPtr)))
}

void GraphicsBackendOpenGL::DeleteFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteFramebuffers(count, reinterpret_cast<GLuint *>(framebuffersPtr)))
}

void GraphicsBackendOpenGL::BindFramebuffer(FramebufferTarget target, GraphicsBackendFramebuffer framebuffer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindFramebuffer(Cast(target), framebuffer.Framebuffer))
}

void GraphicsBackendOpenGL::SetFramebufferTexture(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glFramebufferTexture(Cast(target), Cast(attachment), texture.Texture, level))
}

void GraphicsBackendOpenGL::SetFramebufferTextureLayer(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level, int layer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glFramebufferTextureLayer(Cast(target), Cast(attachment), texture.Texture, level, layer))
}

GraphicsBackendBuffer GraphicsBackendOpenGL::CreateBuffer(int size, BufferBindTarget bindTarget, BufferUsageHint usageHint)
{
    GraphicsBackendBuffer buffer{};
    CHECK_GRAPHICS_BACKEND_FUNC(glGenBuffers(1, reinterpret_cast<GLuint*>(&buffer.Buffer)))

    buffer.BindTarget = bindTarget;
    buffer.UsageHint = usageHint;
    buffer.IsDataInitialized = false;
    buffer.Size = size;
    return buffer;
}

void GraphicsBackendOpenGL::DeleteBuffer(const GraphicsBackendBuffer &buffer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteBuffers(1, reinterpret_cast<const GLuint *>(&buffer.Buffer)))
}

void GraphicsBackendOpenGL::BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(OpenGLHelpers::ToBufferBindTarget(target), buffer.Buffer))
}

void GraphicsBackendOpenGL::BindBufferRange(const GraphicsBackendBuffer &buffer, GraphicsBackendResourceBindings bindings, int offset, int size)
{
    auto bindTarget = OpenGLHelpers::ToBufferBindTarget(buffer.BindTarget);
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(bindTarget, buffer.Buffer))
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBufferRange(bindTarget, bindings.VertexIndex, buffer.Buffer, offset, size))
}

void GraphicsBackendOpenGL::SetBufferData(GraphicsBackendBuffer &buffer, long offset, long size, const void *data)
{
    GLenum bindTarget = OpenGLHelpers::ToBufferBindTarget(buffer.BindTarget);
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(bindTarget, buffer.Buffer))
    if (!buffer.IsDataInitialized)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glBufferData(bindTarget, buffer.Size, nullptr, OpenGLHelpers::ToBufferUsageHint(buffer.UsageHint)))
        buffer.IsDataInitialized = true;
    }
    CHECK_GRAPHICS_BACKEND_FUNC(glBufferSubData(bindTarget, offset, size, data))
}

void GraphicsBackendOpenGL::CopyBufferSubData(BufferBindTarget sourceTarget, BufferBindTarget destinationTarget, int sourceOffset, int destinationOffset, int size)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCopyBufferSubData(OpenGLHelpers::ToBufferBindTarget(sourceTarget), OpenGLHelpers::ToBufferBindTarget(destinationTarget), sourceOffset, destinationOffset, size))
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

void GraphicsBackendOpenGL::SetCapability(GraphicsBackendCapability capability, bool enabled)
{
    if (enabled)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glEnable(Cast(capability)))
    }
    else
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glDisable(Cast(capability)))
    }
}

void GraphicsBackendOpenGL::SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBlendFunc(Cast(sourceFactor), Cast(destinationFactor)))
}

void GraphicsBackendOpenGL::SetCullFace(CullFace cullFace)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCullFace(Cast(cullFace)))
}

void GraphicsBackendOpenGL::SetCullFaceOrientation(CullFaceOrientation orientation)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glFrontFace(Cast(orientation)))
}

void GraphicsBackendOpenGL::SetDepthFunction(DepthFunction function)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDepthFunc(Cast(function)))
}

void GraphicsBackendOpenGL::SetDepthWrite(bool enabled)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDepthMask(enabled ? GL_TRUE : GL_FALSE))
}

void GraphicsBackendOpenGL::SetDepthRange(double near, double far)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDepthRange(near, far))
}

void GraphicsBackendOpenGL::SetViewport(int x, int y, int width, int height)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glViewport(x, y, width, height))
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

GraphicsBackendProgram GraphicsBackendOpenGL::CreateProgram(GraphicsBackendShaderObject *shaders, int shadersCount)
{
    GraphicsBackendProgram program{};
    program.Program = CHECK_GRAPHICS_BACKEND_FUNC(glCreateProgram())

    for (int i = 0; i < shadersCount; ++i)
    {
        auto shader = *(shaders + i);
        bool isShader = CHECK_GRAPHICS_BACKEND_FUNC(glIsShader(shader.ShaderObject))
        if (isShader)
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glAttachShader(program.Program, shader.ShaderObject))
        }
    }

    CHECK_GRAPHICS_BACKEND_FUNC(glLinkProgram(program.Program))

    for (int i = 0; i < shadersCount; ++i)
    {
        auto shader = *(shaders + i);
        bool isShader = CHECK_GRAPHICS_BACKEND_FUNC(glIsShader(shader.ShaderObject))
        if (isShader)
        {
            CHECK_GRAPHICS_BACKEND_FUNC(glDetachShader(program.Program, shader.ShaderObject))
            CHECK_GRAPHICS_BACKEND_FUNC(glDeleteShader(shader.ShaderObject))
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

    return program;
}

void GraphicsBackendOpenGL::DeleteProgram(GraphicsBackendProgram program)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteProgram(program.Program))
}

void GraphicsBackendOpenGL::IntrospectProgram(GraphicsBackendProgram program, std::unordered_map<std::string, GraphicsBackendUniformInfo> &uniforms, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> &buffers)
{
    static const std::string perInstanceStructName = "PerInstance[1]";
    static const std::string perInstanceDataBufferName = "PerInstanceData";

    std::vector<char> nameBuffer(GetNameBufferSize(program));

    // Uniforms

    int uniformCount;
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramiv(program.Program, GL_ACTIVE_UNIFORMS, &uniformCount))

    int binding = 0;
    for (unsigned int i = 0; i < uniformCount; ++i)
    {
        int blockIndex;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformsiv(program.Program, 1, &i, GL_UNIFORM_BLOCK_INDEX, &blockIndex))
        if (blockIndex >= 0)
        {
            continue;
        }

        GraphicsBackendUniformInfo uniformInfo{};

        int uniformNameLength;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniform(program.Program, i, nameBuffer.size(), &uniformNameLength, nullptr, reinterpret_cast<GLenum*>(&uniformInfo.Type), &nameBuffer[0]))
        std::string uniformName(nameBuffer.begin(), nameBuffer.begin() + uniformNameLength);

        uniformInfo.Location = CHECK_GRAPHICS_BACKEND_FUNC(glGetUniformLocation(program.Program, &uniformName[0]))

        // TODO: correctly parse arrays

        if (UniformDataTypeUtils::IsTexture(uniformInfo.Type))
        {
            uniformInfo.IsTexture = true;
            uniformInfo.HasSampler = true;
            uniformInfo.TextureBindings = GraphicsBackendResourceBindings{binding, binding};
            ++binding;
        }

        uniforms[uniformName] = uniformInfo;
    }

    // UBO

    int uniformBlocksCount;
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramiv(program.Program, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlocksCount))

    for (int i = 0; i < uniformBlocksCount; ++i)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformBlockBinding(program.Program, i, i))

        int nameSize;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformBlockName(program.Program, i, nameBuffer.size(), &nameSize, &nameBuffer[0]))
        std::string uniformBlockName(nameBuffer.begin(), nameBuffer.begin() + nameSize);

        int blockSize;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformBlockiv(program.Program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize))

        auto variables = GetUniformBlockVariables(program, i, nameBuffer);

        // special hack for PerInstanceData buffer when SSBO is not supported
        // replace real block size with array stride so each renderer allocates buffer to store only 1 element
        // for SSBO this happens by default, when it has an array without size - returned block size is equal to the size of 1 element
        if (uniformBlockName == perInstanceDataBufferName)
        {
            for (const auto &pair : variables)
            {
                if (pair.first.find(perInstanceStructName) != std::string::npos)
                {
                    blockSize = std::min(blockSize, pair.second);
                }
            }
        }

        auto buffer = std::make_shared<GraphicsBackendBufferInfo>(GraphicsBackendBufferInfo::BufferType::UNIFORM, blockSize, variables);
        buffer->SetBindings({i, i});
        buffers[uniformBlockName] = buffer;
    }

    // SSBO

#ifdef GL_ARB_program_interface_query
    int blocksCount;
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramInterfaceiv(program.Program, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &blocksCount))

    for (int i = 0; i < blocksCount; ++i)
    {
        CHECK_GRAPHICS_BACKEND_FUNC(glShaderStorageBlockBinding(program.Program, i, i))

        int nameSize;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramResourceName(program.Program, GL_SHADER_STORAGE_BLOCK, i, nameBuffer.size(), &nameSize, nameBuffer.data()))
        std::string blockName(nameBuffer.begin(), nameBuffer.begin() + nameSize);

        int blockSize;
        auto blockSizeParameter = GL_BUFFER_DATA_SIZE;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramResourceiv(program.Program, GL_SHADER_STORAGE_BLOCK, i, 1, &blockSizeParameter, 1, nullptr, &blockSize))

        auto variables = GetShaderStorageBlockVariables(program, i);
        auto buffer = std::make_shared<GraphicsBackendBufferInfo>(GraphicsBackendBufferInfo::BufferType::SHADER_STORAGE, blockSize, variables);
        buffer->SetBindings({i, i});
        buffers[blockName] = buffer;
    }
#endif
}

void GraphicsBackendOpenGL::UseProgram(GraphicsBackendProgram program)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glUseProgram(program.Program))
}

void GraphicsBackendOpenGL::SetUniform(int location, UniformDataType dataType, int count, const void *data, bool transpose)
{
    GLboolean transposeFlag = transpose ? GL_TRUE : GL_FALSE;

    switch (dataType)
    {
        case UniformDataType::FLOAT:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform1fv(location, count, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_VEC2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform2fv(location, count, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_VEC3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform3fv(location, count, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_VEC4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform4fv(location, count, reinterpret_cast<const GLfloat *>(data)))
            break;

        case UniformDataType::DOUBLE:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform1dv(location, count, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_VEC2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform2dv(location, count, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_VEC3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform3dv(location, count, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_VEC4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform4dv(location, count, reinterpret_cast<const GLdouble *>(data)))
            break;

        case UniformDataType::INT:
        case UniformDataType::BOOL:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform1iv(location, count, reinterpret_cast<const GLint *>(data)))
            break;
        case UniformDataType::INT_VEC2:
        case UniformDataType::BOOL_VEC2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform2iv(location, count, reinterpret_cast<const GLint *>(data)))
            break;
        case UniformDataType::INT_VEC3:
        case UniformDataType::BOOL_VEC3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform3iv(location, count, reinterpret_cast<const GLint *>(data)))
            break;
        case UniformDataType::INT_VEC4:
        case UniformDataType::BOOL_VEC4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform4iv(location, count, reinterpret_cast<const GLint *>(data)))
            break;

        case UniformDataType::UNSIGNED_INT:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform1uiv(location, count, reinterpret_cast<const GLuint *>(data)))
            break;
        case UniformDataType::UNSIGNED_INT_VEC2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform2uiv(location, count, reinterpret_cast<const GLuint *>(data)))
            break;
        case UniformDataType::UNSIGNED_INT_VEC3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform3uiv(location, count, reinterpret_cast<const GLuint *>(data)))
            break;
        case UniformDataType::UNSIGNED_INT_VEC4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform4uiv(location, count, reinterpret_cast<const GLuint *>(data)))
            break;

        case UniformDataType::FLOAT_MAT2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2fv(location, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3fv(location, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4fv(location, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT2x3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2x3fv(location, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT2x4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2x4fv(location, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT3x2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3x2fv(location, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT3x4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3x4fv(location, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT4x2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4x2fv(location, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT4x3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4x3fv(location, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;

        case UniformDataType::DOUBLE_MAT2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2dv(location, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3dv(location, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4dv(location, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT2x3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2x3dv(location, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT2x4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2x4dv(location, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT3x2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3x2dv(location, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT3x4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3x4dv(location, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT4x2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4x2dv(location, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT4x3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4x3dv(location, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;

        case UniformDataType::SAMPLER_1D:
        case UniformDataType::SAMPLER_2D:
        case UniformDataType::SAMPLER_3D:
        case UniformDataType::SAMPLER_1D_SHADOW:
        case UniformDataType::SAMPLER_2D_SHADOW:
        case UniformDataType::SAMPLER_1D_ARRAY:
        case UniformDataType::SAMPLER_1D_ARRAY_SHADOW:
        case UniformDataType::SAMPLER_2D_ARRAY:
        case UniformDataType::SAMPLER_2D_ARRAY_SHADOW:
        case UniformDataType::SAMPLER_2D_MULTISAMPLE:
        case UniformDataType::SAMPLER_2D_MULTISAMPLE_ARRAY:
        case UniformDataType::SAMPLER_CUBE:
        case UniformDataType::SAMPLER_CUBE_SHADOW:
        case UniformDataType::SAMPLER_BUFFER:
        case UniformDataType::SAMPLER_2D_RECT:
        case UniformDataType::SAMPLER_2D_RECT_SHADOW:
        case UniformDataType::INT_SAMPLER_1D:
        case UniformDataType::INT_SAMPLER_2D:
        case UniformDataType::INT_SAMPLER_3D:
        case UniformDataType::INT_SAMPLER_CUBE:
        case UniformDataType::INT_SAMPLER_1D_ARRAY:
        case UniformDataType::INT_SAMPLER_2D_ARRAY:
        case UniformDataType::INT_SAMPLER_2D_MULTISAMPLE:
        case UniformDataType::INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case UniformDataType::INT_SAMPLER_BUFFER:
        case UniformDataType::INT_SAMPLER_2D_RECT:
        case UniformDataType::UNSIGNED_INT_SAMPLER_1D:
        case UniformDataType::UNSIGNED_INT_SAMPLER_2D:
        case UniformDataType::UNSIGNED_INT_SAMPLER_3D:
        case UniformDataType::UNSIGNED_INT_SAMPLER_CUBE:
        case UniformDataType::UNSIGNED_INT_SAMPLER_1D_ARRAY:
        case UniformDataType::UNSIGNED_INT_SAMPLER_2D_ARRAY:
        case UniformDataType::UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
        case UniformDataType::UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case UniformDataType::UNSIGNED_INT_SAMPLER_BUFFER:
        case UniformDataType::UNSIGNED_INT_SAMPLER_2D_RECT:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform1i(location, *(reinterpret_cast<const GLint *>(data))))
            break;
    }
}

void GraphicsBackendOpenGL::Clear(ClearMask mask)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glClear(Cast(mask)))
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

bool GraphicsBackendOpenGL::SupportShaderStorageBuffer()
{
    static bool result = extensions.contains("GL_ARB_shader_storage_buffer_object") &&
                         extensions.contains("GL_ARB_program_interface_query"); // required to query info about SSBO in shaders
    return result;
}

void GraphicsBackendOpenGL::BlitFramebuffer(int srcMinX, int srcMinY, int srcMaxX, int srcMaxY, int dstMinX, int dstMinY, int dstMaxX, int dstMaxY, BlitFramebufferMask mask, BlitFramebufferFilter filter)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBlitFramebuffer(srcMinX, srcMinY, srcMaxX, srcMaxY, dstMinX, dstMinY, dstMaxX, dstMaxY, Cast(mask), Cast(filter)))
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
}

void GraphicsBackendOpenGL::EndRenderPass()
{
}

GRAPHICS_BACKEND_TYPE_ENUM GraphicsBackendOpenGL::GetError()
{
    return glGetError();
}

const char *GraphicsBackendOpenGL::GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error)
{
    return reinterpret_cast<const char *>(gluGetString(error));
}

int GraphicsBackendOpenGL::GetNameBufferSize(GraphicsBackendProgram program)
{
    int uniformNameLength = 0;
    int uniformBlockNameLength = 0;
    int shaderStorageBlockNameLength = 0;
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramiv(program.Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformNameLength))
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramiv(program.Program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &uniformBlockNameLength))
#ifdef GL_ARB_program_interface_query
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramInterfaceiv(program.Program, GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH, &shaderStorageBlockNameLength))
#endif
    return std::max(uniformNameLength, std::max(uniformBlockNameLength, shaderStorageBlockNameLength));
}

std::unordered_map<std::string, int> GraphicsBackendOpenGL::GetUniformBlockVariables(GraphicsBackendProgram program, int uniformBlockIndex, std::vector<char> nameBuffer)
{
    int uniformsCount;
    CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformBlockiv(program.Program, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformsCount))

    std::vector<int> uniformsIndices(uniformsCount);
    CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformBlockiv(program.Program, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, &uniformsIndices[0]))

    std::unordered_map<std::string, int> variables;
    for (unsigned int i = 0; i < uniformsCount; ++i)
    {
        int uniformNameLength;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniform(program.Program, uniformsIndices[i], nameBuffer.size(), &uniformNameLength, nullptr, nullptr, &nameBuffer[0]))
        std::string uniformName(nameBuffer.begin(), nameBuffer.begin() + uniformNameLength);

        int uniformOffset;
        CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformsiv(program.Program, 1, reinterpret_cast<unsigned int *>(&uniformsIndices[i]), GL_UNIFORM_OFFSET, &uniformOffset))

        variables[uniformName] = uniformOffset;
    }

    return variables;
}

std::unordered_map<std::string, int> GraphicsBackendOpenGL::GetShaderStorageBlockVariables(GraphicsBackendProgram program, int ssboIndex)
{
#ifdef GL_ARB_program_interface_query
    int variablesCount;
    auto variablesCountParameter = GL_NUM_ACTIVE_VARIABLES;
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramResourceiv(program.Program, GL_SHADER_STORAGE_BLOCK, ssboIndex, 1, &variablesCountParameter, 1, nullptr, &variablesCount))

    std::vector<int> variablesIndices(variablesCount);
    auto variablesIndicesParameter = GL_ACTIVE_VARIABLES;
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramResourceiv(program.Program, GL_SHADER_STORAGE_BLOCK, ssboIndex, 1, &variablesIndicesParameter, variablesCount, nullptr, &variablesIndices[0]))

    constexpr int variableParametersCount = 2;
    GLenum variableParameters[variableParametersCount] = {GL_NAME_LENGTH, GL_OFFSET};

    std::unordered_map<std::string, int> variables;
    for(int i = 0; i < variablesCount; ++i)
    {
        int values[variableParametersCount];
        CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramResourceiv(program.Program, GL_BUFFER_VARIABLE, variablesIndices[i], variableParametersCount, &variableParameters[0], variableParametersCount, nullptr, &values[0]))

        std::vector<char> nameData(values[0]);
        CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramResourceName(program.Program, GL_BUFFER_VARIABLE, variablesIndices[i], nameData.size(), nullptr, &nameData[0]))
        std::string name(nameData.begin(), nameData.end() - 1);

        variables[name] = values[1];
    }

    return variables;
#else
    return {};
#endif
}