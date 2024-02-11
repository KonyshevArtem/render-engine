#ifndef OPENGL_STUDY_PER_INSTANCE_DATA_H
#define OPENGL_STUDY_PER_INSTANCE_DATA_H

#include "matrix4x4/matrix4x4.h"

// Keep in-sync with shaders/common/per_instance_data.cg
struct PerInstanceData
{
    Matrix4x4 ModelMatrix;
    Matrix4x4 ModelNormalMatrix;
};

#endif //OPENGL_STUDY_PER_INSTANCE_DATA_H
