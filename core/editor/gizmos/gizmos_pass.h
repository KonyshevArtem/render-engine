#if OPENGL_STUDY_EDITOR

#ifndef OPENGL_STUDY_GIZMOZ_PASS_H
#define OPENGL_STUDY_GIZMOZ_PASS_H

#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

#include <memory>

struct Context;
class Texture2D;

class GizmosPass
{
public:
    GizmosPass();
    ~GizmosPass();

    void Execute(Context &_context);

private:
    GLuint m_Framebuffer;

    GizmosPass(const GizmosPass &) = delete;
    GizmosPass(GizmosPass &&)      = delete;

    GizmosPass &operator=(const GizmosPass &) = delete;
    GizmosPass &operator=(GizmosPass &&) = delete;

    void Outline() const;
    void Gizmos() const;
    void CheckTexture(std::shared_ptr<Texture2D> &_texture) const;
};

#endif

#endif