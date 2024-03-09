#if OPENGL_STUDY_EDITOR

#ifndef OPENGL_STUDY_HIERARCHY
#define OPENGL_STUDY_HIERARCHY

#include <memory>
#include <unordered_set>

class GameObject;

namespace Hierarchy
{
    std::unordered_set<std::shared_ptr<GameObject>> &GetSelectedGameObjects();
};

#endif

#endif