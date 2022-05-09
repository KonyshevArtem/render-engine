#ifndef OPENGL_STUDY_RENDER_PASS_H
#define OPENGL_STUDY_RENDER_PASS_H

#include "graphics/draw_call_info.h"
#include "graphics/render_settings.h"
#include <string>
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

struct Context;

class RenderPass
{
public:
    RenderPass(const std::string &_name, DrawCallInfo::Sorting _sorting, DrawCallInfo::Filter _filter, GLbitfield _clearFlags);
    ~RenderPass() = default;

    void Execute(const Context &_ctx);

private:
    RenderPass(const RenderPass &) = delete;
    RenderPass(RenderPass &&)      = delete;

    RenderPass &operator=(const RenderPass &) = delete;
    RenderPass &operator=(RenderPass &&) = delete;

    std::string    m_Name;
    GLbitfield     m_ClearFlags;
    RenderSettings m_RenderSettings;
};

#endif //OPENGL_STUDY_RENDER_PASS_H
