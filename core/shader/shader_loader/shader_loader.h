#ifndef OPENGL_STUDY_SHADER_LOADER_H
#define OPENGL_STUDY_SHADER_LOADER_H

#include "matrix4x4/matrix4x4.h"
#include <filesystem>
#include <memory>
#include <string>

class Shader;

namespace ShaderLoader
{
    std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords);
}; // namespace ShaderLoader

#endif //OPENGL_STUDY_SHADER_LOADER_H
