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

    static Vector3    CalcTranslation(float _phase);
    static Quaternion CalcRotation(float _phase, int _i);
    static Vector3    CalcScale(float _phase);

    const float                     LOOP_DURATION = 3000;
    shared_ptr<Material>            m_WaterMaterial;
    unique_ptr<CameraFlyController> m_CameraFlyControl;
};

#endif //OPENGL_STUDY_TEST_SCENE_H
