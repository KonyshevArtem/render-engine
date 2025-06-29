#include "scene.h"
#include "arguments.h"
#include "scene_parser.h"
#include "component/component.h"

void Scene::Update()
{
    if (Current != nullptr)
    {
        for (const std::shared_ptr<GameObject>& go : Current->m_GameObjects)
        {
            for (const std::shared_ptr<Component>& component: go->m_Components)
                component->Update();
        }

        Current->UpdateInternal();
    }
    else
    {
        Load("core_resources/scenes/pbr_demo.scene");
    }
}
void Scene::Load(const std::string& scenePath)
{
    Current = SceneParser::Parse(scenePath);
}
