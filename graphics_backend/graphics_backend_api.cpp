#include "graphics_backend_api.h"
#include "graphics_backend_implementations/graphics_backend_api_base.h"

GraphicsBackendBase *s_GraphicsBackend;

void GraphicsBackend::Init(void *data)
{
    s_GraphicsBackend = GraphicsBackendBase::Create();
    s_GraphicsBackend->Init(data);
}

GraphicsBackendBase *GraphicsBackend::Current()
{
    return s_GraphicsBackend;
}

int GraphicsBackend::GetInFlightFrameIndex()
{
    return Current()->GetFrameNumber() % GetMaxFramesInFlight();
}