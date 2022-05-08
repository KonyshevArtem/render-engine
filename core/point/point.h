#ifndef OPENGL_STUDY_POINT
#define OPENGL_STUDY_POINT

#include "drawable_geometry/drawable_geometry.h"
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

    inline GLenum GetGeometryType() const override
    {
        return GL_POINTS;
    }

    inline bool HasIndexes() const override
    {
        return false;
    }

    inline GLsizei GetElementsCount() const override
    {
        return 1;
    }

private:
    Point(const Point &) = delete;
    Point(Point &&)      = delete;

    Point &operator=(const Point &) = delete;
    Point &operator=(Point &&) = delete;
};

#endif