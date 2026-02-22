#include "billboard_renderer.h"
#include "material/material.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include "texture_2d/texture_2d.h"
#include "resources/resources.h"

std::shared_ptr<BillboardRenderer> BillboardRenderer::Create(const nlohmann::json& componentData)
{
    std::string texturePath;
    float size;
    componentData.at("Texture").get_to(texturePath);
    componentData.at("Size").get_to(size);

    std::shared_ptr<BillboardRenderer> renderer = std::make_shared<BillboardRenderer>(nullptr, size, "BillboardRenderer_" + texturePath);
    renderer->SetTexture(Resources::Load<Texture2D>(texturePath));
    return renderer;
}

std::shared_ptr<Worker::Task> BillboardRenderer::CreateAsync(const nlohmann::json &componentData, const std::function<void(std::shared_ptr<BillboardRenderer>)> &callback)
{
    std::string texturePath;
    float size;
    componentData.at("Texture").get_to(texturePath);
    componentData.at("Size").get_to(size);

    std::shared_ptr<BillboardRenderer> renderer = std::make_shared<BillboardRenderer>(nullptr, size, "BillboardRenderer_" + texturePath);

    std::shared_ptr<Worker::Task> textureTask = Resources::LoadAsync<Texture2D>(texturePath, [renderer](std::shared_ptr<Texture2D> texture)
                                                                                { renderer->SetTexture(texture); });

    std::shared_ptr<Worker::Task> task = Worker::CreateTask([callback, renderer]{ callback(renderer); });
    task->AddDependency(textureTask);
    task->Schedule();

    return task;
}

std::shared_ptr<Mesh> s_BillboardMesh = nullptr;

BillboardRenderer::BillboardRenderer(const std::shared_ptr<Texture2D>& texture, float size, const std::string& name) :
    Renderer(nullptr)
{
    static std::shared_ptr<Shader> shader = Shader::Load("core_resources/shaders/billboard", {}, {}, {CullFace::NONE});
    static std::vector<Vector3> points(4);
    static std::vector<int> indices{0, 1, 2, 1, 2, 3};

    if (!s_BillboardMesh)
    {
        s_BillboardMesh = std::make_shared<Mesh>(points, indices, "BillboardQuad");
    }

    m_Material = std::make_shared<Material>(shader, name);

    SetTexture(texture);
    SetSize(size);
}

Bounds BillboardRenderer::GetAABB() const
{
    return GetModelMatrix() * m_Bounds;
}

std::shared_ptr<DrawableGeometry> BillboardRenderer::GetGeometry()
{
    return s_BillboardMesh;
}

void BillboardRenderer::SetSize(float _size)
{
    auto position = GetModelMatrix().GetPosition();
    m_Bounds.Min  = position - Vector3 {_size, 0, _size};
    m_Bounds.Max  = position + Vector3 {_size, 2 * _size, _size};

    Vector4 size {_size, _size / m_Aspect, 0, 0};
    m_Material->SetVector("_Size", size);
}

void BillboardRenderer::SetTexture(const std::shared_ptr<Texture2D>& texture)
{
    std::shared_lock lock(m_MaterialMutex);

    const std::shared_ptr<Texture2D>& t = texture ? texture : Texture2D::White();
    m_Material->SetTexture("_Texture", t);
    m_Aspect = static_cast<float>(t->GetWidth()) / t->GetHeight();
}

void BillboardRenderer::SetRenderQueue(int _renderQueue)
{
    if (m_Material)
        m_Material->SetRenderQueue(_renderQueue);
}