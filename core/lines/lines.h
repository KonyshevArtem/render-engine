#ifndef OPENGL_STUDY_LINES_H
#define OPENGL_STUDY_LINES_H

#include "drawable_geometry/drawable_geometry.h"
#include "bounds/bounds.h"

#include <vector>

struct Vector3;

class Lines : public DrawableGeometry
{
public:
    Lines(const std::vector<Vector3> &points, const std::vector<int> &indices);
    ~Lines() override;

    inline Bounds GetBounds() const
    {
        return m_Bounds;
    }

    Lines(const Lines &) = delete;
    Lines(Lines &&)      = delete;

    Lines &operator=(const Lines &) = delete;
    Lines &operator=(Lines &&) = delete;

private:
    GraphicsBackendBuffer m_IndexBuffer{};

    Bounds m_Bounds;
};


#endif //OPENGL_STUDY_LINES_H
