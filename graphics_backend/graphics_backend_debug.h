#ifndef OPENGL_STUDY_GRAPHICS_BACKEND_DEBUG_H
#define OPENGL_STUDY_GRAPHICS_BACKEND_DEBUG_H

#include <string>

#include "graphics_backend.h"

namespace GraphicsBackendDebug
{
    void CheckError();
    void LogError(GraphicsBackendError error, const std::string &file, int line);
}

#endif //OPENGL_STUDY_GRAPHICS_BACKEND_DEBUG_H
