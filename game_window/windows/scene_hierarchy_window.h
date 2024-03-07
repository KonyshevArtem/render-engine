#ifndef OPENGL_STUDY_SCENE_HIERARCHY_WINDOW_H
#define OPENGL_STUDY_SCENE_HIERARCHY_WINDOW_H

#include "base_window.h"

class SceneHierarchyWindow : public BaseWindow
{
public:
    SceneHierarchyWindow();
    ~SceneHierarchyWindow() override = default;

protected:
    void DrawInternal() override;
};


#endif //OPENGL_STUDY_SCENE_HIERARCHY_WINDOW_H
