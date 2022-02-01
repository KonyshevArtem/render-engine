#include "fbx_asset.h"
#include "../../math/vector2/vector2.h"
#include "../../math/vector3/vector3.h"
#include "../../utils/utils.h"
#include "../mesh/mesh.h"
#include <cstdio>

std::shared_ptr<FBXAsset> FBXAsset::Load(const std::filesystem::path &_path)
{
    auto *fp = fopen((Utils::GetExecutableDirectory() / _path).string().c_str(), "rb");
    if (!fp)
        return nullptr;

    // determine fbx size
    fseek(fp, 0, SEEK_END);
    auto file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // prepare buffer and read
    std::vector<ofbx::u8> content(file_size);
    fread(&content[0], 1, file_size, fp);

    auto *scene = ofbx::load(&content[0], file_size, (ofbx::u64) ofbx::LoadFlags::TRIANGULATE); // NOLINT(cppcoreguidelines-narrowing-conversions)

    // clean up
    fclose(fp);

    if (!scene)
        return nullptr;

    return std::shared_ptr<FBXAsset>(new FBXAsset(scene));
}

std::shared_ptr<Mesh> FBXAsset::GetMesh(unsigned int _index) const
{
    if (_index >= m_Meshes.size())
        throw std::out_of_range("Index " + std::to_string(_index) + " out of range of m_Meshes.size()");

    return m_Meshes.at(_index);
}

FBXAsset::FBXAsset(ofbx::IScene *_scene) :
    m_Scene(_scene)
{
    for (int i = 0; i < m_Scene->getMeshCount(); ++i)
    {
        auto *m = m_Scene->getMesh(i);
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
        mesh->Init();

        m_Meshes.push_back(mesh);
    }
}

FBXAsset::~FBXAsset()
{
    if (m_Scene != nullptr)
    {
        m_Scene->destroy();
        m_Scene = nullptr;
    }
}
