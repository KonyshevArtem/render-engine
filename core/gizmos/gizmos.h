#ifndef OPENGL_STUDY_GIZMOS_H
#define OPENGL_STUDY_GIZMOS_H

#include "../../math/matrix4x4/matrix4x4.h"
#include <memory>
#include <vector>
#ifdef OPENGL_STUDY_WINDOWS
#include "GL/glew.h"
#include "GL/freeglut.h"
#elif OPENGL_STUDY_MACOS
#include "GLUT/glut.h"
#include "OpenGL/gl3.h"
#endif

struct Matrix4x4;

namespace Gizmos
{
    class GizmosPrimitive
    {
    public:
        GizmosPrimitive(GLuint _vertexArrayObject, GLuint _pointsBuffer, GLuint _indexBuffer, int _indexCount);
        ~GizmosPrimitive();

        void Draw() const;

    private:
        GLuint m_VertexArrayObject = 0;
        GLuint m_PointsBuffer      = 0;
        GLuint m_IndexBuffer       = 0;
        int    m_IndexCount        = 0;
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