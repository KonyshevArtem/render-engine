#ifndef RENDER_QUEUE_H
#define RENDER_QUEUE_H

#include "graphics/draw_call_info.h"

#include <vector>
#include <memory>

class Renderer;
struct RenderSettings;

class RenderQueue
{
public:
    RenderQueue() = default;
    ~RenderQueue() = default;

    void Prepare(const Vector3& cameraPosition, const std::vector<std::shared_ptr<Renderer>>& renderers, const RenderSettings& renderSettings);
    void Clear();

    bool IsEmpty() const;
    const std::vector<DrawCallInfo>& GetDrawCalls() const;

private:
    std::vector<DrawCallInfo> m_DrawCalls;
};

#endif //RENDER_QUEUE_H
