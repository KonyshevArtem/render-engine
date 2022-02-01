#ifndef OPENGL_STUDY_TEST_SCENE_H
#define OPENGL_STUDY_TEST_SCENE_H

#include "../core/scene/scene.h"

struct Vector3;
struct Quaternion;
class Material;
class CameraFlyController;

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

    static constexpr float               LOOP_DURATION = 3000;
    std::shared_ptr<Material>            m_WaterMaterial;
    std::shared_ptr<Light>               m_SpotLight;
    std::unique_ptr<CameraFlyController> m_CameraFlyControl;
};

#endif //OPENGL_STUDY_TEST_SCENE_H
