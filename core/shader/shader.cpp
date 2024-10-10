#include "shader.h"
#include "shader_pass/shader_pass.h"
#include "shader_loader/shader_loader.h"


std::shared_ptr<Shader> Shader::Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords)
{
    auto shader = ShaderLoader::Load(_path, _keywords);

    if (!shader)
    {
        auto fallback = ShaderLoader::Load("resources/shaders/fallback/fallback.shader", _keywords);

        if (!fallback)
            exit(1);

        return fallback;
    }

    return shader;
}

std::shared_ptr<Shader> Shader::Load2(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords,
    BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo, std::unordered_map<std::string, std::string> tags)
{
    auto shader = ShaderLoader::Load2(_path, _keywords, blendInfo, cullInfo, depthInfo, tags);

    if (!shader)
    {
        auto fallback = ShaderLoader::Load2("resources/shaders/fallback/fallback", {}, {}, {}, {}, {{"LightMode", "Fallback"}});

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