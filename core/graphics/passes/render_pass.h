#ifndef OPENGL_STUDY_RENDER_PASS_H
#define OPENGL_STUDY_RENDER_PASS_H

#include "../../renderer/renderer.h"
#include <OpenGL/gl3.h>

class Context;

class RenderPass
{
public:
    RenderPass(Renderer::Sorting _rendererSorting, Renderer::Filter _filter, GLbitfield _clearFlags);

    void Execute(const Context &_ctx);

private:
    RenderPass(const RenderPass &) = delete;

    Renderer::Sorting m_Sorting;
    Renderer::Filter  m_Filter;
    GLbitfield        m_ClearFlags;
};

#endif //OPENGL_STUDY_RENDER_PASS_H
