#include "mesh_renderer.h"
#include "material/material.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include "fbx_asset/fbx_asset.h"

std::shared_ptr<MeshRenderer> MeshRenderer::Create(const nlohmann::json& componentData)
{
    std::shared_ptr<MeshRenderer> renderer = std::shared_ptr<MeshRenderer>(new MeshRenderer());

    int meshIndex;
    std::string assetPath;
    componentData.at("MeshIndex").get_to(meshIndex);
    componentData.at("FBXAsset").get_to(assetPath);
    renderer->m_Mesh = FBXAsset::Load(assetPath)->GetMesh(meshIndex);

    std::string materialPath;
    componentData.at("Material").get_to(materialPath);
    renderer->m_Material = Material::Load(materialPath);

    if (componentData.contains("CastShadows"))
        componentData.at("CastShadows").get_to(renderer->CastShadows);

    return renderer;
}

MeshRenderer::MeshRenderer(std::shared_ptr<Mesh>              _mesh,
                           const std::shared_ptr<Material>   &_material) :
    Renderer(_material),
    m_Mesh(std::move(_mesh))
{
}

Bounds MeshRenderer::GetAABB() const
{
    return m_Mesh ? GetModelMatrix() * m_Mesh->GetBounds() : Bounds();
}

std::shared_ptr<DrawableGeometry> MeshRenderer::GetGeometry() const
{
    return m_Mesh;
}