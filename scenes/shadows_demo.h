#ifndef OPENGL_STUDY_SHADOWS_DEMO_H
#define OPENGL_STUDY_SHADOWS_DEMO_H

#include "scene/scene.h"
#include "../scripts/camera_fly_controller/camera_fly_controller.h"

class ShadowsDemo: public Scene
{
public:
    static void Load();

private:
    void Init();

    void UpdateInternal() override;

    std::unique_ptr<CameraFlyController> m_CameraFlyControl;
};

#endif //OPENGL_STUDY_SHADOWS_DEMO_H
