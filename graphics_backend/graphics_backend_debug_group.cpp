#include "graphics_backend_debug_group.h"
#include "graphics_backend_api.h"

GraphicsBackendDebugGroup::GraphicsBackendDebugGroup(const std::string& name, GPUQueue queue) : Queue(queue)
{
    GraphicsBackend::Current()->PushDebugGroup(name, Queue);
}

GraphicsBackendDebugGroup::~GraphicsBackendDebugGroup()
{
    GraphicsBackend::Current()->PopDebugGroup(Queue);
}

