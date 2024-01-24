#ifndef OPENGL_STUDY_UNIFORM_INFO_H
#define OPENGL_STUDY_UNIFORM_INFO_H

#include "enums/uniform_data_type.h"
#include "types/graphics_backend_uniform_location.h"

struct UniformInfo
{
    UniformDataType Type;
    GraphicsBackendUniformLocation Location;
    int Index;
};

#endif //OPENGL_STUDY_UNIFORM_INFO_H
