#ifndef RENDER_ENGINE_FBX_ASSET_H
#define RENDER_ENGINE_FBX_ASSET_H

#include "ofbx.h"
#include "resources/resource.h"

#include <memory>
#include <vector>
#include <string>

class Mesh;

class FBXAsset : public Resource
{
public:
    ~FBXAsset() = default;

    std::shared_ptr<Mesh> GetMesh(unsigned int _index) const;

    FBXAsset(const FBXAsset &) = delete;
    FBXAsset(FBXAsset &&)      = delete;

    FBXAsset &operator=(const FBXAsset &) = delete;
    FBXAsset &operator=(FBXAsset &&)      = delete;

private:
    explicit FBXAsset(ofbx::IScene* scene, const std::string& name);

    std::vector<std::shared_ptr<Mesh>> m_Meshes;

    friend class Resources;
};

#endif //RENDER_ENGINE_FBX_ASSET_H
