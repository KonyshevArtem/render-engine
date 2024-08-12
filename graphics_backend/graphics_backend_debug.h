#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H

#include <string>

#include "graphics_backend.h"

namespace GraphicsBackendDebug
{
    struct DebugGroup
    {
        explicit DebugGroup(const std::string &_name);
        ~DebugGroup();
    };

    void CheckError();
    void LogError(GRAPHICS_BACKEND_TYPE_ENUM error, const std::string& line, const std::string &file, int lineNumber);
}

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H
