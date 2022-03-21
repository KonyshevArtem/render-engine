#if OPENGL_STUDY_EDITOR

#include "hierarchy.h"
#include "gameObject/gameObject.h"

static std::vector<std::shared_ptr<GameObject>> selectedGameObjects;

const std::vector<std::shared_ptr<GameObject>> &Hierarchy::GetSelectedGameObjects()
{
    return selectedGameObjects;
}

std::vector<std::shared_ptr<Renderer>> Hierarchy::GetSelectedRenderers()
{
    std::vector<std::shared_ptr<Renderer>> renderers;
    for (const auto &go: selectedGameObjects)
    {
        if (!go)
            continue;

        auto &renderer = go->Renderer;
        if (renderer)
            renderers.push_back(renderer);
    }

    return renderers;
}

void Hierarchy::SetSelectedGameObjects(const std::vector<std::shared_ptr<GameObject>> &_gameObjects)
{
    selectedGameObjects = _gameObjects;
}

#endif