#ifndef RENDER_ENGINE_SHADER_H
#define RENDER_ENGINE_SHADER_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "enums/texture_internal_format.h"
#include "enums/primitive_type.h"
#include "shader/shader_structs.h"
#include "drawable_geometry/vertex_attributes/vertex_attributes.h"

struct GraphicsBackendTextureInfo;
struct GraphicsBackendSamplerInfo;
class GraphicsBackendBufferInfo;

class Shader
{
public:
    static std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords,
        BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo);

    Shader(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
           std::unordered_map<std::string, GraphicsBackendTextureInfo> textures,
           std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers,
           std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers,
           std::string name, bool _supportInstancing);
    ~Shader();

    Shader(const Shader &) = delete;
    Shader(Shader &&)      = delete;

    Shader &operator=(const Shader &) = delete;
    Shader &operator=(Shader &&) = delete;

    const GraphicsBackendProgram &GetProgram(const VertexAttributes &vertexAttributes, TextureInternalFormat colorTargetFormat, bool isLinear, TextureInternalFormat depthTargetFormat, PrimitiveType primitiveType);

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

    CullInfo m_CullInfo;
    BlendInfo m_BlendInfo;
    DepthInfo m_DepthInfo;
    std::string m_Name;
    bool m_SupportInstancing;

    std::unordered_map<std::string, GraphicsBackendTextureInfo> m_Textures;
    std::unordered_map<std::string, GraphicsBackendSamplerInfo> m_Samplers;
    std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> m_Buffers;

    const GraphicsBackendProgram &CreatePSO(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, TextureInternalFormat colorFormat, bool isLinear,
                                            TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes, PrimitiveType primitiveType, const std::string& name);
};

#endif //RENDER_ENGINE_SHADER_H