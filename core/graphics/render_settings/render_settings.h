#ifndef OPENGL_STUDY_RENDER_SETTINGS_H
#define OPENGL_STUDY_RENDER_SETTINGS_H

#include "draw_call_comparer.h"
#include "draw_call_filter.h"

#include <string>
#include <unordered_map>

class ShaderPass;

struct RenderSettings
{
    std::unordered_map<std::string, std::string> Tags;
    DrawCallSortMode Sorting = DrawCallSortMode::NO_SORTING;
    DrawCallFilter Filter = DrawCallFilter::All();

    bool TagsMatch(const ShaderPass &_shaderPass) const;
};

#endif