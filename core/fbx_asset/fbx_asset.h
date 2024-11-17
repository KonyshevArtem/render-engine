#ifndef RENDER_ENGINE_FBX_ASSET_H
#define RENDER_ENGINE_FBX_ASSET_H

#include "ofbx.h"
#include <filesystem>
#include <memory>
#include <vector>
#include <string>

class Mesh;

class FBXAsset
{
public:
    static std::shared_ptr<FBXAsset> Load(const std::filesystem::path &_path);

    ~FBXAsset() = default;

    std::shared_ptr<Mesh> GetMesh(unsigned int _index) const;

    FBXAsset(const FBXAsset &) = delete;
    FBXAsset(FBXAsset &&)      = delete;

    FBXAsset &operator=(const FBXAsset &) = delete;
    FBXAsset &operator=(FBXAsset &&)      = delete;

private:
    explicit FBXAsset(ofbx::IScene* scene, const std::string& name);

    std::vector<std::shared_ptr<Mesh>> m_Meshes;
};

#endif //RENDER_ENGINE_FBX_ASSET_H
