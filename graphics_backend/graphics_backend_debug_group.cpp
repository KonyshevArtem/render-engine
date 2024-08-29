#include "graphics_backend_debug_group.h"
#include "graphics_backend_api.h"
#include "debug.h"

static int debugGroupID = 0;

GraphicsBackendDebugGroup::GraphicsBackendDebugGroup(const std::string &_name)
{
    GraphicsBackend::Current()->PushDebugGroup(_name, debugGroupID++);
}

GraphicsBackendDebugGroup::~GraphicsBackendDebugGroup()
{
    GraphicsBackend::Current()->PopDebugGroup();
    --debugGroupID;

    if (debugGroupID < 0)
    {
        Debug::LogError("Popping more debug groups than pushing");
        debugGroupID = 0;
    }
}

