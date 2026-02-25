#ifdef RENDER_BACKEND_OPENGL

#include "graphics_backend_api.h"
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
#include "types/graphics_backend_color_attachment_descriptor.h"
#include "types/graphics_backend_fence.h"
#include "types/graphics_backend_profiler_marker.h"
#include "types/graphics_backend_sampler_info.h"
#include "types/graphics_backend_program_descriptor.h"
#include "helpers/opengl_helpers.h"
#include "debug.h"
#include "arguments.h"
#include "string_encoding_util.h"

#include <stdexcept>
#include <cassert>
#include <array>
#include <chrono>

#if __has_include("windows.h")
#include <windows.h>
#endif

#if __has_include("android/native_window_jni.h")
#include <android/native_window_jni.h>
#endif

#if RENDER_ENGINE_WINDOWS
typedef HWND Window;
typedef HDC DeviceContext;
typedef HGLRC GLContext;
#elif RENDER_ENGINE_ANDROID
typedef ANativeWindow* Window;
typedef EGLDisplay DeviceContext;
typedef EGLContext GLContext;
#endif

namespace OpenGLLocal
{
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

    struct RenderTargetState
    {
        GLuint Target;
        GLbitfield ClearFlags;
        bool IsBackbuffer;
        bool IsEnabled;

        TextureType TextureType;
        int Level;
        int Layer;
    };

    struct GeometryData
    {
        GLuint VAO;
        GraphicsBackendVertexAttributeDescriptor* VertexAttributes;
        uint32_t VertexAttributesCount;
    };

    Window s_Window;
    DeviceContext s_DeviceContext;
    GLContext s_MainThreadContext;
    std::unordered_map<std::thread::id, GLContext> s_ThreadContexts;

    GLuint s_Framebuffers[GraphicsBackend::GetMaxFramesInFlight()][2];
    OpenGLLocal::RenderTargetState s_RenderTargetStates[static_cast<int>(FramebufferAttachment::MAX)];
    uint64_t s_DebugGroupId = 0;
    uint64_t s_TimestampDifference = 0;
    GLsync s_FrameFinishFence[GraphicsBackend::GetMaxFramesInFlight()];

    bool s_StencilEnabled = false;
    GLenum s_StencilFrontFunc = GL_ALWAYS;
    GLenum s_StencilBackFunc = GL_ALWAYS;
    uint8_t s_StencilReadMask = 255;
    uint8_t s_StencilValue = 0;

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

    void ResetRenderTargetStates()
    {
        constexpr int max = static_cast<int>(FramebufferAttachment::MAX);
        for (int i = 0; i < max; ++i)
        {
            const FramebufferAttachment attachment = static_cast<FramebufferAttachment>(i);

            OpenGLLocal::RenderTargetState& state = s_RenderTargetStates[i];
            state.Target = 0;
            state.ClearFlags = 0;
            state.IsBackbuffer = true;
            state.IsEnabled = attachment == FramebufferAttachment::COLOR_ATTACHMENT0 || attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT;
            state.TextureType = TextureType::TEXTURE_2D;
            state.Level = 0;
            state.Layer = 0;
        }
    }

    void BindBuffer(GLenum bindTarget, const GraphicsBackendBuffer& buffer, uint32_t index, int offset, int size)
    {
        const OpenGLLocal::BufferData* bufferData = reinterpret_cast<OpenGLLocal::BufferData*>(buffer.Buffer);

        glBindBuffer(bindTarget, bufferData->GLBuffer);
        glBindBufferRange(bindTarget, index, bufferData->GLBuffer, offset, size);
    }

    void UpdateStencil()
    {
        if (s_StencilEnabled)
        {
            glStencilFuncSeparate(GL_FRONT, s_StencilFrontFunc, s_StencilValue, s_StencilReadMask);
            glStencilFuncSeparate(GL_BACK, s_StencilBackFunc, s_StencilValue, s_StencilReadMask);
        }
    }
}

void GraphicsBackendOpenGL::Init(void* data)
{
    GraphicsBackendBase::Init(data);

    OpenGLLocal::s_Window = reinterpret_cast<Window>(data);

#if RENDER_ENGINE_WINDOWS
    OpenGLLocal::s_DeviceContext = GetDC(OpenGLLocal::s_Window);

    PIXELFORMATDESCRIPTOR pixelFormatDescriptor = {};
    pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
    pixelFormatDescriptor.nVersion = 1;
    pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDescriptor.cColorBits = 32;
    pixelFormatDescriptor.cDepthBits = 24;
    pixelFormatDescriptor.cStencilBits = 8;
    pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
    int pixelFormat = ChoosePixelFormat(OpenGLLocal::s_DeviceContext, &pixelFormatDescriptor);
    SetPixelFormat(OpenGLLocal::s_DeviceContext, pixelFormat, &pixelFormatDescriptor);

    HGLRC tempContext = wglCreateContext(OpenGLLocal::s_DeviceContext);
    wglMakeCurrent(OpenGLLocal::s_DeviceContext, tempContext);
#elif RENDER_ENGINE_ANDROID
    OpenGLLocal::s_DeviceContext = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (OpenGLLocal::s_DeviceContext == EGL_NO_DISPLAY)
        LogContextError("eglGetDisplay");
#endif

    OpenGLHelpers::InitBindings();

#if RENDER_ENGINE_WINDOWS
    int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 2,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
    };

    OpenGLLocal::s_MainThreadContext = wglCreateContextAttribsARB(OpenGLLocal::s_DeviceContext, 0, attribs);
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempContext);
    wglMakeCurrent(OpenGLLocal::s_DeviceContext, OpenGLLocal::s_MainThreadContext);
#elif RENDER_ENGINE_ANDROID
    OpenGLLocal::s_MainThreadContext = eglGetCurrentContext();
    if (OpenGLLocal::s_MainThreadContext == EGL_NO_CONTEXT)
        LogContextError("eglGetCurrentContext");
#endif

    int extensionsCount;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensionsCount);
    for (int i = 0; i < extensionsCount; ++i)
    {
        const unsigned char *ext = glGetStringi(GL_EXTENSIONS, i);
        m_Extensions.insert(std::string(reinterpret_cast<const char *>(ext)));
    }

    glGenFramebuffers(2 * GraphicsBackend::GetMaxFramesInFlight(), &OpenGLLocal::s_Framebuffers[0][0]);

#ifdef GL_ARB_seamless_cube_map
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif
    glEnable(GL_DEPTH_TEST);

#if RENDER_ENGINE_EDITOR
    if (Arguments::Contains("-debuglayer"))
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLLocal::DebugMessageCallback, nullptr);

        for (const OpenGLLocal::DebugMessageType &messageType: OpenGLLocal::s_DebugMessageTypes)
            glDebugMessageControl(GL_DONT_CARE, messageType.Type, GL_DONT_CARE, 0, nullptr, messageType.Enabled ? GL_TRUE : GL_FALSE);
    }
#endif

    OpenGLLocal::ResetRenderTargetStates();

    int64_t glTimestamp;
    glGetInteger64v(GL_TIMESTAMP, &glTimestamp);
    glTimestamp /= 1000;

    const auto cpuTimestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    OpenGLLocal::s_TimestampDifference = cpuTimestamp - glTimestamp;
}

GraphicsBackendName GraphicsBackendOpenGL::GetName()
{
#if RENDER_ENGINE_ANDROID
    return GraphicsBackendName::GLES;
#else
    return GraphicsBackendName::OPENGL;
#endif
}

void GraphicsBackendOpenGL::InitNewFrame()
{
    GraphicsBackendBase::InitNewFrame();

    GLsync currentFence = OpenGLLocal::s_FrameFinishFence[GraphicsBackend::GetInFlightFrameIndex()];
    if (glIsSync(currentFence))
    {
        glClientWaitSync(currentFence, GL_SYNC_FLUSH_COMMANDS_BIT, 160000000);
        glDeleteSync(currentFence);
    }

    CreatePendingContexts();
}

void GraphicsBackendOpenGL::FillImGuiInitData(void* data)
{
    struct ImGuiData
    {
        void* Window;
        int MajorVersion;
        int MinorVersion;
    };

    ImGuiData* imGuiData = static_cast<ImGuiData*>(data);
    imGuiData->Window = OpenGLLocal::s_Window;
    imGuiData->MajorVersion = OPENGL_MAJOR_VERSION;
    imGuiData->MinorVersion = OPENGL_MINOR_VERSION;
}

void GraphicsBackendOpenGL::FillImGuiFrameData(void *data)
{
}

GraphicsBackendTexture GraphicsBackendOpenGL::CreateTexture(int width, int height, int depth, TextureType type, TextureInternalFormat format, int mipLevels, bool isLinear, bool isRenderTarget, const std::string& name)
{
    InitContext();

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

GraphicsBackendSampler GraphicsBackendOpenGL::CreateSampler(TextureWrapMode wrapMode, TextureFilteringMode filteringMode, const float *borderColor, int minLod, ComparisonFunction comparisonFunction, const std::string& name)
{
    InitContext();

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
        glSamplerParameterfv(sampler.Sampler, GL_TEXTURE_BORDER_COLOR, borderColor);

    glSamplerParameteri(sampler.Sampler, GL_TEXTURE_MIN_LOD, minLod);

    if (comparisonFunction != ComparisonFunction::NONE)
    {
        const GLenum function = OpenGLHelpers::ToComparisonFunction(comparisonFunction);
        glSamplerParameteri(sampler.Sampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glSamplerParameteri(sampler.Sampler, GL_TEXTURE_COMPARE_FUNC, function);
    }

    if (!name.empty())
        glObjectLabel(GL_SAMPLER, sampler.Sampler, name.length(), name.c_str());

    return sampler;
}

void GraphicsBackendOpenGL::DeleteTexture_Internal(const GraphicsBackendTexture &texture)
{
    glDeleteTextures(1, reinterpret_cast<const GLuint *>(&texture.Texture));
}

void GraphicsBackendOpenGL::DeleteSampler_Internal(const GraphicsBackendSampler &sampler)
{
    glDeleteSamplers(1, reinterpret_cast<const GLuint *>(&sampler.Sampler));
}

void GraphicsBackendOpenGL::BindTexture_Internal(const GraphicsBackendTexture& texture, uint32_t index)
{
    glActiveTexture(OpenGLHelpers::ToTextureUnit(index));
    glBindTexture(OpenGLHelpers::ToTextureType(texture.Type), texture.Texture);
    glUniform1i(index, index);
}

void GraphicsBackendOpenGL::BindSampler_Internal(const GraphicsBackendSampler& sampler, uint32_t index)
{
    glActiveTexture(OpenGLHelpers::ToTextureUnit(index));
    glBindSampler(index, sampler.Sampler);
}

void GraphicsBackendOpenGL::GenerateMipmaps(const GraphicsBackendTexture &texture)
{
    GLenum textureType = OpenGLHelpers::ToTextureType(texture.Type);
    glBindTexture(textureType, texture.Texture);
    glGenerateMipmap(textureType);
}

void GraphicsBackendOpenGL::UploadImagePixels(const GraphicsBackendTexture &texture, int level, CubemapFace cubemapFace, int width, int height, int depth, int imageSize, const void *pixelsData)
{
    InitContext();

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
            glTexSubImage3D(target, level, 0, 0, 0, width, height, depth + 1, pixelFormat, dataType, pixelsData);
        }
        else
        {
            glTexSubImage2D(target, level, 0, 0, width, height, pixelFormat, dataType, pixelsData);
        }
    }

    if (!IsMainThread())
        glFinish();
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
    const int attachmentIndex = static_cast<int>(descriptor.Attachment);
    OpenGLLocal::RenderTargetState& state = OpenGLLocal::s_RenderTargetStates[attachmentIndex];

    state.IsBackbuffer = descriptor.IsBackbuffer;

    if (descriptor.IsBackbuffer)
    {
        state.Target = 0;
        state.IsEnabled = descriptor.Attachment == FramebufferAttachment::COLOR_ATTACHMENT0 ||
                          descriptor.Attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT ||
                          descriptor.Attachment == FramebufferAttachment::DEPTH_ATTACHMENT ||
                          descriptor.Attachment == FramebufferAttachment::STENCIL_ATTACHMENT;
    }
    else
    {
        state.Target = descriptor.Texture.Texture;
        state.TextureType = descriptor.Texture.Type;
        state.Level = descriptor.Level;
        state.Layer = descriptor.Layer;
        state.IsEnabled = true;
    }

    const bool needClear = descriptor.LoadAction == LoadAction::CLEAR;
    if (descriptor.Attachment == FramebufferAttachment::DEPTH_ATTACHMENT)
    {
        state.ClearFlags = needClear ? GL_DEPTH_BUFFER_BIT : 0;
        OpenGLLocal::s_RenderTargetStates[static_cast<int>(FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT)].IsEnabled = false;

        OpenGLLocal::RenderTargetState& stencilState = OpenGLLocal::s_RenderTargetStates[static_cast<int>(FramebufferAttachment::STENCIL_ATTACHMENT)];
        stencilState.IsEnabled = true;
        stencilState.Target = 0;
    }
    else if (descriptor.Attachment == FramebufferAttachment::STENCIL_ATTACHMENT)
    {
        state.ClearFlags = needClear ? GL_STENCIL_BUFFER_BIT : 0;
        OpenGLLocal::s_RenderTargetStates[static_cast<int>(FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT)].IsEnabled = false;

        OpenGLLocal::RenderTargetState& depthState = OpenGLLocal::s_RenderTargetStates[static_cast<int>(FramebufferAttachment::DEPTH_ATTACHMENT)];
        depthState.IsEnabled = false;
        depthState.Target = 0;
    }
    else if (descriptor.Attachment == FramebufferAttachment::DEPTH_STENCIL_ATTACHMENT)
    {
        state.ClearFlags = needClear ? GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT : 0;
        OpenGLLocal::s_RenderTargetStates[static_cast<int>(FramebufferAttachment::DEPTH_ATTACHMENT)].IsEnabled = false;
        OpenGLLocal::s_RenderTargetStates[static_cast<int>(FramebufferAttachment::STENCIL_ATTACHMENT)].IsEnabled = false;
    }
    else
    {
        state.ClearFlags = needClear ? GL_COLOR_BUFFER_BIT : 0;
    }
}

TextureInternalFormat GraphicsBackendOpenGL::GetRenderTargetFormat(FramebufferAttachment attachment, bool* outIsLinear)
{
    // Not implemented. Currently used only for compiling PSO, but OpenGL does not require render target format
    return TextureInternalFormat::RGBA8;
}

GraphicsBackendBuffer GraphicsBackendOpenGL::CreateBuffer(int size, const std::string& name, bool allowCPUWrites, const void* data)
{
    InitContext();

    const GLbitfield bufferFlags = allowCPUWrites ? GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT : 0;

    GLuint glBuffer;
    glGenBuffers(1, &glBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, glBuffer);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, data, bufferFlags);
    if (!name.empty())
    {
        glObjectLabel(GL_BUFFER, glBuffer, name.length(), name.c_str());
    }

    OpenGLLocal::BufferData* bufferData = new OpenGLLocal::BufferData();
    bufferData->GLBuffer = glBuffer;
    bufferData->Data = allowCPUWrites ? static_cast<uint8_t*>(glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, bufferFlags)) : nullptr;

    GraphicsBackendBuffer buffer{};
    buffer.Buffer = reinterpret_cast<uint64_t>(bufferData);
    buffer.Size = size;
    return buffer;
}

void GraphicsBackendOpenGL::DeleteBuffer_Internal(const GraphicsBackendBuffer &buffer)
{
    const OpenGLLocal::BufferData* bufferData = reinterpret_cast<OpenGLLocal::BufferData*>(buffer.Buffer);
    if (bufferData->Data)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, bufferData->GLBuffer);
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }

    glDeleteBuffers(1, reinterpret_cast<const GLuint *>(&buffer.Buffer));
    delete bufferData;
}

void GraphicsBackendOpenGL::BindBuffer_Internal(const GraphicsBackendBuffer &buffer, uint32_t index, int offset, int size)
{
    OpenGLLocal::BindBuffer(GL_SHADER_STORAGE_BUFFER, buffer, index, offset, size);
}

void GraphicsBackendOpenGL::BindStructuredBuffer_Internal(const GraphicsBackendBuffer &buffer, uint32_t index, int offset, int size, int count)
{
    OpenGLLocal::BindBuffer(GL_SHADER_STORAGE_BUFFER, buffer, index, offset, size);
}

void GraphicsBackendOpenGL::BindConstantBuffer_Internal(const GraphicsBackendBuffer &buffer, uint32_t index, int offset, int size)
{
    OpenGLLocal::BindBuffer(GL_UNIFORM_BUFFER, buffer, index, offset, size);
}

void GraphicsBackendOpenGL::SetBufferData(const GraphicsBackendBuffer& buffer, long offset, long size, const void *data)
{
    const OpenGLLocal::BufferData* bufferData = reinterpret_cast<OpenGLLocal::BufferData*>(buffer.Buffer);
    assert(bufferData->Data);
    memcpy(bufferData->Data + offset, data, size);
}

void GraphicsBackendOpenGL::CopyBufferSubData(const GraphicsBackendBuffer& source, const GraphicsBackendBuffer& destination, int sourceOffset, int destinationOffset, int size)
{
    const OpenGLLocal::BufferData* sourceBufferData = reinterpret_cast<OpenGLLocal::BufferData*>(source.Buffer);
    const OpenGLLocal::BufferData* destinationBufferData = reinterpret_cast<OpenGLLocal::BufferData*>(destination.Buffer);

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
    OpenGLLocal::GeometryData* geometryData = new OpenGLLocal::GeometryData();

    GraphicsBackendGeometry geometry{};
    geometry.VertexBuffer = vertexBuffer;
    geometry.IndexBuffer = indexBuffer;
    geometry.Geometry = reinterpret_cast<uint64_t>(geometryData);

    if (IsMainThread())
    {
        CreateVAO(geometry, vertexAttributes.data(), vertexAttributes.size());

        geometryData->VertexAttributes = nullptr;
        geometryData->VertexAttributesCount = 0;
    }
    else
    {
        geometryData->VAO = 0;
        geometryData->VertexAttributes = new GraphicsBackendVertexAttributeDescriptor[vertexAttributes.size()];
        geometryData->VertexAttributesCount = vertexAttributes.size();
        memcpy(geometryData->VertexAttributes, vertexAttributes.data(), vertexAttributes.size() * sizeof(GraphicsBackendVertexAttributeDescriptor));
    }

    return geometry;
}

void GraphicsBackendOpenGL::DeleteGeometry_Internal(const GraphicsBackendGeometry &geometry)
{
    OpenGLLocal::GeometryData* geometryData = reinterpret_cast<OpenGLLocal::GeometryData*>(geometry.Geometry);
    if (geometryData->VAO)
        glDeleteVertexArrays(1, &geometryData->VAO);

    DeleteBuffer(geometry.VertexBuffer);
    DeleteBuffer(geometry.IndexBuffer);

    delete[] geometryData->VertexAttributes;
    delete geometryData;
}

void GraphicsBackendOpenGL::SetViewport(int x, int y, int width, int height, float nearDepth, float farDepth)
{
    glViewport(x, y, width, height);
    glDepthRangef(nearDepth, farDepth);
}

void GraphicsBackendOpenGL::SetScissorRect(int x, int y, int width, int height)
{
    glScissor(x, y, width, height);
}

GraphicsBackendShaderObject GraphicsBackendOpenGL::CompileShader(ShaderType shaderType, const std::string& source, const std::string& name)
{
    InitContext();

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

GraphicsBackendShaderObject GraphicsBackendOpenGL::CompileShaderBinary(ShaderType shaderType, const std::vector<uint8_t>& shaderBinary, const std::string& name)
{
    return {};
}

GraphicsBackendProgram GraphicsBackendOpenGL::CreateProgram(const GraphicsBackendProgramDescriptor& descriptor)
{
    InitContext();

    const GLuint glProgram = glCreateProgram();

    const std::vector<GraphicsBackendShaderObject>& shaders = *descriptor.Shaders;

    for (auto &shader : shaders)
    {
        bool isShader = glIsShader(shader.ShaderObject);
        if (isShader)
            glAttachShader(glProgram, shader.ShaderObject);

    }

    glLinkProgram(glProgram);

    for (auto &shader : shaders)
    {
        bool isShader = glIsShader(shader.ShaderObject);
        if (isShader)
            glDetachShader(glProgram, shader.ShaderObject);

    }

    if (descriptor.Name && !descriptor.Name->empty())
        glObjectLabel(GL_PROGRAM, glProgram, descriptor.Name->length(), descriptor.Name->c_str());


    int isLinked;
    glGetProgramiv(glProgram, GL_LINK_STATUS, &isLinked);
    if (!isLinked)
    {
        int infoLogLength;
        glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

        std::string logMsg(infoLogLength + 1, ' ');
        glGetProgramInfoLog(glProgram, infoLogLength, nullptr, &logMsg[0]);

        throw std::runtime_error("Link failed with error:\n" + logMsg);
    }

    return GraphicsBackendBase::CreateProgram(glProgram, descriptor);
}

void GraphicsBackendOpenGL::DeleteShader_Internal(GraphicsBackendShaderObject shader)
{
    glDeleteShader(shader.ShaderObject);
}

void GraphicsBackendOpenGL::DeleteProgram_Internal(GraphicsBackendProgram program)
{
    const GLuint glProgram = program.Program;
    glDeleteProgram(glProgram);
}

void GraphicsBackendOpenGL::UseProgram(const GraphicsBackendProgram& program)
{
    const GLuint glProgram = program.Program;

    glUseProgram(glProgram);

    const GraphicsBackendBlendDescriptor& blendDescriptor = GetBlendState();
    if (blendDescriptor.Enabled)
    {
        glEnable(GL_BLEND);
        glBlendFunc(OpenGLHelpers::ToBlendFactor(blendDescriptor.SourceFactor), OpenGLHelpers::ToBlendFactor(blendDescriptor.DestinationFactor));
    }
    else
        glDisable(GL_BLEND);

    const uint8_t colorMask = static_cast<uint8_t>(blendDescriptor.ColorWriteMask);
    glColorMask(
        (colorMask & static_cast<uint8_t>(ColorWriteMask::RED)) != 0 ? GL_TRUE : GL_FALSE,
        (colorMask & static_cast<uint8_t>(ColorWriteMask::GREEN)) != 0 ? GL_TRUE : GL_FALSE,
        (colorMask & static_cast<uint8_t>(ColorWriteMask::BLUE)) != 0 ? GL_TRUE : GL_FALSE,
        (colorMask & static_cast<uint8_t>(ColorWriteMask::ALPHA)) != 0 ? GL_TRUE : GL_FALSE
    );

    const GraphicsBackendRasterizerDescriptor& rasterizerDescriptor = GetRasterizerState();
    if (rasterizerDescriptor.Face != CullFace::NONE)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(OpenGLHelpers::ToCullFace(rasterizerDescriptor.Face));
    }
    else
        glDisable(GL_CULL_FACE);

    const GraphicsBackendStencilDescriptor& stencilDescriptor = GetStencilDescriptor();
    OpenGLLocal::s_StencilEnabled = stencilDescriptor.Enabled;
    if (stencilDescriptor.Enabled)
    {
        glEnable(GL_STENCIL_TEST);
        glStencilOpSeparate(
                GL_FRONT,
                OpenGLHelpers::ToStencilOp(stencilDescriptor.FrontFaceOpDescriptor.FailOp),
                OpenGLHelpers::ToStencilOp(stencilDescriptor.FrontFaceOpDescriptor.DepthFailOp),
                OpenGLHelpers::ToStencilOp(stencilDescriptor.FrontFaceOpDescriptor.PassOp)
            );
        glStencilOpSeparate(
                GL_BACK,
                OpenGLHelpers::ToStencilOp(stencilDescriptor.BackFaceOpDescriptor.FailOp),
                OpenGLHelpers::ToStencilOp(stencilDescriptor.BackFaceOpDescriptor.DepthFailOp),
                OpenGLHelpers::ToStencilOp(stencilDescriptor.BackFaceOpDescriptor.PassOp)
        );
        glStencilMask(stencilDescriptor.WriteMask);

        ComparisonFunction frontFunc = stencilDescriptor.FrontFaceOpDescriptor.ComparisonFunction;
        ComparisonFunction backFunc = stencilDescriptor.BackFaceOpDescriptor.ComparisonFunction;
        OpenGLLocal::s_StencilFrontFunc = frontFunc != ComparisonFunction::NONE ? OpenGLHelpers::ToComparisonFunction(frontFunc) : GL_ALWAYS;
        OpenGLLocal::s_StencilBackFunc = backFunc != ComparisonFunction::NONE ? OpenGLHelpers::ToComparisonFunction(backFunc) : GL_ALWAYS;
        OpenGLLocal::s_StencilReadMask = stencilDescriptor.ReadMask;
    }
    else
        glDisable(GL_STENCIL_TEST);

    glFrontFace(OpenGLHelpers::ToCullFaceOrientation(rasterizerDescriptor.Orientation));

    const GraphicsBackendDepthDescriptor& depthDescriptor = GetDepthState();
    if (depthDescriptor.Enabled)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(OpenGLHelpers::ToComparisonFunction(depthDescriptor.DepthFunction));
        glDepthMask(depthDescriptor.WriteDepth);
    }
    else
        glDisable(GL_DEPTH_TEST);

    BindResources(program);
}

void GraphicsBackendOpenGL::SetClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void GraphicsBackendOpenGL::SetClearDepth(double depth)
{
    glClearDepthf(depth);
}

void GraphicsBackendOpenGL::SetStencilValue(uint8_t value)
{
    OpenGLLocal::s_StencilValue = value;
}

void GraphicsBackendOpenGL::DrawArrays(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int count)
{
    ++m_DrawCallCount;

    OpenGLLocal::UpdateStencil();
    BindGeometry(geometry);
    glDrawArrays(OpenGLHelpers::ToPrimitiveType(primitiveType), firstIndex, count);
}

void GraphicsBackendOpenGL::DrawArraysInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int firstIndex, int indicesCount, int instanceCount)
{
    ++m_DrawCallCount;

    OpenGLLocal::UpdateStencil();
    BindGeometry(geometry);
    glDrawArraysInstanced(OpenGLHelpers::ToPrimitiveType(primitiveType), firstIndex, indicesCount, instanceCount);
}

void GraphicsBackendOpenGL::DrawElements(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType)
{
    ++m_DrawCallCount;

    OpenGLLocal::UpdateStencil();
    BindGeometry(geometry);
    glDrawElements(OpenGLHelpers::ToPrimitiveType(primitiveType), elementsCount, OpenGLHelpers::ToIndicesDataType(dataType), nullptr);
}

void GraphicsBackendOpenGL::DrawElementsInstanced(const GraphicsBackendGeometry &geometry, PrimitiveType primitiveType, int elementsCount, IndicesDataType dataType, int instanceCount)
{
    ++m_DrawCallCount;

    OpenGLLocal::UpdateStencil();
    BindGeometry(geometry);
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

    glBindFramebuffer(GL_READ_FRAMEBUFFER, OpenGLLocal::s_Framebuffers[GraphicsBackend::GetInFlightFrameIndex()][0]);
    AttachTextureToFramebuffer(GL_READ_FRAMEBUFFER, glAttachment, source.Type, source.Texture, 0, 0);

    GLuint destinationFramebuffer = destinationDescriptor.IsBackbuffer ? 0 : OpenGLLocal::s_Framebuffers[GraphicsBackend::GetInFlightFrameIndex()][1];
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destinationFramebuffer);
    if (!destinationDescriptor.IsBackbuffer)
    {
        AttachTextureToFramebuffer(GL_DRAW_FRAMEBUFFER, glAttachment, destinationDescriptor.Texture.Type, destinationDescriptor.Texture.Texture, 0, 0);
    }

    glBlitFramebuffer(sourceX, sourceX, sourceX + width, sourceY + height, destinationX, destinationY, destinationX + width, destinationY + height, mask, GL_NEAREST);
}

void GraphicsBackendOpenGL::PushDebugGroup(const std::string& name, GPUQueue queue)
{
#ifdef GL_KHR_debug
    glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, OpenGLLocal::s_DebugGroupId++, -1, name.c_str());
#endif
}

void GraphicsBackendOpenGL::PopDebugGroup(GPUQueue queue)
{
#ifdef GL_KHR_debug
    assert(OpenGLLocal::s_DebugGroupId > 0);
    glPopDebugGroup();
    --OpenGLLocal::s_DebugGroupId;
#endif
}

GraphicsBackendProfilerMarker GraphicsBackendOpenGL::PushProfilerMarker()
{
    GLuint glQueries[2];
    glGenQueries(2, &glQueries[0]);
    glQueryCounter(glQueries[0], GL_TIMESTAMP);

    GraphicsBackendProfilerMarker marker{};
    marker.Info[k_RenderGPUQueueIndex].StartMarker = glQueries[0];
    marker.Info[k_RenderGPUQueueIndex].EndMarker = glQueries[1];
    return marker;
}

void GraphicsBackendOpenGL::PopProfilerMarker(GraphicsBackendProfilerMarker& marker)
{
    const GLuint glQuery = marker.Info[k_RenderGPUQueueIndex].EndMarker;
    glQueryCounter(glQuery, GL_TIMESTAMP);
}

bool GraphicsBackendOpenGL::ResolveProfilerMarker(const GraphicsBackendProfilerMarker& marker, ProfilerMarkerResolveResults& outResults)
{
    const GLuint glQueryStart = marker.Info[k_RenderGPUQueueIndex].StartMarker;
    const GLuint glQueryEnd = marker.Info[k_RenderGPUQueueIndex].EndMarker;

    GLuint queryStartAvailable;
    GLuint queryEndAvailable;
    glGetQueryObjectuiv(glQueryStart, GL_QUERY_RESULT_AVAILABLE, &queryStartAvailable);
    glGetQueryObjectuiv(glQueryEnd, GL_QUERY_RESULT_AVAILABLE, &queryEndAvailable);

    const bool markerResolved = queryStartAvailable && queryEndAvailable;
    if (markerResolved)
    {
        auto resolveQuery = [](GLuint query, uint64_t& outTimestamp)
        {
            glGetQueryObjectui64v(query, GL_QUERY_RESULT, &outTimestamp);
            glDeleteQueries(1, &query);

            // from nanoseconds to microseconds
            outTimestamp /= 1000;

            // GL_TIMESTAMP is counted from implementation-defined point of time, that might not match with system clock
            outTimestamp += OpenGLLocal::s_TimestampDifference;
        };

        resolveQuery(glQueryStart, outResults[k_RenderGPUQueueIndex].StartTimestamp);
        resolveQuery(glQueryEnd, outResults[k_RenderGPUQueueIndex].EndTimestamp);
    }

    outResults[k_RenderGPUQueueIndex].IsActive = markerResolved;
    outResults[k_CopyGPUQueueIndex].IsActive = false;

    return markerResolved;
}

void GraphicsBackendOpenGL::BeginRenderPass(const std::string& name)
{
    bool isBackbuffer = true;
    GLbitfield clearFlag = 0;

    for (const OpenGLLocal::RenderTargetState& state : OpenGLLocal::s_RenderTargetStates)
    {
        if (!state.IsEnabled)
            continue;

        isBackbuffer &= state.IsBackbuffer || !state.Target;
        clearFlag |= state.ClearFlags;
    }

    PushDebugGroup(name, GPUQueue::RENDER);

    if (isBackbuffer)
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }
    else
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGLLocal::s_Framebuffers[GraphicsBackend::GetInFlightFrameIndex()][0]);

        constexpr int maxAttachments = static_cast<int>(FramebufferAttachment::MAX);
        for (int i = 0; i < maxAttachments; ++i)
        {
            const OpenGLLocal::RenderTargetState& state = OpenGLLocal::s_RenderTargetStates[i];
            if (!state.IsEnabled)
                continue;

            const GLenum glAttachment = OpenGLHelpers::ToFramebufferAttachment(static_cast<FramebufferAttachment>(i));
            AttachTextureToFramebuffer(GL_DRAW_FRAMEBUFFER, glAttachment, state.TextureType, state.Target, state.Level, state.Layer);
        }
    }

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
    GraphicsBackendBase::EndRenderPass();

    OpenGLLocal::ResetRenderTargetStates();
    PopDebugGroup(GPUQueue::RENDER);
}

void GraphicsBackendOpenGL::BeginCopyPass(const std::string& name)
{
    PushDebugGroup(name, GPUQueue::COPY);
}

void GraphicsBackendOpenGL::EndCopyPass()
{
    PopDebugGroup(GPUQueue::COPY);
}

GraphicsBackendFence GraphicsBackendOpenGL::CreateFence(FenceType fenceType, const std::string& name)
{
    return GraphicsBackendFence{};
}

void GraphicsBackendOpenGL::DeleteFence(const GraphicsBackendFence& fence)
{
}

void GraphicsBackendOpenGL::SignalFence(const GraphicsBackendFence& fence)
{
}

void GraphicsBackendOpenGL::WaitForFence(const GraphicsBackendFence& fence)
{
}

void GraphicsBackendOpenGL::Flush()
{
    glFlush();
}

void GraphicsBackendOpenGL::Present()
{
    OpenGLLocal::s_FrameFinishFence[GraphicsBackend::GetInFlightFrameIndex()] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

#if RENDER_ENGINE_WINDOWS
    SwapBuffers(OpenGLLocal::s_DeviceContext);
#endif
}

void GraphicsBackendOpenGL::TransitionRenderTarget(const GraphicsBackendRenderTargetDescriptor &target, ResourceState state, GPUQueue queue)
{
}

void GraphicsBackendOpenGL::TransitionTexture(const GraphicsBackendTexture& texture, ResourceState state, GPUQueue queue)
{
}

void GraphicsBackendOpenGL::TransitionBuffer(const GraphicsBackendBuffer& buffer, ResourceState state, GPUQueue queue)
{
}

bool GraphicsBackendOpenGL::RequireVertexAttributesForPSO() const
{
    return false;
}

bool GraphicsBackendOpenGL::RequirePrimitiveTypeForPSO() const
{
    return false;
}

bool GraphicsBackendOpenGL::RequireRTFormatsForPSO() const
{
    return false;
}

bool GraphicsBackendOpenGL::RequireStencilStateForPSO() const
{
    return false;
}

bool GraphicsBackendOpenGL::RequireDepthStateForPSO() const
{
    return false;
}

bool GraphicsBackendOpenGL::RequireRasterizerStateForPSO() const
{
    return false;
}

bool GraphicsBackendOpenGL::RequireBlendStateForPSO() const
{
    return false;
}

void GraphicsBackendOpenGL::CreateVAO(const GraphicsBackendGeometry& geometry, const GraphicsBackendVertexAttributeDescriptor* vertexAttributes, uint32_t vertexAttributesCount)
{
    OpenGLLocal::GeometryData* geometryData = reinterpret_cast<OpenGLLocal::GeometryData*>(geometry.Geometry);
    const OpenGLLocal::BufferData* vertexBufferData = reinterpret_cast<OpenGLLocal::BufferData*>(geometry.VertexBuffer.Buffer);
    const OpenGLLocal::BufferData* indexBufferData = reinterpret_cast<OpenGLLocal::BufferData*>(geometry.IndexBuffer.Buffer);

    glGenVertexArrays(1, &geometryData->VAO);
    glBindVertexArray(geometryData->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData->GLBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferData->GLBuffer);

    for (uint32_t i = 0; i < vertexAttributesCount; ++i)
    {
        const GraphicsBackendVertexAttributeDescriptor& descriptor = vertexAttributes[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, descriptor.Dimensions, OpenGLHelpers::ToVertexAttributeDataType(descriptor.DataType), descriptor.IsNormalized ? GL_TRUE : GL_FALSE, descriptor.Stride, reinterpret_cast<const void *>(descriptor.Offset));
    }
}

void GraphicsBackendOpenGL::BindGeometry(const GraphicsBackendGeometry& geometry)
{
    OpenGLLocal::GeometryData* geometryData = reinterpret_cast<OpenGLLocal::GeometryData*>(geometry.Geometry);
    if (!geometryData->VAO)
    {
        CreateVAO(geometry, geometryData->VertexAttributes, geometryData->VertexAttributesCount);

        delete[] geometryData->VertexAttributes;
        geometryData->VertexAttributes = nullptr;
    }

    glBindVertexArray(geometryData->VAO);
}

void GraphicsBackendOpenGL::InitContext()
{
    if (IsMainThread())
        return;

    bool contextReady;
    std::thread::id threadId = std::this_thread::get_id();

    {
        std::shared_lock readLock(m_ThreadContextsMutex);
        contextReady = OpenGLLocal::s_ThreadContexts.contains(threadId);
    }

    if (!contextReady)
    {
        {
            std::unique_lock writeLock(m_ThreadContextsMutex);
            m_PendingContextCreation.push_back(threadId);
        }

        while (!contextReady)
        {
            std::unique_lock readLock(m_ThreadContextsMutex);
            contextReady = OpenGLLocal::s_ThreadContexts.contains(threadId);
        }
    }
    else
        return;

    {
        std::unique_lock lock(m_ThreadContextsMutex);

        GLContext context = OpenGLLocal::s_ThreadContexts[threadId];
#if RENDER_ENGINE_WINDOWS
        if (!wglMakeCurrent(OpenGLLocal::s_DeviceContext, context))
            LogContextError("wglMakeCurrent");
#elif RENDER_ENGINE_ANDROID
        if (!eglMakeCurrent(OpenGLLocal::s_DeviceContext, EGL_NO_SURFACE, EGL_NO_SURFACE, context))
            LogContextError("eglMakeCurrent");
#endif
    }
}

void GraphicsBackendOpenGL::CreatePendingContexts()
{
    std::unique_lock lock(m_ThreadContextsMutex);

    for (std::thread::id threadId : m_PendingContextCreation)
    {
        GLContext workerContext;
#if RENDER_ENGINE_WINDOWS
        workerContext = wglCreateContext(OpenGLLocal::s_DeviceContext);
        if (!workerContext)
            LogContextError("wglCreateContext");

        if (!wglShareLists(OpenGLLocal::s_MainThreadContext, workerContext))
            LogContextError("wglShareLists");
#elif RENDER_ENGINE_ANDROID
        EGLint attributes[5] = {EGL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR_VERSION, EGL_CONTEXT_MINOR_VERSION, OPENGL_MINOR_VERSION, EGL_NONE};
        workerContext = eglCreateContext(OpenGLLocal::s_DeviceContext, nullptr, OpenGLLocal::s_MainThreadContext, attributes);
        if (workerContext == EGL_NO_CONTEXT)
            LogContextError("eglCreateContext");
#endif

        OpenGLLocal::s_ThreadContexts[threadId] = workerContext;
    }

    m_PendingContextCreation.clear();
}

void GraphicsBackendOpenGL::LogContextError(const std::string& tag)
{
#if RENDER_ENGINE_WINDOWS
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    DWORD length = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            dw,
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
            (LPTSTR) &lpMsgBuf,
            0,
            nullptr);

    if (length > 0)
    {
        std::string text = StringEncodingUtil::Utf16ToUtf8(reinterpret_cast<const char16_t*>(lpMsgBuf), length);
        Debug::LogErrorFormat("[{}] {}", tag, text);
    }
#elif RENDER_ENGINE_ANDROID
    auto GetErrorString = [](EGLint error)
    {
        switch (error)
        {
            case EGL_SUCCESS:
                return "No error occurred";
            case EGL_NOT_INITIALIZED:
                return "EGL not initialized or failed to initialize";
            case EGL_BAD_ACCESS:
                return "Resource is already in use";
            case EGL_BAD_ALLOC:
                return "Memory allocation failed";
            case EGL_BAD_ATTRIBUTE:
                return "Invalid attribute or value";
            case EGL_BAD_CONTEXT:
                return "Invalid rendering context";
            case EGL_BAD_CONFIG:
                return "Invalid framebuffer configuration";
            case EGL_BAD_DISPLAY:
                return "Invalid display";
            case EGL_BAD_SURFACE:
                return "Invalid surface";
            case EGL_CONTEXT_LOST:
                return "Context lost due to power management";
        }
    };

    EGLint error = eglGetError();
    Debug::LogErrorFormat("[{}] {}", tag, GetErrorString(error));
#endif
}

#endif // RENDER_BACKEND_OPENGL