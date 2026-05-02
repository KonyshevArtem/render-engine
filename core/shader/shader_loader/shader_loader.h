#ifndef RENDER_ENGINE_SHADER_LOADER_H
#define RENDER_ENGINE_SHADER_LOADER_H

#include <filesystem>
#include <string>
#include <set>

#include "types/graphics_backend_program_descriptor.h"
#include "hash.h"

class Shader;

namespace ShaderLoader
{
    template<std::ranges::range Container>
	size_t GetDefinesHash(const Container& defines)
    {
        std::set<std::string> orderedDefines;
        for (const std::string& define : defines)
            orderedDefines.insert(define);

        std::string combinedDefines;
        for (const std::string& define : orderedDefines)
            combinedDefines += define;

        return Hash::FNV1a(combinedDefines);
    }

	void Load(const std::filesystem::path& path, const std::vector<std::string>& defines,
        std::vector<GraphicsBackendShaderObject>& outShaders,
        std::unordered_map<std::string, GraphicsBackendTextureInfo>& outTextures,
        std::unordered_map<std::string, std::shared_ptr<GraphicsBackendBufferInfo>>& outBuffers,
        std::unordered_map<std::string, GraphicsBackendSamplerInfo>& outSamplers,
        std::unordered_map<std::string, GraphicsBackendTLASInfo>& outTLASes,
        ThreadGroupSize& outThreadGroupSize,
        std::string& outName,
        bool& outSupportInstancing);
}; // namespace ShaderLoader

#endif //RENDER_ENGINE_SHADER_LOADER_H
