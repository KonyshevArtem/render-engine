#ifndef OPENGL_STUDY_FBX_ASSET_H
#define OPENGL_STUDY_FBX_ASSET_H

#include "../../external/OpenFBX/ofbx.h"
#include "filesystem"
#include "memory"
#include "vector"

class Mesh;

using namespace std;

class FBXAsset
{
public:
    static shared_ptr<FBXAsset> Load(const filesystem::path &_path);

    virtual ~FBXAsset();

    shared_ptr<Mesh> GetMesh(unsigned int _index) const;

private:
    explicit FBXAsset(ofbx::IScene *_scene);
    FBXAsset(const FBXAsset &) = delete;

    ofbx::IScene *           m_Scene;
    vector<shared_ptr<Mesh>> m_Meshes;
};

#endif //OPENGL_STUDY_FBX_ASSET_H
