#ifndef OPENGL_STUDY_PROGRAM_INTERFACE_PARAMETER_H
#define OPENGL_STUDY_PROGRAM_INTERFACE_PARAMETER_H

#include "graphics_backend.h"

enum class ProgramInterfaceParameter : GRAPHICS_BACKEND_TYPE_ENUM
{
    ACTIVE_RESOURCES                = GRAPHICS_BACKEND_ACTIVE_RESOURCES,
    MAX_NAME_LENGTH                 = GRAPHICS_BACKEND_MAX_NAME_LENGTH,
    MAX_NUM_ACTIVE_VARIABLES        = GRAPHICS_BACKEND_MAX_NUM_ACTIVE_VARIABLES,
    MAX_NUM_COMPATIBLE_SUBROUTINES  = GRAPHICS_BACKEND_MAX_NUM_COMPATIBLE_SUBROUTINES,
};

#endif //OPENGL_STUDY_PROGRAM_INTERFACE_PARAMETER_H