#ifndef OPENGL_STUDY_RENDER_SETTINGS_H
#define OPENGL_STUDY_RENDER_SETTINGS_H

#include "shader/shader.h"
#include <memory>
#include <string>
#include <unordered_map>

struct RenderSettings
{
    std::unordered_map<std::string, std::string> Tags;

    bool TagsMatch(const Shader &_shader, int _passIndex) const;
};

#endif