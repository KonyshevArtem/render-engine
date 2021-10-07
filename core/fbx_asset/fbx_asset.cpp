#include "fbx_asset.h"
#include "../../math/vector2/vector2.h"
#include "../../math/vector3/vector3.h"
#include "../../utils/utils.h"
#include "../mesh/mesh.h"
#include <cstdio>

shared_ptr<FBXAsset> FBXAsset::Load(const filesystem::path &_path)
{
    FILE *fp = fopen((Utils::GetExecutableDirectory() / _path).c_str(), "rb");
    if (!fp)
        return nullptr;

    // determine fbx size
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // prepare buffer and read
    auto *content = new ofbx::u8[file_size];
    fread(content, 1, file_size, fp);

    auto scene = ofbx::load((ofbx::u8 *) content, file_size, (ofbx::u64) ofbx::LoadFlags::TRIANGULATE); // NOLINT(cppcoreguidelines-narrowing-conversions)

    // clean up
    delete[] content;
    fclose(fp);

    if (!scene)
        return nullptr;

    return shared_ptr<FBXAsset>(new FBXAsset(scene));
}

FBXAsset::FBXAsset(ofbx::IScene *_scene)
{
    m_Scene = _scene;

    for (int i = 0; i < m_Scene->getMeshCount(); ++i)
    {
        auto m = m_Scene->getMesh(i);
        if (m == nullptr)
            continue;

        auto geom = m->getGeometry();
        if (geom == nullptr)
            continue;

        auto vertices = vector<Vector3>();
        auto normals  = vector<Vector3>();
        auto uvs      = vector<Vector2>();
        auto indexes  = vector<int>();

        for (int j = 0; j < geom->getVertexCount(); ++j)
        {
            auto v  = geom->getVertices()[j];
            auto n  = geom->getNormals()[j];
            auto uv = geom->getUVs()[j];
            vertices.emplace_back(v.x, v.y, v.z);
            normals.emplace_back(n.x, n.y, n.z);
            uvs.emplace_back(uv.x, uv.y);
        }

        // index with negative value marks end of polygon and is also decreased by 1
        for (int j = 0; j < geom->getIndexCount(); ++j)
        {
            int index = geom->getFaceIndices()[j];
            indexes.push_back(index < 0 ? -index - 1 : index);
        }

        auto mesh = make_shared<Mesh>(vertices, normals, indexes, uvs);
        mesh->Init();

        Meshes.push_back(mesh);
    }
}

FBXAsset::~FBXAsset()
{
    if (m_Scene != nullptr)
        m_Scene->destroy();
}
