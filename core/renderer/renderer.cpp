#include "renderer.h"
#include "mesh_renderer.h"
#include "billboard_renderer.h"
#include "gameObject/gameObject.h"
#include "graphics/graphics.h"
#include "material/material.h"
#include "fbx_asset/fbx_asset.h"
#include "texture_2d/texture_2d.h"

// Register components here because they might not be explicitly referenced anywhere
REGISTER_COMPONENT(MeshRenderer)
REGISTER_COMPONENT(BillboardRenderer)

std::shared_ptr<Component> MeshRendererComponentFactory::CreateComponent(const nlohmann::json& componentData)
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

std::shared_ptr<Component> BillboardRendererComponentFactory::CreateComponent(const nlohmann::json &componentData)
{
    std::string texturePath;
    componentData.at("Texture").get_to(texturePath);
    std::shared_ptr<Texture2D> texture = Texture2D::Load(texturePath);

    return std::make_shared<BillboardRenderer>(texture, "BillboardRenderer_" + texturePath);
}

Renderer::Renderer(const std::shared_ptr<Material> &_material) :
    m_Material(_material)
{
}

Matrix4x4 Renderer::GetModelMatrix() const
{
    if (m_GameObject.expired())
        return Matrix4x4::Identity();

    auto go = m_GameObject.lock();
    return go->GetLocalToWorldMatrix();
}

std::shared_ptr<Material> Renderer::GetMaterial() const
{
    return m_Material;
}
