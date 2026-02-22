#ifndef RENDER_ENGINE_SHADER_H
#define RENDER_ENGINE_SHADER_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_stencil_descriptor.h"
#include "types/graphics_backend_depth_descriptor.h"
#include "types/graphics_backend_rasterizer_descriptor.h"
#include "enums/texture_internal_format.h"
#include "enums/primitive_type.h"
#include "shader/shader_structs.h"
#include "drawable_geometry/vertex_attributes/vertex_attributes.h"

struct GraphicsBackendTextureInfo;
struct GraphicsBackendSamplerInfo;
class GraphicsBackendBufferInfo;
class DrawableGeometry;

class Shader
{
public:
    static std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::vector<std::string> &_keywords,
        BlendInfo blendInfo);

    Shader(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo,
           std::unordered_map<std::string, GraphicsBackendTextureInfo> textures,
           std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers,
           std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers,
           std::string name, bool _supportInstancing);
    ~Shader();

    Shader(const Shader &) = delete;
    Shader(Shader &&)      = delete;

    Shader &operator=(const Shader &) = delete;
    Shader &operator=(Shader &&) = delete;

    const GraphicsBackendProgram& GetProgram(const std::shared_ptr<DrawableGeometry>& geometry);
    const GraphicsBackendProgram& GetProgram(const VertexAttributes& vertexAttributes, PrimitiveType primitiveType);

    inline const std::unordered_map<std::string, GraphicsBackendTextureInfo> &GetTextures() const
    {
        return m_Textures;
    }

    inline const std::unordered_map<std::string, GraphicsBackendSamplerInfo> &GetSamplers() const
    {
        return m_Samplers;
    }

    inline const std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> &GetBuffers() const
    {
        return m_Buffers;
    }

    inline bool SupportInstancing() const
    {
        return m_SupportInstancing;
    }

private:
    std::vector<GraphicsBackendShaderObject> m_Shaders;
    std::unordered_map<size_t, GraphicsBackendProgram> m_Programs;

    BlendInfo m_BlendInfo;
    std::string m_Name;
    bool m_SupportInstancing;

    std::unordered_map<std::string, GraphicsBackendTextureInfo> m_Textures;
    std::unordered_map<std::string, GraphicsBackendSamplerInfo> m_Samplers;
    std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> m_Buffers;

	const GraphicsBackendProgram& CreatePSO(std::vector<GraphicsBackendShaderObject>& shaders, BlendInfo blendInfo, TextureInternalFormat colorFormat, bool isLinear,
		TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor>& vertexAttributes, PrimitiveType primitiveType,
		const GraphicsBackendStencilDescriptor& stencilDescriptor,
        const GraphicsBackendDepthDescriptor& depthDescriptor,
        const GraphicsBackendRasterizerDescriptor& rasterizerDescriptor,
		const std::string& name);
};

#endif //RENDER_ENGINE_SHADER_H