#ifndef RENDER_ENGINE_SHADER_LOADER_H
#define RENDER_ENGINE_SHADER_LOADER_H

#include <filesystem>
#include <memory>
#include <string>

class Shader;

namespace ShaderLoader
{
    std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords);
    std::shared_ptr<Shader> Load2(const std::filesystem::path &_path, const std::initializer_list<std::string> &_keywords);
}; // namespace ShaderLoader

#endif //RENDER_ENGINE_SHADER_LOADER_H
