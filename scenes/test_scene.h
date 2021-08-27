#ifndef OPENGL_STUDY_TEST_SCENE_H
#define OPENGL_STUDY_TEST_SCENE_H

#include "../core/scene/scene.h"
#include "../scripts/camera_fly_controller/camera_fly_controller.h"

class TestScene: public Scene
{
public:
    static void Load();
    void        UpdateInternal() override;

private:
    void Init();

    static Vector3    CalcTranslation(float phase);
    static Quaternion CalcRotation(float phase, int i);
    static Vector3    CalcScale(float phase);

    const float                     LoopDuration = 3000;
    shared_ptr<Material>            WaterMaterial;
    unique_ptr<CameraFlyController> CameraFlyControl;
};

#endif //OPENGL_STUDY_TEST_SCENE_H
