#ifndef RENDER_ENGINE_MESH_H
#define RENDER_ENGINE_MESH_H

#include "bounds/bounds.h"
#include "drawable_geometry/drawable_geometry.h"
#include "resources/resource.h"
#include "types/graphics_backend_blas.h"
#include "types/graphics_backend_buffer_view.h"
#include "vector2/vector2.h"
#include "vector3/vector3.h"

#include <memory>
#include <string>

class Material;

class Mesh: public DrawableGeometry, public Resource
{
public:
    Mesh(const std::vector<Vector3>& vertices, const std::vector<int>& indices, const std::string& name);
    Mesh(const std::vector<Vector3>& vertices,
         const std::vector<Vector3>& normals,
         const std::vector<int>& indices,
         const std::vector<Vector2>& uvs,
         const std::vector<Vector3>& tangents,
         const std::string& name);
    Mesh(const std::span<uint8_t>& vertexData, const std::span<int>& indices, bool hasUV, bool hasNormals, bool hasTangents,
         const Vector3& minPoint, const Vector3& maxPoint, const std::string& name);
    ~Mesh() override;

    inline Bounds GetBounds() const
    {
        return m_Bounds;
    }

    int GetVertexCount() const
    {
        return m_VertexCount;
    }

    int GetIndexCount()  const
    {
        return m_IndexCount;
    }

    const GraphicsBackendBufferView& GetVertexBufferView() const
    {
        return m_VertexBufferView;
    }

    const GraphicsBackendBufferView& GetIndexBufferView() const
    {
        return m_IndexBufferView;
    }

    const std::string& GetName() const
    {
        return m_Name;
    }

    static const std::shared_ptr<Mesh>& GetFullscreenMesh();
    static const std::shared_ptr<Mesh>& GetQuadMesh();

    Mesh(const Mesh &) = delete;
    Mesh(Mesh &&)      = delete;

    Mesh &operator=(const Mesh &) = delete;
    Mesh &operator=(Mesh &&) = delete;

    GraphicsBackendBLAS BLAS{};

private:
    Bounds m_Bounds;

	GraphicsBackendBufferView m_VertexBufferView{};
	GraphicsBackendBufferView m_IndexBufferView{};
    int m_VertexCount;
    int m_IndexCount;

    std::string m_Name;

    void CreateGeometry(const uint8_t* vertexData, uint64_t vertexDataSize, const int* indices, uint64_t indicesCount, IndicesDataType indicesDataType, const std::string& name);
};


#endif //RENDER_ENGINE_MESH_H