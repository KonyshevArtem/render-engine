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

    inline PrimitiveType GetPrimitiveType() const override
    {
        return PrimitiveType::LINES;
    }

    inline bool HasIndexes() const override
    {
        return true;
    }

    inline int GetElementsCount() const override
    {
        return m_IndicesCount;
    }

    Lines(const Lines &) = delete;
    Lines(Lines &&)      = delete;

    Lines &operator=(const Lines &) = delete;
    Lines &operator=(Lines &&) = delete;

private:
    GraphicsBackendBuffer m_IndexBuffer{};

    Bounds m_Bounds;
    int m_IndicesCount;
};


#endif //OPENGL_STUDY_LINES_H
