#ifndef RENDER_ENGINE_SHADER_H
#define RENDER_ENGINE_SHADER_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "shader_structs.h"

class ShaderPass;

class Shader
{
public:
    static std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords,
        BlendInfo blendInfo, CullInfo cullInfo, DepthInfo depthInfo);

    Shader(std::vector<std::shared_ptr<ShaderPass>> _passes, bool _supportInstancing);
    ~Shader() = default;

    Shader(const Shader &) = delete;
    Shader(Shader &&)      = delete;

    Shader &operator=(const Shader &) = delete;
    Shader &operator=(Shader &&) = delete;

    std::shared_ptr<ShaderPass> GetPass(int passIndex) const;

    inline int PassesCount() const
    {
        return m_Passes.size();
    }

    inline bool SupportInstancing() const
    {
        return m_SupportInstancing;
    }

private:
    std::vector<std::shared_ptr<ShaderPass>> m_Passes;
    bool m_SupportInstancing;
};

#endif //RENDER_ENGINE_SHADER_H