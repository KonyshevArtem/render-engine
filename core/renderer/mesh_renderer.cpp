#include "mesh_renderer.h"
#include "material/material.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include "resources/resources.h"

std::shared_ptr<MeshRenderer> MeshRenderer::Create(const nlohmann::json& componentData)
{
    std::shared_ptr<MeshRenderer> renderer = std::shared_ptr<MeshRenderer>(new MeshRenderer());

    std::string assetPath;
    componentData.at("Mesh").get_to(assetPath);
    renderer->m_Mesh = Resources::Load<Mesh>(assetPath);

    std::string materialPath;
    componentData.at("Material").get_to(materialPath);
    renderer->m_Material = Resources::Load<Material>(materialPath);

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