#ifndef RENDER_ENGINE_SHADOWS_DEMO_H
#define RENDER_ENGINE_SHADOWS_DEMO_H

#include "scene/scene.h"
#include "../scripts/camera_fly_controller/camera_fly_controller.h"

class ShadowsDemo: public Scene
{
public:
    static void Load();

private:
    void Init();
};

#endif //RENDER_ENGINE_SHADOWS_DEMO_H
