#ifndef OPENGL_STUDY_POINT
#define OPENGL_STUDY_POINT

#include "drawable_geometry/drawable_geometry.h"
#include "graphics/render_settings.h"
#include "material/material.h"
#include "vector3/vector3.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif

class Point: public DrawableGeometry
{
public:
    Point();
    virtual ~Point() = default;

    void Draw(const Material &_material, const RenderSettings &_settings) const;
    void SetPosition(const Vector3 &_position);

    inline GLenum GetGeometryType() const override
    {
        return GL_POINTS;
    }

private:
    Point(const Point &) = delete;
    Point(Point &&)      = delete;

    Point &operator=(const Point &) = delete;
    Point &operator=(Point &&) = delete;
};

#endif