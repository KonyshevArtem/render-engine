#ifndef RENDER_ENGINE_SELECTION_OUTLINE_PASS_H
#define RENDER_ENGINE_SELECTION_OUTLINE_PASS_H

#if RENDER_ENGINE_EDITOR

#include "graphics/passes/render_pass.h"

struct Context;

class SelectionOutlinePass : public RenderPass
{
public:
    explicit SelectionOutlinePass(int priority);
    ~SelectionOutlinePass() override = default;

    bool Prepare();
    void Execute(const Context& ctx) override;

    SelectionOutlinePass(const SelectionOutlinePass&) = delete;
    SelectionOutlinePass(SelectionOutlinePass&&) = delete;

    SelectionOutlinePass &operator=(const SelectionOutlinePass&) = delete;
    SelectionOutlinePass &operator=(SelectionOutlinePass&&) = delete;
};


#endif //RENDER_ENGINE_SELECTION_OUTLINE_PASS_H

#endif