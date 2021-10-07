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

    ~FBXAsset();

    vector<shared_ptr<Mesh>> Meshes;

private:
    explicit FBXAsset(ofbx::IScene *_scene);

    ofbx::IScene *m_Scene;
};

#endif //OPENGL_STUDY_FBX_ASSET_H
