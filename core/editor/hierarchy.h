#if RENDER_ENGINE_EDITOR

#ifndef RENDER_ENGINE_HIERARCHY
#define RENDER_ENGINE_HIERARCHY

#include <memory>
#include <unordered_set>

class GameObject;

namespace Hierarchy
{
    std::unordered_set<std::shared_ptr<GameObject>> &GetSelectedGameObjects();
};

#endif

#endif