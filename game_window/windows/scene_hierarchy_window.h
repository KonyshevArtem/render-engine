#ifndef OPENGL_STUDY_SCENE_HIERARCHY_WINDOW_H
#define OPENGL_STUDY_SCENE_HIERARCHY_WINDOW_H

#include "base_window.h"

#include <memory>
#include <vector>
#include <unordered_set>

class GameObject;

class SceneHierarchyWindow : public BaseWindow
{
public:
    SceneHierarchyWindow();
    ~SceneHierarchyWindow() override = default;

protected:
    void DrawInternal() override;

private:
    bool m_IsRenaming;
    bool m_RenamingNeedsFocus;
    std::shared_ptr<GameObject> m_RenameTarget;

    bool DrawRenameInput(std::shared_ptr<GameObject> &go);
    void DrawContextMenu(std::shared_ptr<GameObject> &go, std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects);
    void DrawGameObjectsHierarchy(std::vector<std::shared_ptr<GameObject>> &gameObjects, std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects, bool selectAll, bool selectRange);
    void DrawGameObjectsHierarchy(std::vector<std::shared_ptr<GameObject>> &gameObjects, std::unordered_set<std::shared_ptr<GameObject>> &selectedGameObjects, bool selectAll, bool selectRange, int startIndex, int endIndex);
    void Reset();
};


#endif //OPENGL_STUDY_SCENE_HIERARCHY_WINDOW_H
