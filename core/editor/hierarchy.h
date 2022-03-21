#if OPENGL_STUDY_EDITOR

#ifndef OPENGL_STUDY_HIERARCHY
#define OPENGL_STUDY_HIERARCHY

#include <memory>
#include <vector>

class GameObject;
class Renderer;

namespace Hierarchy
{
    const std::vector<std::shared_ptr<GameObject>> &GetSelectedGameObjects();
    std::vector<std::shared_ptr<Renderer>>          GetSelectedRenderers();
    void                                            SetSelectedGameObjects(const std::vector<std::shared_ptr<GameObject>> &_gameObjects);
}; // namespace Hierarchy

#endif

#endif