#ifndef OPENGL_STUDY_SHADER_H
#define OPENGL_STUDY_SHADER_H

#include "matrix4x4/matrix4x4.h"
#include "shader_loader/shader_loader.h"
#include "uniform_info/uniform_info.h"
#include "vector4/vector4.h"
#include "enums/texture_unit.h"
#include "shader_structs.h"
#include "types/graphics_backend_program.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>


class Texture;
class PropertyBlock;
class UniformBlock;
class ShaderPass;

class Shader
{
#pragma region construction

public:
    static std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords);

    Shader(std::vector<std::shared_ptr<ShaderPass>> _passes, std::unordered_map<std::string, std::string> _defaultValues, bool _supportInstancing);
    ~Shader() = default;

    Shader(const Shader &) = delete;
    Shader(Shader &&)      = delete;

    Shader &operator=(const Shader &) = delete;
    Shader &operator=(Shader &&) = delete;

#pragma endregion

#pragma region fields

private:
    std::vector<std::shared_ptr<ShaderPass>>     m_Passes;
    std::unordered_map<std::string, std::string> m_DefaultValues;
    bool                                         m_SupportInstancing;

#pragma endregion

#pragma region public methods

public:
    void        Use(int _passIndex) const;
    std::string GetPassTagValue(int _passIndex, const std::string &_tag) const;

    inline int PassesCount() const
    {
        return m_Passes.size();
    }

    inline bool SupportInstancing() const
    {
        return m_SupportInstancing;
    }

    static void SetPropertyBlock(const PropertyBlock &_propertyBlock);
    static void SetUniformBlock(const UniformBlock &uniformBlock);

#pragma endregion

#pragma region service methods

private:
    void        SetDefaultValues(const std::unordered_map<std::string, UniformInfo> &_uniforms) const;
    static void SetUniform(const std::string &_name, const void *_data);
    static void SetTextureUniform(const std::string &_name, const Texture &_texture);

#pragma endregion

    friend class UniformBlock;
};

#endif //OPENGL_STUDY_SHADER_H