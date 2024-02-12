#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#include <filesystem>
#include <string>
#include <vector>

class ShaderPass;

class Shader
{
public:
    static std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords);

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

#endif //OPENGL_STUDY_SHADER_H