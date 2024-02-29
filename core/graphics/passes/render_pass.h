#ifndef OPENGL_STUDY_RENDER_PASS_H
#define OPENGL_STUDY_RENDER_PASS_H

#include "graphics/render_settings/draw_call_comparer.h"
#include "graphics/render_settings/draw_call_filter.h"
#include "graphics/render_settings/render_settings.h"
#include "enums/clear_mask.h"

#include <string>

struct Context;

class RenderPass
{
public:
    RenderPass(std::string _name, DrawCallSortMode _sorting, DrawCallFilter _filter, ClearMask _clearMask, const std::string &_lightModeTag);
    ~RenderPass() = default;

    void Execute(const Context &_ctx);

    RenderPass(const RenderPass &) = delete;
    RenderPass(RenderPass &&)      = delete;

    RenderPass &operator=(const RenderPass &) = delete;
    RenderPass &operator=(RenderPass &&) = delete;

private:
    std::string    m_Name;
    ClearMask      m_ClearMask;
    RenderSettings m_RenderSettings;
};

#endif //OPENGL_STUDY_RENDER_PASS_H
