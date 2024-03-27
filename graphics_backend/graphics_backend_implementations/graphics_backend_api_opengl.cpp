#include "graphics_backend_api_opengl.h"
#include "graphics_backend_debug.h"
#include "enums/texture_unit.h"
#include "enums/uniform_data_type.h"
#include "types/graphics_backend_texture.h"
#include "types/graphics_backend_sampler.h"
#include "types/graphics_backend_buffer.h"
#include "types/graphics_backend_framebuffer.h"
#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_uniform_location.h"
#include "types/graphics_backend_vao.h"

#include <set>
#include <type_traits>
#include <cstring>

GraphicsBackendVAO GraphicsBackendVAO::NONE = GraphicsBackendVAO();
GraphicsBackendBuffer GraphicsBackendBuffer::NONE = GraphicsBackendBuffer();
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

const std::string &GraphicsBackendOpenGL::GetShadingLanguageDirective()
{
    static std::string languageDirective = "#version " + std::to_string(GetMajorVersion() * 100 + GetMinorVersion() * 10);
    return languageDirective;
}

GraphicsBackendName GraphicsBackendOpenGL::GetName()
{
    return GraphicsBackendName::OPENGL;
}

void GraphicsBackendOpenGL::PlatformDependentSetup(void *commandBufferPtr, void *backbufferDescriptor)
{
}

void GraphicsBackendOpenGL::GenerateTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenTextures(texturesCount, reinterpret_cast<GLuint *>(texturesPtr)))
}

void GraphicsBackendOpenGL::GenerateSampler(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenSamplers(samplersCount, reinterpret_cast<GLuint *>(samplersPtr)))
}

void GraphicsBackendOpenGL::DeleteTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteTextures(texturesCount, reinterpret_cast<GLuint *>(texturesPtr)))
}

void GraphicsBackendOpenGL::DeleteSamplers(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteSamplers(samplersCount, reinterpret_cast<GLuint *>(samplersPtr)))
}

void GraphicsBackendOpenGL::BindTexture(TextureType type, GraphicsBackendTexture texture)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(Cast(type), texture.Texture))
}

void GraphicsBackendOpenGL::BindSampler(TextureUnit unit, GraphicsBackendSampler sampler)
{
    auto unitIndex = TextureUnitUtils::TextureUnitToIndex(unit);
    CHECK_GRAPHICS_BACKEND_FUNC(glBindSampler(unitIndex, sampler.Sampler))
}

void GraphicsBackendOpenGL::GenerateMipmaps(TextureType type)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenerateMipmap(Cast(type)))
}

void GraphicsBackendOpenGL::SetTextureParameterInt(TextureType type, TextureParameter parameter, int value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glTexParameteri(Cast(type), Cast(parameter), value))
}

void GraphicsBackendOpenGL::SetSamplerParameterInt(GraphicsBackendSampler sampler, SamplerParameter parameter, int value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameteri(sampler.Sampler, Cast(parameter), value))
}

void GraphicsBackendOpenGL::SetSamplerParameterFloatArray(GraphicsBackendSampler sampler, SamplerParameter parameter, const float *valueArray)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameterfv(sampler.Sampler, Cast(parameter), valueArray))
}

void GraphicsBackendOpenGL::GetTextureLevelParameterInt(TextureTarget target, int level, TextureLevelParameter parameter, int *outValues)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetTexLevelParameteriv(Cast(target), level, Cast(parameter), outValues))
}

void GraphicsBackendOpenGL::TextureImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, TexturePixelFormat pixelFormat, TextureDataType dataType,
                                     const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glTexImage2D(Cast(target), level, Cast(textureFormat), width, height, border, Cast(pixelFormat), Cast(dataType), pixelsData))
}

void GraphicsBackendOpenGL::TextureImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, TexturePixelFormat pixelFormat, TextureDataType dataType,
                                     const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glTexImage3D(Cast(target), level, Cast(textureFormat), width, height, depth, border, Cast(pixelFormat), Cast(dataType), pixelsData))
}

void GraphicsBackendOpenGL::TextureCompressedImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, int imageSize, const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCompressedTexImage2D(Cast(target), level, Cast(textureFormat), width, height, border, imageSize, pixelsData))
}

void GraphicsBackendOpenGL::TextureCompressedImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, int imageSize, const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCompressedTexImage3D(Cast(target), level, Cast(textureFormat), width, height, depth, border, imageSize, pixelsData))
}

void GraphicsBackendOpenGL::GetTextureImage(TextureTarget target, int level, TexturePixelFormat pixelFormat, TextureDataType dataType, void *outPixels)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetTexImage(Cast(target), level, Cast(pixelFormat), Cast(dataType), outPixels))
}

void GraphicsBackendOpenGL::GetCompressedTextureImage(TextureTarget target, int level, void *outPixels)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetCompressedTexImage(Cast(target), level, outPixels))
}

void GraphicsBackendOpenGL::SetActiveTextureUnit(TextureUnit unit)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glActiveTexture(Cast(unit)))
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

void GraphicsBackendOpenGL::GenerateBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenBuffers(buffersCount, reinterpret_cast<GLuint *>(buffersPtr)))
}

void GraphicsBackendOpenGL::DeleteBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteBuffers(buffersCount, reinterpret_cast<GLuint *>(buffersPtr)))
}

void GraphicsBackendOpenGL::BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(Cast(target), buffer.Buffer))
}

void GraphicsBackendOpenGL::BindBufferRange(BufferBindTarget target, int bindingPointIndex, GraphicsBackendBuffer buffer, int offset, int size)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBufferRange(Cast(target), bindingPointIndex, buffer.Buffer, offset, size))
}

void GraphicsBackendOpenGL::SetBufferData(BufferBindTarget target, long size, const void *data, BufferUsageHint usageHint)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBufferData(Cast(target), size, data, Cast(usageHint)))
}

void GraphicsBackendOpenGL::SetBufferSubData(BufferBindTarget target, long offset, long size, const void *data)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBufferSubData(Cast(target), offset, size, data))
}

void GraphicsBackendOpenGL::CopyBufferSubData(BufferBindTarget sourceTarget, BufferBindTarget destinationTarget, int sourceOffset, int destinationOffset, int size)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCopyBufferSubData(Cast(sourceTarget), Cast(destinationTarget), sourceOffset, destinationOffset, size))
}

void GraphicsBackendOpenGL::GenerateVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenVertexArrays(vaoCount, reinterpret_cast<GLuint *>(vaoPtr)))
}

void GraphicsBackendOpenGL::DeleteVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteVertexArrays(vaoCount, reinterpret_cast<GLuint *>(vaoPtr)))
}

void GraphicsBackendOpenGL::BindVertexArrayObject(GraphicsBackendVAO vao)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindVertexArray(vao.VAO))
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

std::string GetShaderTypeName(ShaderType shaderType)
{
    switch (shaderType)
    {
        case ShaderType::VERTEX_SHADER:
            return "Vertex";
        case ShaderType::FRAGMENT_SHADER:
            return "Fragment";
        case ShaderType::GEOMETRY_SHADER:
            return "Geometry";
        default:
            return "Unknown";
    }
}

GLuint ToOpenGLShaderType(ShaderType shaderType)
{
    switch (shaderType)
    {
        case ShaderType::VERTEX_SHADER:
            return GL_VERTEX_SHADER;
        case ShaderType::TESS_CONTROL_SHADER:
            return GL_TESS_CONTROL_SHADER;
        case ShaderType::TESS_EVALUATION_SHADER:
            return GL_TESS_EVALUATION_SHADER;
        case ShaderType::GEOMETRY_SHADER:
            return GL_GEOMETRY_SHADER;
        case ShaderType::FRAGMENT_SHADER:
            return GL_FRAGMENT_SHADER;
    }
}

GraphicsBackendShaderObject GraphicsBackendOpenGL::CompileShader(ShaderType shaderType, const std::string &source)
{
    GraphicsBackendShaderObject shaderObject{};
    shaderObject.ShaderObject = CHECK_GRAPHICS_BACKEND_FUNC(glCreateShader(ToOpenGLShaderType(shaderType)))

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

        throw std::runtime_error(GetShaderTypeName(shaderType) + " shader compilation failed with errors:\n" + logMsg);
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

void GraphicsBackendOpenGL::GetProgramParameter(GraphicsBackendProgram program, ProgramParameter parameter, int *value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramiv(program.Program, Cast(parameter), value))
}

bool GraphicsBackendOpenGL::TryGetUniformBlockIndex(GraphicsBackendProgram program, const char *name, int *index)
{
    *index = CHECK_GRAPHICS_BACKEND_FUNC(glGetUniformBlockIndex(program.Program, name))
    return *index != GL_INVALID_INDEX;
}

void GraphicsBackendOpenGL::SetUniformBlockBinding(GraphicsBackendProgram program, int uniformBlockIndex, int uniformBlockBinding)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glUniformBlockBinding(program.Program, uniformBlockIndex, uniformBlockBinding))
}

void GraphicsBackendOpenGL::GetActiveUniform(GraphicsBackendProgram program, int index, int nameBufferSize, int *nameLength, int *size, UniformDataType *dataType, char *name)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniform(program.Program, index, nameBufferSize, nameLength, size, reinterpret_cast<GLenum*>(dataType), name))
}

void GraphicsBackendOpenGL::GetActiveUniformsParameter(GraphicsBackendProgram program, int uniformCount, const unsigned int *uniformIndices, UniformParameter parameter, int *values)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformsiv(program.Program, uniformCount, uniformIndices, Cast(parameter), values))
}

void GraphicsBackendOpenGL::GetActiveUniformBlockName(GraphicsBackendProgram program, int uniformBlockIndex, int nameBufferSize, int *nameLength, char *name)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformBlockName(program.Program, uniformBlockIndex, nameBufferSize, nameLength, name))
}

GraphicsBackendUniformLocation GraphicsBackendOpenGL::GetUniformLocation(GraphicsBackendProgram program, const char *uniformName)
{
    GraphicsBackendUniformLocation location{};
    location.UniformLocation = CHECK_GRAPHICS_BACKEND_FUNC(glGetUniformLocation(program.Program, uniformName))
    return location;
}

void GraphicsBackendOpenGL::UseProgram(GraphicsBackendProgram program)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glUseProgram(program.Program))
}

void GraphicsBackendOpenGL::SetUniform(GraphicsBackendUniformLocation location, UniformDataType dataType, int count, const void *data, bool transpose)
{
    GLboolean transposeFlag = transpose ? GL_TRUE : GL_FALSE;

    switch (dataType)
    {
        case UniformDataType::FLOAT:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform1fv(location.UniformLocation, count, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_VEC2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform2fv(location.UniformLocation, count, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_VEC3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform3fv(location.UniformLocation, count, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_VEC4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform4fv(location.UniformLocation, count, reinterpret_cast<const GLfloat *>(data)))
            break;

        case UniformDataType::DOUBLE:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform1dv(location.UniformLocation, count, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_VEC2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform2dv(location.UniformLocation, count, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_VEC3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform3dv(location.UniformLocation, count, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_VEC4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform4dv(location.UniformLocation, count, reinterpret_cast<const GLdouble *>(data)))
            break;

        case UniformDataType::INT:
        case UniformDataType::BOOL:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform1iv(location.UniformLocation, count, reinterpret_cast<const GLint *>(data)))
            break;
        case UniformDataType::INT_VEC2:
        case UniformDataType::BOOL_VEC2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform2iv(location.UniformLocation, count, reinterpret_cast<const GLint *>(data)))
            break;
        case UniformDataType::INT_VEC3:
        case UniformDataType::BOOL_VEC3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform3iv(location.UniformLocation, count, reinterpret_cast<const GLint *>(data)))
            break;
        case UniformDataType::INT_VEC4:
        case UniformDataType::BOOL_VEC4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform4iv(location.UniformLocation, count, reinterpret_cast<const GLint *>(data)))
            break;

        case UniformDataType::UNSIGNED_INT:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform1uiv(location.UniformLocation, count, reinterpret_cast<const GLuint *>(data)))
            break;
        case UniformDataType::UNSIGNED_INT_VEC2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform2uiv(location.UniformLocation, count, reinterpret_cast<const GLuint *>(data)))
            break;
        case UniformDataType::UNSIGNED_INT_VEC3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform3uiv(location.UniformLocation, count, reinterpret_cast<const GLuint *>(data)))
            break;
        case UniformDataType::UNSIGNED_INT_VEC4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform4uiv(location.UniformLocation, count, reinterpret_cast<const GLuint *>(data)))
            break;

        case UniformDataType::FLOAT_MAT2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2fv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3fv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4fv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT2x3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2x3fv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT2x4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2x4fv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT3x2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3x2fv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT3x4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3x4fv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT4x2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4x2fv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;
        case UniformDataType::FLOAT_MAT4x3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4x3fv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLfloat *>(data)))
            break;

        case UniformDataType::DOUBLE_MAT2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2dv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3dv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4dv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT2x3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2x3dv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT2x4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix2x4dv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT3x2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3x2dv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT3x4:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix3x4dv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT4x2:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4x2dv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
            break;
        case UniformDataType::DOUBLE_MAT4x3:
        CHECK_GRAPHICS_BACKEND_FUNC(glUniformMatrix4x3dv(location.UniformLocation, count, transposeFlag, reinterpret_cast<const GLdouble *>(data)))
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
        CHECK_GRAPHICS_BACKEND_FUNC(glUniform1i(location.UniformLocation, *(reinterpret_cast<const GLint *>(data))))
            break;
    }
}

void GraphicsBackendOpenGL::GetActiveUniformBlockParameter(GraphicsBackendProgram program, int uniformBlockIndex, UniformBlockParameter parameter, int *values)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformBlockiv(program.Program, uniformBlockIndex, Cast(parameter), values))
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

void GraphicsBackendOpenGL::DrawArrays(PrimitiveType primitiveType, int firstIndex, int count)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawArrays(Cast(primitiveType), firstIndex, count))
}

void GraphicsBackendOpenGL::DrawArraysInstanced(PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawArraysInstanced(Cast(primitiveType), firstIndex, indicesCount, instanceCount))
}

void GraphicsBackendOpenGL::DrawElements(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawElements(Cast(primitiveType), elementsCount, Cast(dataType), indices))
}

void GraphicsBackendOpenGL::DrawElementsInstanced(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices, int instanceCount)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawElementsInstanced(Cast(primitiveType), elementsCount, Cast(dataType), indices, instanceCount))
}

void GraphicsBackendOpenGL::GetProgramInterfaceParameter(GraphicsBackendProgram program, ProgramInterface interface, ProgramInterfaceParameter parameter, int *outValues)
{
#ifdef GL_ARB_program_interface_query
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramInterfaceiv(program.Program, Cast(interface), Cast(parameter), outValues))
#else
    if (outValues)
    {
        *outValues = 0;
    }
#endif
}

void GraphicsBackendOpenGL::GetProgramResourceParameters(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int parametersCount, ProgramResourceParameter *parameters, int bufferSize, int *lengths, int *outValues)
{
#ifdef GL_ARB_program_interface_query
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramResourceiv(program.Program, Cast(interface), resourceIndex, parametersCount, Cast(parameters), bufferSize, lengths, outValues))
#else
    if (lengths)
    {
        *lengths = 0;
    }
    if (outValues)
    {
        std::memset(outValues, 0, sizeof(int) * bufferSize);
    }
#endif
}

void GraphicsBackendOpenGL::GetProgramResourceName(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int bufferSize, int *outLength, char *outName)
{
#ifdef GL_ARB_program_interface_query
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramResourceName(program.Program, Cast(interface), resourceIndex, bufferSize, outLength, outName))
#else
    if (outLength)
    {
        *outLength = 0;
    }
    if (outName)
    {
        std::memset(outName, 0, bufferSize);
    }
#endif
}

bool GraphicsBackendOpenGL::SupportShaderStorageBuffer()
{
    static bool result = extensions.contains("GL_ARB_shader_storage_buffer_object") &&
                         extensions.contains("GL_ARB_program_interface_query"); // required to query info about SSBO in shaders
    return result;
}

void GraphicsBackendOpenGL::SetShaderStorageBlockBinding(GraphicsBackendProgram program, int blockIndex, int blockBinding)
{
#ifdef GL_ARB_shader_storage_buffer_object
    CHECK_GRAPHICS_BACKEND_FUNC(glShaderStorageBlockBinding(program.Program, blockIndex, blockBinding))
#endif
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

GRAPHICS_BACKEND_TYPE_ENUM GraphicsBackendOpenGL::GetError()
{
    return glGetError();
}

const char *GraphicsBackendOpenGL::GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error)
{
    return reinterpret_cast<const char *>(gluGetString(error));
}
