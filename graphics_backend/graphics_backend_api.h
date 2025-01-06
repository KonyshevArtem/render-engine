#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_API_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_API_H

#include "graphics_backend_implementations/graphics_backend_api_base.h"

class GraphicsBackend
{
public:
    static void Init(void *data, const std::string &backend);
    static GraphicsBackendBase* Current();

    static int GetInFlightFrameIndex();
    static constexpr inline int GetMaxFramesInFlight()
    {
        return 2;
    }
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_API_H
