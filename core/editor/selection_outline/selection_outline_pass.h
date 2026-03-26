#ifndef RENDER_ENGINE_SELECTION_OUTLINE_PASS_H
#define RENDER_ENGINE_SELECTION_OUTLINE_PASS_H

#if RENDER_ENGINE_EDITOR

#include "graphics/passes/render_pass.h"
#include "graphics/render_queue/render_queue.h"

struct RenderData;
class Material;

class SelectionOutlinePass : public RenderPass
{
public:
    SelectionOutlinePass();
    ~SelectionOutlinePass() override = default;

    void Prepare(RenderData& renderData) override;
    void Execute(const RenderData& renderData) override;

    SelectionOutlinePass(const SelectionOutlinePass&) = delete;
    SelectionOutlinePass(SelectionOutlinePass&&) = delete;

    SelectionOutlinePass &operator=(const SelectionOutlinePass&) = delete;
    SelectionOutlinePass &operator=(SelectionOutlinePass&&) = delete;

private:
    RenderQueue m_SelectedObjectsQueue;
    std::shared_ptr<Material> m_SilhouetteMaterial;
};


#endif //RENDER_ENGINE_SELECTION_OUTLINE_PASS_H

#endif