#ifndef RENDER_ENGINE_SCENE_HIERARCHY_WINDOW_H
#define RENDER_ENGINE_SCENE_HIERARCHY_WINDOW_H

#include "base_window.h"

#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>

class GameObject;

class SceneHierarchyWindow : public BaseWindow
{
public:
    SceneHierarchyWindow();
    ~SceneHierarchyWindow() override = default;

protected:
    void DrawInternal() override;

private:
    struct HierarchyEntry
    {
        std::weak_ptr<GameObject> GameObject;
        int LocalIndex;
        int Depth;
    };

    bool m_IsRenaming;
    bool m_RenamingNeedsFocus;
    std::shared_ptr<GameObject> m_RenameTarget;

    std::unordered_map<int, bool> m_GameObjectsExpandedMap;
    std::vector<HierarchyEntry> m_LinearHierarchy;

    void BuildLinearHierarchy(std::vector<std::shared_ptr<GameObject>> &gameObjects, int depth);
    bool DrawRenameInput(std::shared_ptr<GameObject> &go);
    void DrawContextMenu(std::shared_ptr<GameObject> &go, std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects);
    void DrawGameObjectsHierarchy(std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects, bool selectAll, bool selectRange);
    void DrawGameObjectsHierarchy(std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects, bool selectAll, bool selectRange, int startIndex, int endIndex);
    void Reset();
};


#endif //RENDER_ENGINE_SCENE_HIERARCHY_WINDOW_H
