#if OPENGL_STUDY_EDITOR

#ifndef OPENGL_STUDY_GIZMOS_H
#define OPENGL_STUDY_GIZMOS_H

#include "matrix4x4/matrix4x4.h"
#include "types/graphics_backend_vao.h"
#include "types/graphics_backend_buffer.h"

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
        GraphicsBackendVAO m_VertexArrayObject{};
        GraphicsBackendBuffer m_PointsBuffer{};
        GraphicsBackendBuffer m_IndexBuffer{};
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