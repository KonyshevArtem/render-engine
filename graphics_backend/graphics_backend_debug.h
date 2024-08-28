#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H

#include <string>

namespace GraphicsBackendDebug
{
    struct DebugGroup
    {
        explicit DebugGroup(const std::string &_name);
        ~DebugGroup();
    };
}

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_DEBUG_H
