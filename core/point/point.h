#ifndef OPENGL_STUDY_POINT
#define OPENGL_STUDY_POINT

#include "drawable_geometry/drawable_geometry.h"
#include "enums/primitive_type.h"

class Point: public DrawableGeometry
{
public:
    Point();
    ~Point() override = default;

    inline PrimitiveType GetPrimitiveType() const override
    {
        return PrimitiveType::POINTS;
    }

    inline bool HasIndexes() const override
    {
        return false;
    }

    inline int GetElementsCount() const override
    {
        return 1;
    }

    Point(const Point &) = delete;
    Point(Point &&)      = delete;

    Point &operator=(const Point &) = delete;
    Point &operator=(Point &&) = delete;
};

#endif