#ifndef RENDER_ENGINE_SHADER_LOADER_H
#define RENDER_ENGINE_SHADER_LOADER_H

#include <filesystem>
#include <string>
#include <unordered_map>

#include "shader/shader_structs.h"
#include "types/graphics_backend_stencil_descriptor.h"
#include "types/graphics_backend_depth_descriptor.h"

class Shader;

namespace ShaderLoader
{
    std::shared_ptr<Shader> Load(const std::filesystem::path &_path, const std::vector<std::string> &_keywords,
        BlendInfo blendInfo, CullInfo cullInfo, GraphicsBackendDepthDescriptor depthDescriptor, GraphicsBackendStencilDescriptor stencilDescriptor);
}; // namespace ShaderLoader

#endif //RENDER_ENGINE_SHADER_LOADER_H
