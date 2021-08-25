#ifndef OPENGL_STUDY_TEST_SCENE_H
#define OPENGL_STUDY_TEST_SCENE_H

#include "../core/scene/scene.h"

class TestScene: public Scene
{
public:
    static void Load();
    void        Update(float time, float deltaTime) override;

private:
    void Init();

    static Vector3    CalcTranslation(float phase);
    static Quaternion CalcRotation(float phase, int i);
    static Vector3    CalcScale(float phase);

    const float          LoopDuration = 3000;
    shared_ptr<Material> WaterMaterial;
};

#endif //OPENGL_STUDY_TEST_SCENE_H
