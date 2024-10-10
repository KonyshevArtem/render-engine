#ifndef RENDER_ENGINE_RENDER_SETTINGS_H
#define RENDER_ENGINE_RENDER_SETTINGS_H

#include "draw_call_comparer.h"
#include "draw_call_filter.h"

class Material;

struct RenderSettings
{
    DrawCallSortMode Sorting = DrawCallSortMode::NO_SORTING;
    DrawCallFilter Filter = DrawCallFilter::All();
    std::shared_ptr<Material> OverrideMaterial;
};

#endif