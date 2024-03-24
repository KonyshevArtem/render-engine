#include "graphics_backend_api.h"
#include "graphics_backend_implementations/graphics_backend_api_base.h"

GraphicsBackendBase *s_GraphicsBackend;

void GraphicsBackend::Init(const std::string &backend)
{
    s_GraphicsBackend = GraphicsBackendBase::Create(backend);
    s_GraphicsBackend->Init();
}

GraphicsBackendBase *GraphicsBackend::Current()
{
    return s_GraphicsBackend;
}