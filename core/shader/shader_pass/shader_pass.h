#ifndef OPENGL_STUDY_SHADER_PASS_H
#define OPENGL_STUDY_SHADER_PASS_H

#include "types/graphics_backend_program.h"
#include "shader/shader_structs.h"
#include "property_block/property_block.h"

#include <string>
#include <unordered_map>

struct UniformInfo;
class BufferInfo;

class ShaderPass
{
public:
    ShaderPass(GraphicsBackendProgram program, BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo,
               std::unordered_map<std::string, std::string> &tags, const std::unordered_map<std::string, std::string> &defaultValues);

    ~ShaderPass();

    ShaderPass(const ShaderPass &) = delete;
    ShaderPass(ShaderPass &&) = delete;

    ShaderPass &operator=(const ShaderPass &) = delete;
    ShaderPass &operator=(ShaderPass &&) = delete;

    inline GraphicsBackendProgram GetProgram() const
    {
        return m_Program;
    }

    inline const BlendInfo &GetBlendInfo() const
    {
        return m_BlendInfo;
    }

    inline const CullInfo &GetCullInfo() const
    {
        return m_CullInfo;
    }

    inline const DepthInfo &GetDepthInfo() const
    {
        return m_DepthInfo;
    }

    inline const std::unordered_map<std::string, UniformInfo> &GetUniforms() const
    {
        return m_Uniforms;
    }

    inline const std::unordered_map<std::string, std::shared_ptr<BufferInfo>> &GetBuffers() const
    {
        return m_Buffers;
    }

    inline const PropertyBlock &GetDefaultValuesBlock() const
    {
        return m_DefaultValuesBlock;
    }

    std::string GetTagValue(const std::string &tag) const;

private:
    GraphicsBackendProgram m_Program;
    PropertyBlock m_DefaultValuesBlock;

    BlendInfo m_BlendInfo;
    CullInfo m_CullInfo;
    DepthInfo m_DepthInfo;

    std::unordered_map<std::string, std::string> m_Tags;
    std::unordered_map<std::string, UniformInfo> m_Uniforms;
    std::unordered_map<std::string, std::shared_ptr<BufferInfo>> m_Buffers;
};


#endif //OPENGL_STUDY_SHADER_PASS_H
