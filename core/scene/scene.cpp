#include "scene.h"

void Scene::Update()
{
    if (Current != nullptr)
        Current->UpdateInternal();
}
