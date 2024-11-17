#include "graphics_backend_debug_group.h"
#include "graphics_backend_api.h"

GraphicsBackendDebugGroup::GraphicsBackendDebugGroup(const std::string& name)
{
    GraphicsBackend::Current()->PushDebugGroup(name);
}

GraphicsBackendDebugGroup::~GraphicsBackendDebugGroup()
{
    GraphicsBackend::Current()->PopDebugGroup();
}

