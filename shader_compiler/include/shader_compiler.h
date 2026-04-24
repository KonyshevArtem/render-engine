#ifndef RENDER_ENGINE_SHADER_COMPILER_H
#define RENDER_ENGINE_SHADER_COMPILER_H

#include <filesystem>

namespace ShaderCompilerLib
{
	bool CompileShader(const std::filesystem::path& inputPath, const std::filesystem::path& outputPath, const std::string& backendName, const std::vector<std::string>& defines, bool debug);
}

#endif