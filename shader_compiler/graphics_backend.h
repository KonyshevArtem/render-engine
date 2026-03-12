#ifndef RENDER_ENGINE_SHADER_COMPILER_GRAPHICS_BACKEND_H
#define RENDER_ENGINE_SHADER_COMPILER_GRAPHICS_BACKEND_H

#include <string>

enum GraphicsBackend : int
{
    GRAPHICS_BACKEND_OPENGL = 0,
    GRAPHICS_BACKEND_GLES = 1,
    GRAPHICS_BACKEND_METAL = 2,
    GRAPHICS_BACKEND_DX12 = 3,

    GRAPHICS_BACKEND_MAX
};

enum ShaderType
{
	VERTEX_SHADER,
    FRAGMENT_SHADER,
    COMPUTE_SHADER,

    COUNT
};

// Used to offset binding index for constant buffers on Metal because they use the same index space as other buffers
// Must match with value in GraphicsBackendMetal
constexpr uint32_t k_MetalConstantBufferBindingOffset = 8;

// Used to offset binding offset for RW buffers on OpenGL because they use the same index space as read-only buffers
// Must match with value in GraphicsBackendOpenGL
constexpr uint32_t k_OpenGLRWBuffersBindingOffset = 16;

std::string GetBackendLiteral(GraphicsBackend backend)
{
    switch (backend)
    {
        case GRAPHICS_BACKEND_OPENGL:
            return "opengl";
        case GRAPHICS_BACKEND_GLES:
            return "gles";
        case GRAPHICS_BACKEND_METAL:
            return "metal";
        case GRAPHICS_BACKEND_DX12:
            return "dx12";
        default:
            return "";
    }
}

std::wstring GetBackendDefine(GraphicsBackend backend)
{
    switch (backend)
    {
        case GRAPHICS_BACKEND_OPENGL:
            return L"OPENGL_SHADER";
        case GRAPHICS_BACKEND_GLES:
            return L"GLES_SHADER";
        case GRAPHICS_BACKEND_METAL:
            return L"METAL_SHADER";
        case GRAPHICS_BACKEND_DX12:
            return L"DX12_SHADER";
        default:
            return L"";
    }
}

bool TryGetBackend(const std::string& backendString, GraphicsBackend& outBackend)
{
    for (int i = 0; i < GRAPHICS_BACKEND_MAX; ++i)
    {
        auto backend = static_cast<GraphicsBackend>(i);
        if (backendString == GetBackendLiteral(backend))
        {
            outBackend = backend;
            return true;
        }
    }

    return false;
}

std::string GetShaderEntryPoint(ShaderType type)
{
	switch (type)
	{
	case VERTEX_SHADER:
        return "vertexMain";
	case FRAGMENT_SHADER:
        return "fragmentMain";
	case COMPUTE_SHADER:
        return "computeMain";
	}
}

std::wstring GetShaderEntryPointWString(ShaderType type)
{
    switch (type)
    {
    case VERTEX_SHADER:
        return L"vertexMain";
    case FRAGMENT_SHADER:
        return L"fragmentMain";
    case COMPUTE_SHADER:
        return L"computeMain";
    }
}

std::wstring GetShaderProfile(ShaderType type)
{
    switch (type)
    {
    case VERTEX_SHADER:
        return L"vs_6_0";
    case FRAGMENT_SHADER:
        return L"ps_6_0";
    case COMPUTE_SHADER:
        return L"cs_6_0";
    }
}

std::string GetShaderOutputFilename(ShaderType type)
{
    switch (type)
    {
    case VERTEX_SHADER:
        return "vs";
    case FRAGMENT_SHADER:
        return "ps";
    case COMPUTE_SHADER:
        return "cs";
    }
}

#endif //RENDER_ENGINE_SHADER_COMPILER_GRAPHICS_BACKEND_H
