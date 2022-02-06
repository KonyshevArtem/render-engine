#ifndef OPENGL_STUDY_GIZMOZ_PASS_H
#define OPENGL_STUDY_GIZMOZ_PASS_H

#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

struct Context;

class GizmosPass
{
public:
    GizmosPass()  = default;
    ~GizmosPass() = default;

    void Execute(Context &_context);

private:
    GizmosPass(const GizmosPass &) = delete;
    GizmosPass(GizmosPass &&)      = delete;

    GizmosPass &operator=(const GizmosPass &) = delete;
    GizmosPass &operator=(GizmosPass &&) = delete;
};

#endif