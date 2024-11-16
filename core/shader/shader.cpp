#include "shader.h"
#include "shader_pass/shader_pass.h"
#include "shader_loader/shader_loader.h"

std::shared_ptr<Shader> Shader::Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords,
    BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo)
{
    auto shader = ShaderLoader::Load(_path, _keywords, blendInfo, cullInfo, depthInfo);

    if (!shader)
    {
        auto fallback = ShaderLoader::Load("core_resources/shaders/fallback/fallback", {}, {}, {}, {});

        if (!fallback)
            exit(1);

        return fallback;
    }

    return shader;
}

Shader::Shader(std::vector<std::shared_ptr<ShaderPass>> _passes, bool _supportInstancing) :
    m_Passes(std::move(_passes)),
    m_SupportInstancing(_supportInstancing)
{
}

std::shared_ptr<ShaderPass> Shader::GetPass(int passIndex) const
{
    if (passIndex < 0 || passIndex >= m_Passes.size())
        throw std::out_of_range("[Shader] Pass Index out of range");

    return m_Passes.at(passIndex);
}