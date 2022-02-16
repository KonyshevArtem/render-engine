#include "gizmos.h"
#include "vector3/vector3.h"

std::shared_ptr<Gizmos::GizmosPrimitive> m_WireCubePrimitive;
std::vector<Gizmos::GizmosDrawInfo>      m_DrawInfos;

Gizmos::GizmosPrimitive::GizmosPrimitive(GLuint _vertexArrayObject, GLuint _pointsBuffer, GLuint _indexBuffer, int _indexCount) :
    m_VertexArrayObject(_vertexArrayObject), m_PointsBuffer(_pointsBuffer), m_IndexBuffer(_indexBuffer), m_IndexCount(_indexCount)
{
}

Gizmos::GizmosPrimitive::~GizmosPrimitive()
{
    glDeleteVertexArrays(1, &m_VertexArrayObject);
    glDeleteBuffers(1, &m_PointsBuffer);
    glDeleteBuffers(1, &m_IndexBuffer);
}

void Gizmos::GizmosPrimitive::Draw() const
{
    glBindVertexArray(m_VertexArrayObject);
    glDrawElements(GL_LINES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Gizmos::DrawWireCube(const Matrix4x4 &_matrix)
{
    m_DrawInfos.push_back(Gizmos::GizmosDrawInfo {m_WireCubePrimitive, _matrix});
}

void Gizmos::Init()
{
    constexpr int PRIMITIVES_COUNT = 1;
    GLuint        vertexArrayObjects[PRIMITIVES_COUNT] {0};
    GLuint        pointsBuffers[PRIMITIVES_COUNT] {0};
    GLuint        indexBuffers[PRIMITIVES_COUNT] {0};

    glGenVertexArrays(PRIMITIVES_COUNT, &vertexArrayObjects[0]);
    glGenBuffers(PRIMITIVES_COUNT, &pointsBuffers[0]);
    glGenBuffers(PRIMITIVES_COUNT, &indexBuffers[0]);

    // wire cube
    {
        constexpr int INDEX = 0;
        glBindVertexArray(vertexArrayObjects[INDEX]);
        glBindBuffer(GL_ARRAY_BUFFER, pointsBuffers[INDEX]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[INDEX]);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        constexpr int WIRE_CUBE_POINTS_COUNT = 8;
        constexpr int WIRE_CUBE_INDEX_COUNT  = 24;

        Vector3 wireCubePoints[WIRE_CUBE_POINTS_COUNT] {
                {-1, -1, -1},
                {-1, -1, 1},
                {-1, 1, -1},
                {-1, 1, 1},
                {1, -1, -1},
                {1, -1, 1},
                {1, 1, -1},
                {1, 1, 1}};
        int wireCubeIndexes[WIRE_CUBE_INDEX_COUNT] {0, 1, 0, 2, 1, 3, 2, 3, 4, 5, 4, 6, 5, 7, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7};

        glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * WIRE_CUBE_POINTS_COUNT, &wireCubePoints[0], GL_STATIC_READ);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * WIRE_CUBE_INDEX_COUNT, &wireCubeIndexes[0], GL_STATIC_READ);

        m_WireCubePrimitive = std::make_shared<GizmosPrimitive>(vertexArrayObjects[INDEX], pointsBuffers[INDEX], indexBuffers[INDEX], WIRE_CUBE_INDEX_COUNT);
    }
}

std::vector<Gizmos::GizmosDrawInfo> Gizmos::GetDrawInfos()
{
    return m_DrawInfos;
}

void Gizmos::ClearDrawInfos()
{
    m_DrawInfos.clear();
}
