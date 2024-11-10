#include "fbx_asset.h"
#include "vector2/vector2.h"
#include "vector3/vector3.h"
#include "file_system/file_system.h"
#include "mesh/mesh.h"

std::shared_ptr<FBXAsset> FBXAsset::Load(const std::filesystem::path &_path)
{
    std::vector<uint8_t> content;
    FileSystem::ReadFileBytes(FileSystem::GetResourcesPath() / _path, content);

    auto *scene = ofbx::load(&content[0], content.size(), static_cast<ofbx::u64>(ofbx::LoadFlags::TRIANGULATE)); // NOLINT(cppcoreguidelines-narrowing-conversions)
    if (!scene)
        return nullptr;

    auto asset = std::shared_ptr<FBXAsset>(new FBXAsset(scene));

    scene->destroy();
    return asset;
}

std::shared_ptr<Mesh> FBXAsset::GetMesh(unsigned int _index) const
{
    if (_index >= m_Meshes.size())
        throw std::out_of_range("Index " + std::to_string(_index) + " out of range of m_Meshes.size()");

    return m_Meshes.at(_index);
}

FBXAsset::FBXAsset(ofbx::IScene *_scene)
{
    for (int i = 0; i < _scene->getMeshCount(); ++i)
    {
        auto *m = _scene->getMesh(i);
        if (m == nullptr)
            continue;

        auto *geom = m->getGeometry();
        if (geom == nullptr)
            continue;

        auto vertices = std::vector<Vector3>();
        auto normals  = std::vector<Vector3>();
        auto tangents = std::vector<Vector3>();
        auto uvs      = std::vector<Vector2>();
        auto indexes  = std::vector<int>();

        bool hasNormals  = geom->getNormals() != nullptr;
        bool hasUVs      = geom->getUVs() != nullptr;
        bool hasTangents = geom->getTangents() != nullptr;

        for (int j = 0; j < geom->getVertexCount(); ++j)
        {
            auto v = geom->getVertices()[j];
            vertices.emplace_back(v.x, v.y, v.z);

            if (hasNormals)
            {
                auto n = geom->getNormals()[j];
                normals.emplace_back(n.x, n.y, n.z);
            }
            if (hasUVs)
            {
                auto uv = geom->getUVs()[j];
                uvs.emplace_back(uv.x, uv.y);
            }
            if (hasTangents)
            {
                auto t = geom->getTangents()[j];
                tangents.emplace_back(t.x, t.y, t.z);
            }
        }

        // index with negative value marks end of polygon and is also decreased by 1 during triangulation
        for (int j = 0; j < geom->getIndexCount(); ++j)
        {
            int index = geom->getFaceIndices()[j];
            indexes.push_back(index < 0 ? -index - 1 : index);
        }

        auto mesh = std::make_shared<Mesh>(vertices, normals, indexes, uvs, tangents);

        m_Meshes.push_back(mesh);
    }
}
