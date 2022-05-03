#ifndef OPENGL_STUDY_POINT
#define OPENGL_STUDY_POINT

#include "graphics/render_settings.h"
#include "material/material.h"
#include "vector3/vector3.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

class Point
{
public:
    Point();
    ~Point();

    void Draw(const Material &_material, const RenderSettings &_settings) const;
    void SetPosition(const Vector3 &_position);

private:
    Point(const Point &) = delete;
    Point(Point &&)      = delete;

    Point &operator=(const Point &) = delete;
    Point &operator=(Point &&) = delete;

    GLuint m_VertexArrayObject = 0;
    GLuint m_PointBuffer       = 0;
};

#endif