#ifndef RENDER_ENGINE_TEST_SCENE_H
#define RENDER_ENGINE_TEST_SCENE_H

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

    static constexpr float               LOOP_DURATION = 3;
    std::shared_ptr<Material>            m_WaterMaterial;
    std::shared_ptr<Light>               m_SpotLight;
    std::shared_ptr<Light>               m_DirectionalLight;

    std::weak_ptr<GameObject> m_RotatingCube;
    std::weak_ptr<GameObject> m_RotatingCylinder1;
    std::weak_ptr<GameObject> m_RotatingCylinder2;
};

#endif //RENDER_ENGINE_TEST_SCENE_H
