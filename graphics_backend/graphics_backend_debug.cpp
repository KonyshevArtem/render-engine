#include "graphics_backend_debug.h"
#include "graphics_backend_api.h"
#include "debug.h"

static int debugGroupID = 0;

GraphicsBackendDebug::DebugGroup::DebugGroup(const std::string &_name)
{
    GraphicsBackend::Current()->PushDebugGroup(_name, debugGroupID++);
}

GraphicsBackendDebug::DebugGroup::~DebugGroup()
{
    GraphicsBackend::Current()->PopDebugGroup();
    --debugGroupID;

    if (debugGroupID < 0)
    {
        Debug::LogError("Popping more debug groups than pushing");
        debugGroupID = 0;
    }
}

