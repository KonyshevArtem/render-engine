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

struct GraphicsBackendUniformInfo;
class GraphicsBackendBufferInfo;

class ShaderPass
{
public:
    ShaderPass(std::vector<GraphicsBackendShaderObject> &shaders, const std::vector<GraphicsBackendVertexAttributeDescriptor> &vertexAttributes,
               BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo, TextureInternalFormat colorFormat, TextureInternalFormat depthFormat,
               std::unordered_map<std::string, std::string> &tags, const std::unordered_map<std::string, std::string> &defaultValues);

    ~ShaderPass();

    ShaderPass(const ShaderPass &) = delete;
    ShaderPass(ShaderPass &&) = delete;

    ShaderPass &operator=(const ShaderPass &) = delete;
    ShaderPass &operator=(ShaderPass &&) = delete;

    const GraphicsBackendProgram &GetProgram(const VertexAttributes &vertexAttributes);

    inline const BlendInfo &GetBlendInfo() const
    {
        return m_BlendInfo;
    }

    inline const CullInfo &GetCullInfo() const
    {
        return m_CullInfo;
    }

    inline const GraphicsBackendDepthStencilState &GetDepthStencilState() const
    {
        return m_DepthStencilState;
    }

    inline const std::unordered_map<std::string, GraphicsBackendUniformInfo> &GetUniforms() const
    {
        return m_Uniforms;
    }

    inline const std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> &GetBuffers() const
    {
        return m_Buffers;
    }

    inline const PropertyBlock &GetDefaultValuesBlock() const
    {
        return m_DefaultValuesBlock;
    }

    std::string GetTagValue(const std::string &tag) const;

private:
    std::vector<GraphicsBackendShaderObject> m_Shaders;
    std::unordered_map<size_t, GraphicsBackendProgram> m_Programs;
    PropertyBlock m_DefaultValuesBlock;

    BlendInfo m_BlendInfo;
    CullInfo m_CullInfo;
    GraphicsBackendDepthStencilState m_DepthStencilState;
    TextureInternalFormat m_ColorFormat;
    TextureInternalFormat m_DepthFormat;

    std::unordered_map<std::string, std::string> m_Tags;
    std::unordered_map<std::string, GraphicsBackendUniformInfo> m_Uniforms;
    std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>> m_Buffers;
};


#endif //RENDER_ENGINE_SHADER_PASS_H
