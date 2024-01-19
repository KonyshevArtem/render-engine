#ifndef OPENGL_STUDY_UNIFORM_INFO_H
#define OPENGL_STUDY_UNIFORM_INFO_H

#include "graphics_backend.h"
#include "enums/uniform_data_type.h"

struct UniformInfo
{
    UniformDataType Type;
    GraphicsBackendUniformLocation Location;
    int Index;
};

#endif //OPENGL_STUDY_UNIFORM_INFO_H
