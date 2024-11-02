#ifndef RENDER_ENGINE_LINES_H
#define RENDER_ENGINE_LINES_H

#include "drawable_geometry/drawable_geometry.h"
#include "bounds/bounds.h"

#include <vector>

struct Vector3;

class Lines : public DrawableGeometry
{
public:
    Lines(const std::vector<Vector3> &points, const std::vector<int> &indices);
    ~Lines() override = default;

    inline Bounds GetBounds() const
    {
        return m_Bounds;
    }

    Lines(const Lines &) = delete;
    Lines(Lines &&)      = delete;

    Lines &operator=(const Lines &) = delete;
    Lines &operator=(Lines &&) = delete;

private:
    Bounds m_Bounds;
};


#endif //RENDER_ENGINE_LINES_H
