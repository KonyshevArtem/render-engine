#ifndef OPENGL_STUDY_PBR_DEMO_H
#define OPENGL_STUDY_PBR_DEMO_H

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