#include "scene.h"
#include "arguments.h"
#include "scene_parser.h"

void Scene::Update()
{
    if (Current != nullptr)
    {
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
