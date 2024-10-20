#ifndef RENDER_ENGINE_PROGRAM_RESOURCE_PARAMETER_H
#define RENDER_ENGINE_PROGRAM_RESOURCE_PARAMETER_H

#include "graphics_backend.h"

enum class ProgramResourceParameter : GRAPHICS_BACKEND_TYPE_ENUM
{
    NAME_LENGTH     = GRAPHICS_BACKEND_NAME_LENGTH,
    TYPE            = GRAPHICS_BACKEND_TYPE,
    ARRAY_SIZE      = GRAPHICS_BACKEND_ARRAY_SIZE,
    OFFSET          = GRAPHICS_BACKEND_OFFSET,
    BLOCK_INDEX     = GRAPHICS_BACKEND_BLOCK_INDEX,

    ARRAY_STRIDE    = GRAPHICS_BACKEND_ARRAY_STRIDE,
    MATRIX_STRIDE   = GRAPHICS_BACKEND_MATRIX_STRIDE,

    IS_ROW_MAJOR = GRAPHICS_BACKEND_IS_ROW_MAJOR,
    IS_PER_PATCH = GRAPHICS_BACKEND_IS_PER_PATCH,

    ATOMIC_COUNTER_BUFFER_INDEX     = GRAPHICS_BACKEND_ATOMIC_COUNTER_BUFFER_INDEX,
    TEXTURE_BUFFER                  = GRAPHICS_BACKEND_TEXTURE_BUFFER,
    BUFFER_BINDING                  = GRAPHICS_BACKEND_BUFFER_BINDING,
    BUFFER_DATA_SIZE                = GRAPHICS_BACKEND_BUFFER_DATA_SIZE,
    NUM_ACTIVE_VARIABLES            = GRAPHICS_BACKEND_NUM_ACTIVE_VARIABLES,
    ACTIVE_VARIABLES                = GRAPHICS_BACKEND_ACTIVE_VARIABLES,

    REFERENCED_BY_VERTEX_SHADER             = GRAPHICS_BACKEND_REFERENCED_BY_VERTEX_SHADER,
    REFERENCED_BY_TESS_CONTROL_SHADER       = GRAPHICS_BACKEND_REFERENCED_BY_TESS_CONTROL_SHADER,
    REFERENCED_BY_TESS_EVALUATION_SHADER    = GRAPHICS_BACKEND_REFERENCED_BY_TESS_EVALUATION_SHADER,
    REFERENCED_BY_GEOMETRY_SHADER           = GRAPHICS_BACKEND_REFERENCED_BY_GEOMETRY_SHADER,
    REFERENCED_BY_FRAGMENT_SHADER           = GRAPHICS_BACKEND_REFERENCED_BY_FRAGMENT_SHADER,
    REFERENCED_BY_COMPUTE_SHADER            = GRAPHICS_BACKEND_REFERENCED_BY_COMPUTE_SHADER,

    TOP_LEVEL_ARRAY_SIZE    = GRAPHICS_BACKEND_TOP_LEVEL_ARRAY_SIZE,
    TOP_LEVEL_ARRAY_STRIDE  = GRAPHICS_BACKEND_TOP_LEVEL_ARRAY_STRIDE,

    LOCATION        = GRAPHICS_BACKEND_LOCATION,
    LOCATION_INDEX  = GRAPHICS_BACKEND_LOCATION_INDEX,

    NUM_COMPATIBLE_SUBROUTINES  = GRAPHICS_BACKEND_NUM_COMPATIBLE_SUBROUTINES,
    COMPATIBLE_SUBROUTINES      = GRAPHICS_BACKEND_COMPATIBLE_SUBROUTINES,

    LOCATION_COMPONENT                  = GRAPHICS_BACKEND_LOCATION_COMPONENT,
    TRANSFORM_FEEDBACK_BUFFER_INDEX     = GRAPHICS_BACKEND_TRANSFORM_FEEDBACK_BUFFER_INDEX,
    TRANSFORM_FEEDBACK_BUFFER_STRIDE    = GRAPHICS_BACKEND_TRANSFORM_FEEDBACK_BUFFER_STRIDE,
};

#endif //RENDER_ENGINE_PROGRAM_RESOURCE_PARAMETER_H
