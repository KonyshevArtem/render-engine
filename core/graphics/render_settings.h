#ifndef OPENGL_STUDY_RENDER_SETTINGS_H
#define OPENGL_STUDY_RENDER_SETTINGS_H

#include "graphics/draw_call_info.h"

#include <string>
#include <unordered_map>

class ShaderPass;

struct RenderSettings
{
    std::unordered_map<std::string, std::string> Tags;
    DrawCallInfo::Sorting                        Sorting = DrawCallInfo::Sorting::NO_SORTING;
    DrawCallInfo::Filter                         Filter  = DrawCallInfo::Filter::All();

    bool TagsMatch(const ShaderPass &_shaderPass) const;
};

#endif