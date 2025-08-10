#ifndef RENDER_ENGINE_MATERIAL_PARSER_H
#define RENDER_ENGINE_MATERIAL_PARSER_H

#include "material.h"
#include <filesystem>

namespace MaterialParser
{
    std::shared_ptr<Material> Parse(const std::filesystem::path& path, bool asyncTextureLoads);
}

#endif //RENDER_ENGINE_MATERIAL_PARSER_H
