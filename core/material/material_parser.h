#ifndef RENDER_ENGINE_MATERIAL_PARSER_H
#define RENDER_ENGINE_MATERIAL_PARSER_H

#include "material.h"

namespace MaterialParser
{
    std::shared_ptr<Material> Parse(const std::string& path);
}

#endif //RENDER_ENGINE_MATERIAL_PARSER_H
