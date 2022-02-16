#ifndef OPENGL_STUDY_FBX_ASSET_H
#define OPENGL_STUDY_FBX_ASSET_H

#include "ofbx.h"
#include <filesystem>
#include <memory>
#include <vector>

class Mesh;

class FBXAsset
{
public:
    static std::shared_ptr<FBXAsset> Load(const std::filesystem::path &_path);

    ~FBXAsset();

    std::shared_ptr<Mesh> GetMesh(unsigned int _index) const;

private:
    explicit FBXAsset(ofbx::IScene *_scene);
    FBXAsset(const FBXAsset &) = delete;
    FBXAsset(FBXAsset &&)      = delete;

    FBXAsset &operator=(const FBXAsset &) = delete;
    FBXAsset &operator=(FBXAsset &&)      = delete;

    ofbx::IScene *                     m_Scene;
    std::vector<std::shared_ptr<Mesh>> m_Meshes;
};

#endif //OPENGL_STUDY_FBX_ASSET_H
