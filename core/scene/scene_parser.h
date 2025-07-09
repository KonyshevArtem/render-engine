#ifndef RENDER_ENGINE_SCENE_PARSER_H
#define RENDER_ENGINE_SCENE_PARSER_H

#include "scene.h"

namespace SceneParser
{
    std::shared_ptr<Scene> Parse(const std::filesystem::path& path);
}

#endif //RENDER_ENGINE_SCENE_PARSER_H
