#include "scene.h"

void Scene::Update()
{
    if (Current != nullptr)
        Current->UpdateInternal();
}

std::vector<std::shared_ptr<GameObject>>::const_iterator Scene::cbegin()
{
    return m_GameObjects.cbegin();
}

std::vector<std::shared_ptr<GameObject>>::const_iterator Scene::cend()
{
    return m_GameObjects.cend();
}