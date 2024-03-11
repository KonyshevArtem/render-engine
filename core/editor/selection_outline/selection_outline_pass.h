#if RENDER_ENGINE_EDITOR

#ifndef RENDER_ENGINE_SELECTION_OUTLINE_PASS_H
#define RENDER_ENGINE_SELECTION_OUTLINE_PASS_H

struct Context;

class SelectionOutlinePass
{
public:
    SelectionOutlinePass() = default;
    ~SelectionOutlinePass() = default;

    void Execute(Context &_context);

    SelectionOutlinePass(const SelectionOutlinePass &) = delete;
    SelectionOutlinePass(SelectionOutlinePass &&)      = delete;

    SelectionOutlinePass &operator=(const SelectionOutlinePass &) = delete;
    SelectionOutlinePass &operator=(SelectionOutlinePass &&) = delete;
};


#endif //RENDER_ENGINE_SELECTION_OUTLINE_PASS_H

#endif