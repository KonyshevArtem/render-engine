#ifndef RENDER_ENGINE_DRAW_RENDERERS_PASS_H
#define RENDER_ENGINE_DRAW_RENDERERS_PASS_H

#include "render_pass.h"
#include "graphics/render_settings/draw_call_comparer.h"
#include "graphics/render_settings/draw_call_filter.h"
#include "graphics/render_settings/render_settings.h"
#include "graphics/render_queue/render_queue.h"

#include <string>
#include <vector>
#include <memory>

struct Context;
struct Vector3;
class Renderer;

class DrawRenderersPass : public RenderPass
{
public:
    DrawRenderersPass(std::string name, DrawCallSortMode sorting, DrawCallFilter filter, int priority);
    ~DrawRenderersPass() override = default;

    void Prepare(const Vector3& cameraPosition, const std::vector<std::shared_ptr<Renderer>>& renderers);
    void Execute(const Context& ctx) override;

    DrawRenderersPass(const DrawRenderersPass&) = delete;
    DrawRenderersPass(DrawRenderersPass&&) = delete;

    DrawRenderersPass &operator=(const DrawRenderersPass&) = delete;
    DrawRenderersPass &operator=(DrawRenderersPass&&) = delete;

private:
    std::string m_Name;
    RenderSettings m_RenderSettings;
    RenderQueue m_RenderQueue;
};

#endif //RENDER_ENGINE_DRAW_RENDERERS_PASS_H
