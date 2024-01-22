#if OPENGL_STUDY_EDITOR

#ifndef OPENGL_STUDY_GIZMOS_H
#define OPENGL_STUDY_GIZMOS_H

#include "matrix4x4/matrix4x4.h"
#include "graphics_backend.h"

#include <memory>
#include <vector>

struct Matrix4x4;

namespace Gizmos
{
    class GizmosPrimitive
    {
    public:
        GizmosPrimitive(GraphicsBackendVAO _vertexArrayObject, GraphicsBackendBuffer _pointsBuffer, GraphicsBackendBuffer _indexBuffer, int _indexCount);
        ~GizmosPrimitive();

        void Draw() const;

    private:
        GraphicsBackendVAO m_VertexArrayObject = 0;
        GraphicsBackendBuffer m_PointsBuffer = 0;
        GraphicsBackendBuffer m_IndexBuffer = 0;
        int m_IndexCount = 0;
    };

    struct GizmosDrawInfo
    {
        std::shared_ptr<GizmosPrimitive> Primitive;
        Matrix4x4                        Matrix;
    };

    void DrawWireCube(const Matrix4x4 &_matrix);

    void                                Init();
    std::vector<Gizmos::GizmosDrawInfo> GetDrawInfos();
    void                                ClearDrawInfos();
} // namespace Gizmos

#endif

#endif