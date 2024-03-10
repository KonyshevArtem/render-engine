#ifndef RENDER_ENGINE_POINT
#define RENDER_ENGINE_POINT

#include "drawable_geometry/drawable_geometry.h"

class Point: public DrawableGeometry
{
public:
    Point();
    ~Point() override = default;

    Point(const Point &) = delete;
    Point(Point &&)      = delete;

    Point &operator=(const Point &) = delete;
    Point &operator=(Point &&) = delete;
};

#endif