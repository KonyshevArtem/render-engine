#ifndef RENDER_ENGINE_PBR_DEMO_H
#define RENDER_ENGINE_PBR_DEMO_H

#include "scene/scene.h"

class PBRDemo: public Scene
{
public:
    static void Load();
    void        UpdateInternal() override;

private:
    void Init();
};

#endif