#ifndef RENDER_ENGINE_SHADER_LOADER_H
#define RENDER_ENGINE_SHADER_LOADER_H

#include <filesystem>
#include <string>

class Shader;

namespace ShaderLoader
{
	std::string GetDefinesHash(const std::vector<std::string>& defines);
	std::shared_ptr<Shader> Load(const std::filesystem::path& path, const std::vector<std::string>& defines);
}; // namespace ShaderLoader

#endif //RENDER_ENGINE_SHADER_LOADER_H
