#include "graphics_backend_api.h"
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

GraphicsBackendVAO GraphicsBackendVAO::NONE = GraphicsBackendVAO();
GraphicsBackendBuffer GraphicsBackendBuffer::NONE = GraphicsBackendBuffer();
GraphicsBackendTexture GraphicsBackendTexture::NONE = GraphicsBackendTexture();
GraphicsBackendFramebuffer GraphicsBackendFramebuffer::NONE = GraphicsBackendFramebuffer();

std::set<std::string> extensions;

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

void GraphicsBackend::Init()
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

void GraphicsBackend::GenerateTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenTextures(texturesCount, reinterpret_cast<GLuint *>(texturesPtr)))
}

void GraphicsBackend::GenerateSampler(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenSamplers(samplersCount, reinterpret_cast<GLuint *>(samplersPtr)))
}

void GraphicsBackend::DeleteTextures(uint32_t texturesCount, GraphicsBackendTexture *texturesPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteTextures(texturesCount, reinterpret_cast<GLuint *>(texturesPtr)))
}

void GraphicsBackend::DeleteSamplers(uint32_t samplersCount, GraphicsBackendSampler *samplersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteSamplers(samplersCount, reinterpret_cast<GLuint *>(samplersPtr)))
}

void GraphicsBackend::BindTexture(TextureType type, GraphicsBackendTexture texture)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindTexture(Cast(type), texture.Texture))
}

void GraphicsBackend::BindSampler(TextureUnit unit, GraphicsBackendSampler sampler)
{
    auto unitIndex = TextureUnitUtils::TextureUnitToIndex(unit);
    CHECK_GRAPHICS_BACKEND_FUNC(glBindSampler(unitIndex, sampler.Sampler))
}

void GraphicsBackend::GenerateMipmaps(TextureType type)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenerateMipmap(Cast(type)))
}

void GraphicsBackend::SetTextureParameterInt(TextureType type, TextureParameter parameter, int value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glTexParameteri(Cast(type), Cast(parameter), value))
}

void GraphicsBackend::SetSamplerParameterInt(GraphicsBackendSampler sampler, SamplerParameter parameter, int value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameteri(sampler.Sampler, Cast(parameter), value))
}

void GraphicsBackend::SetSamplerParameterFloatArray(GraphicsBackendSampler sampler, SamplerParameter parameter, const float *valueArray)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glSamplerParameterfv(sampler.Sampler, Cast(parameter), valueArray))
}

void GraphicsBackend::GetTextureLevelParameterInt(TextureTarget target, int level, TextureLevelParameter parameter, int *outValues)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetTexLevelParameteriv(Cast(target), level, Cast(parameter), outValues))
}

void GraphicsBackend::TextureImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, TexturePixelFormat pixelFormat, TextureDataType dataType,
                                     const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glTexImage2D(Cast(target), level, Cast(textureFormat), width, height, border, Cast(pixelFormat), Cast(dataType), pixelsData))
}

void GraphicsBackend::TextureImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, TexturePixelFormat pixelFormat, TextureDataType dataType,
                                     const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glTexImage3D(Cast(target), level, Cast(textureFormat), width, height, depth, border, Cast(pixelFormat), Cast(dataType), pixelsData))
}

void GraphicsBackend::TextureCompressedImage2D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int border, int imageSize, const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCompressedTexImage2D(Cast(target), level, Cast(textureFormat), width, height, border, imageSize, pixelsData))
}

void GraphicsBackend::TextureCompressedImage3D(TextureTarget target, int level, TextureInternalFormat textureFormat, int width, int height, int depth, int border, int imageSize, const void *pixelsData)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCompressedTexImage3D(Cast(target), level, Cast(textureFormat), width, height, depth, border, imageSize, pixelsData))
}

void GraphicsBackend::GetTextureImage(TextureTarget target, int level, TexturePixelFormat pixelFormat, TextureDataType dataType, void *outPixels)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetTexImage(Cast(target), level, Cast(pixelFormat), Cast(dataType), outPixels))
}

void GraphicsBackend::GetCompressedTextureImage(TextureTarget target, int level, void *outPixels)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetCompressedTexImage(Cast(target), level, outPixels))
}

void GraphicsBackend::SetActiveTextureUnit(TextureUnit unit)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glActiveTexture(Cast(unit)))
}

void GraphicsBackend::GenerateFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenFramebuffers(count, reinterpret_cast<GLuint *>(framebuffersPtr)))
}

void GraphicsBackend::DeleteFramebuffers(int count, GraphicsBackendFramebuffer *framebuffersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteFramebuffers(count, reinterpret_cast<GLuint *>(framebuffersPtr)))
}

void GraphicsBackend::BindFramebuffer(FramebufferTarget target, GraphicsBackendFramebuffer framebuffer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindFramebuffer(Cast(target), framebuffer.Framebuffer))
}

void GraphicsBackend::SetFramebufferTexture(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glFramebufferTexture(Cast(target), Cast(attachment), texture.Texture, level))
}

void GraphicsBackend::SetFramebufferTextureLayer(FramebufferTarget target, FramebufferAttachment attachment, GraphicsBackendTexture texture, int level, int layer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glFramebufferTextureLayer(Cast(target), Cast(attachment), texture.Texture, level, layer))
}

void GraphicsBackend::GenerateBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenBuffers(buffersCount, reinterpret_cast<GLuint *>(buffersPtr)))
}

void GraphicsBackend::DeleteBuffers(int buffersCount, GraphicsBackendBuffer *buffersPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteBuffers(buffersCount, reinterpret_cast<GLuint *>(buffersPtr)))
}

void GraphicsBackend::BindBuffer(BufferBindTarget target, GraphicsBackendBuffer buffer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBuffer(Cast(target), buffer.Buffer))
}

void GraphicsBackend::BindBufferRange(BufferBindTarget target, int bindingPointIndex, GraphicsBackendBuffer buffer, int offset, int size)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindBufferRange(Cast(target), bindingPointIndex, buffer.Buffer, offset, size))
}

void GraphicsBackend::SetBufferData(BufferBindTarget target, long size, const void *data, BufferUsageHint usageHint)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBufferData(Cast(target), size, data, Cast(usageHint)))
}

void GraphicsBackend::SetBufferSubData(BufferBindTarget target, long offset, long size, const void *data)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBufferSubData(Cast(target), offset, size, data))
}

void GraphicsBackend::GenerateVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGenVertexArrays(vaoCount, reinterpret_cast<GLuint *>(vaoPtr)))
}

void GraphicsBackend::DeleteVertexArrayObjects(int vaoCount, GraphicsBackendVAO *vaoPtr)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteVertexArrays(vaoCount, reinterpret_cast<GLuint *>(vaoPtr)))
}

void GraphicsBackend::BindVertexArrayObject(GraphicsBackendVAO vao)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBindVertexArray(vao.VAO))
}

void GraphicsBackend::EnableVertexAttributeArray(int index)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glEnableVertexAttribArray(index))
}

void GraphicsBackend::DisableVertexAttributeArray(int index)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDisableVertexAttribArray(index))
}

void GraphicsBackend::SetVertexAttributePointer(int index, int size, VertexAttributeDataType dataType, bool normalized, int stride, const void *pointer)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glVertexAttribPointer(index, size, Cast(dataType), normalized ? GL_TRUE : GL_FALSE, stride, pointer))
}

void GraphicsBackend::SetVertexAttributeDivisor(int index, int divisor)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glVertexAttribDivisor(index, divisor))
}

void GraphicsBackend::SetCapability(GraphicsBackendCapability capability, bool enabled)
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

void GraphicsBackend::SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glBlendFunc(Cast(sourceFactor), Cast(destinationFactor)))
}

void GraphicsBackend::SetCullFace(CullFace cullFace)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCullFace(Cast(cullFace)))
}

void GraphicsBackend::SetCullFaceOrientation(CullFaceOrientation orientation)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glFrontFace(Cast(orientation)))
}

void GraphicsBackend::SetDepthFunction(DepthFunction function)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDepthFunc(Cast(function)))
}

void GraphicsBackend::SetDepthWrite(bool enabled)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDepthMask(enabled ? GL_TRUE : GL_FALSE))
}

void GraphicsBackend::SetDepthRange(double near, double far)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDepthRange(near, far))
}

void GraphicsBackend::SetViewport(int x, int y, int width, int height)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glViewport(x, y, width, height))
}

GraphicsBackendShaderObject GraphicsBackend::CreateShader(ShaderType shaderType)
{
    GraphicsBackendShaderObject shaderObject{};
    shaderObject.ShaderObject = CHECK_GRAPHICS_BACKEND_FUNC(glCreateShader(Cast(shaderType)))
    return shaderObject;
}

void GraphicsBackend::DeleteShader(GraphicsBackendShaderObject shader)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteShader(shader.ShaderObject))
}

void GraphicsBackend::SetShaderSources(GraphicsBackendShaderObject shader, int sourcesCount, const char **sources, const int *sourceLengths)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glShaderSource(shader.ShaderObject, sourcesCount, sources, sourceLengths))
}

void GraphicsBackend::CompileShader(GraphicsBackendShaderObject shader)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glCompileShader(shader.ShaderObject))
}

void GraphicsBackend::GetShaderParameter(GraphicsBackendShaderObject shader, ShaderParameter parameter, int *value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetShaderiv(shader.ShaderObject, Cast(parameter), value))
}

void GraphicsBackend::GetShaderInfoLog(GraphicsBackendShaderObject shader, int maxLength, int *length, char *infoLog)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetShaderInfoLog(shader.ShaderObject, maxLength, length, infoLog))
}

bool GraphicsBackend::IsShader(GraphicsBackendShaderObject shader)
{
    auto isShader = CHECK_GRAPHICS_BACKEND_FUNC(glIsShader(shader.ShaderObject))
    return isShader == GL_TRUE;
}

void GraphicsBackend::AttachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glAttachShader(program.Program, shader.ShaderObject))
}

void GraphicsBackend::DetachShader(GraphicsBackendProgram program, GraphicsBackendShaderObject shader)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDetachShader(program.Program, shader.ShaderObject))
}

GraphicsBackendProgram GraphicsBackend::CreateProgram()
{
    GraphicsBackendProgram program{};
    program.Program = CHECK_GRAPHICS_BACKEND_FUNC(glCreateProgram())
    return program;
}

void GraphicsBackend::DeleteProgram(GraphicsBackendProgram program)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDeleteProgram(program.Program))
}

void GraphicsBackend::LinkProgram(GraphicsBackendProgram program)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glLinkProgram(program.Program))
}

void GraphicsBackend::GetProgramParameter(GraphicsBackendProgram program, ProgramParameter parameter, int *value)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramiv(program.Program, Cast(parameter), value))
}

void GraphicsBackend::GetProgramInfoLog(GraphicsBackendProgram program, int maxLength, int *length, char *infoLog)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramInfoLog(program.Program, maxLength, length, infoLog))
}

bool GraphicsBackend::TryGetUniformBlockIndex(GraphicsBackendProgram program, const char *name, int *index)
{
    *index = CHECK_GRAPHICS_BACKEND_FUNC(glGetUniformBlockIndex(program.Program, name))
    return *index != GL_INVALID_INDEX;
}

void GraphicsBackend::SetUniformBlockBinding(GraphicsBackendProgram program, int uniformBlockIndex, int uniformBlockBinding)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glUniformBlockBinding(program.Program, uniformBlockIndex, uniformBlockBinding))
}

void GraphicsBackend::GetActiveUniform(GraphicsBackendProgram program, int index, int nameBufferSize, int *nameLength, int *size, UniformDataType *dataType, char *name)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniform(program.Program, index, nameBufferSize, nameLength, size, reinterpret_cast<GLenum*>(dataType), name))
}

void GraphicsBackend::GetActiveUniformsParameter(GraphicsBackendProgram program, int uniformCount, const unsigned int *uniformIndices, UniformParameter parameter, int *values)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformsiv(program.Program, uniformCount, uniformIndices, Cast(parameter), values))
}

void GraphicsBackend::GetActiveUniformBlockName(GraphicsBackendProgram program, int uniformBlockIndex, int nameBufferSize, int *nameLength, char *name)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformBlockName(program.Program, uniformBlockIndex, nameBufferSize, nameLength, name))
}

GraphicsBackendUniformLocation GraphicsBackend::GetUniformLocation(GraphicsBackendProgram program, const char *uniformName)
{
    GraphicsBackendUniformLocation location{};
    location.UniformLocation = CHECK_GRAPHICS_BACKEND_FUNC(glGetUniformLocation(program.Program, uniformName))
    return location;
}

void GraphicsBackend::UseProgram(GraphicsBackendProgram program)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glUseProgram(program.Program))
}

void GraphicsBackend::SetUniform(GraphicsBackendUniformLocation location, UniformDataType dataType, int count, const void *data, bool transpose)
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

void GraphicsBackend::GetActiveUniformBlockParameter(GraphicsBackendProgram program, int uniformBlockIndex, UniformBlockParameter parameter, int *values)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glGetActiveUniformBlockiv(program.Program, uniformBlockIndex, Cast(parameter), values))
}

void GraphicsBackend::Clear(ClearMask mask)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glClear(Cast(mask)))
}

void GraphicsBackend::SetClearColor(float r, float g, float b, float a)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glClearColor(r, g, b, a))
}

void GraphicsBackend::SetClearDepth(double depth)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glClearDepth(depth))
}

void GraphicsBackend::DrawArrays(PrimitiveType primitiveType, int firstIndex, int count)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawArrays(Cast(primitiveType), firstIndex, count))
}

void GraphicsBackend::DrawArraysInstanced(PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawArraysInstanced(Cast(primitiveType), firstIndex, indicesCount, instanceCount))
}

void GraphicsBackend::DrawElements(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawElements(Cast(primitiveType), elementsCount, Cast(dataType), indices))
}

void GraphicsBackend::DrawElementsInstanced(PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, const void *indices, int instanceCount)
{
    CHECK_GRAPHICS_BACKEND_FUNC(glDrawElementsInstanced(Cast(primitiveType), elementsCount, Cast(dataType), indices, instanceCount))
}

void GraphicsBackend::GetProgramInterfaceParameter(GraphicsBackendProgram program, ProgramInterface interface, ProgramInterfaceParameter parameter, int *outValues)
{
#ifdef GL_ARB_program_interface_query
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramInterfaceiv(program.Program, Cast(interface), Cast(parameter), outValues))
#endif
}

void GraphicsBackend::GetProgramResourceParameters(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int parametersCount, ProgramResourceParameter *parameters, int bufferSize, int *lengths, int *outValues)
{
#ifdef GL_ARB_program_interface_query
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramResourceiv(program.Program, Cast(interface), resourceIndex, parametersCount, Cast(parameters), bufferSize, lengths, outValues))
#endif
}

void GraphicsBackend::GetProgramResourceName(GraphicsBackendProgram program, ProgramInterface interface, int resourceIndex, int bufferSize, int *outLength, char *outName)
{
#ifdef GL_ARB_program_interface_query
    CHECK_GRAPHICS_BACKEND_FUNC(glGetProgramResourceName(program.Program, Cast(interface), resourceIndex, bufferSize, outLength, outName))
#endif
}

bool GraphicsBackend::SupportShaderStorageBuffer()
{
    static bool result = extensions.contains("GL_ARB_shader_storage_buffer_object") &&
                         extensions.contains("GL_ARB_program_interface_query"); // required to query info about SSBO in shaders
    return result;
}

void GraphicsBackend::SetShaderStorageBlockBinding(GraphicsBackendProgram program, int blockIndex, int blockBinding)
{
#ifdef GL_ARB_shader_storage_buffer_object
    CHECK_GRAPHICS_BACKEND_FUNC(glShaderStorageBlockBinding(program.Program, blockIndex, blockBinding))
#endif
}

GRAPHICS_BACKEND_TYPE_ENUM GraphicsBackend::GetError()
{
    return glGetError();
}

const char *GraphicsBackend::GetErrorString(GRAPHICS_BACKEND_TYPE_ENUM error)
{
    return reinterpret_cast<const char *>(gluGetString(error));
}