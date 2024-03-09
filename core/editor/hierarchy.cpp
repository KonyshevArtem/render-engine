#if OPENGL_STUDY_EDITOR

#include "hierarchy.h"
#include "gameObject/gameObject.h"

std::unordered_set<std::shared_ptr<GameObject>> &Hierarchy::GetSelectedGameObjects()
{
    static std::unordered_set<std::shared_ptr<GameObject>> selectedGameObjects;
    return selectedGameObjects;
}

#endif