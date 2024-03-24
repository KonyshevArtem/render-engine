#include "graphics_backend_api_base.h"
#include "graphics_backend_api_opengl.h"
#include "graphics_backend_api_metal.h"

GraphicsBackendBase *GraphicsBackendBase::Create(const std::string &backend)
{
    if (backend == "OpenGL")
    {
        return new GraphicsBackendOpenGL();
    }

#if RENDER_ENGINE_APPLE
    if (backend == "Metal")
    {
        return new GraphicsBackendMetal();
    }
#endif

    return nullptr;
}
