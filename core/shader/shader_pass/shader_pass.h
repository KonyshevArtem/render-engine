#ifndef RENDER_ENGINE_SHADER_PASS_H
#define RENDER_ENGINE_SHADER_PASS_H

#include "types/graphics_backend_program.h"
#include "types/graphics_backend_shader_object.h"
#include "types/graphics_backend_depth_stencil_state.h"
#include "enums/texture_internal_format.h"
#include "shader/shader_structs.h"
#include "property_block/property_block.h"
#include "drawable_geometry/vertex_attributes/vertex_attributes.h"

#include <string>
#include <unordered_map>

struct GraphicsBackendTextureInfo;
struct GraphicsBackendSamplerInfo;
class GraphicsBackendBufferInfo;

class ShaderPass
{
public:
    ShaderPass(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
               const std::unordered_map<std::string, std::string> &defaultValues);

    ShaderPass(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
               const std::unordered_map<std::string, std::string> &defaultValues,
               std::unordered_map<std::string, GraphicsBackendTextureInfo> textures,
               std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> buffers,
               std::unordered_map<std::string, GraphicsBackendSamplerInfo> samplers);

    ~ShaderPass();

    ShaderPass(const ShaderPass &) = delete;
    ShaderPass(ShaderPass &&) = delete;

    ShaderPass &operator=(const ShaderPass &) = delete;
    ShaderPass &operator=(ShaderPass &&) = delete;

    const GraphicsBackendProgram &GetProgram(const VertexAttributes &vertexAttributes, TextureInternalFormat colorTargetFormat, TextureInternalFormat depthTargetFormat);

    inline const CullInfo &GetCullInfo() const
    {
        return m_CullInfo;
    }

    inline const GraphicsBackendDepthStencilState &GetDepthStencilState() const
    {
        return m_DepthStencilState;
    }

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

    inline const std::unordered_map<std::string, std::shared_ptr<Texture>> &GetDefaultTextures() const
    {
        return m_DefaultTextures;
    }

private:
    std::vector<GraphicsBackendShaderObject> m_Shaders;
    std::unordered_map<size_t, GraphicsBackendProgram> m_Programs;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_DefaultTextures;

    CullInfo m_CullInfo;
    GraphicsBackendDepthStencilState m_DepthStencilState;
    BlendInfo m_BlendInfo;

    std::unordered_map<std::string, GraphicsBackendTextureInfo> m_Textures;
    std::unordered_map<std::string, GraphicsBackendSamplerInfo> m_Samplers;
    std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> m_Buffers;

    const GraphicsBackendProgram &CreatePSO(std::vector<GraphicsBackendShaderObject> &shaders, BlendInfo blendInfo, TextureInternalFormat colorFormat,
                                            TextureInternalFormat depthFormat, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes,
                                            std::unordered_map<std::string, GraphicsBackendTextureInfo> *uniforms, std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> *buffers);
};


#endif //RENDER_ENGINE_SHADER_PASS_H
