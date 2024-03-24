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

void GraphicsBackendDebug::CheckError()
{
    auto error = GraphicsBackend::Current()->GetError();
    if (error != 0)
        GraphicsBackendDebug::LogError(error, "", 0);
}

void GraphicsBackendDebug::LogError(GRAPHICS_BACKEND_TYPE_ENUM errorCode, const std::string &file, int line)
{
    auto *errorString = GraphicsBackend::Current()->GetErrorString(errorCode);
    if (errorString == nullptr)
        Debug::LogErrorFormat("[Graphics Backend] Unknown error %1%\n%2%:%3%", {std::to_string(errorCode), file, std::to_string(line)});
    else
        Debug::LogErrorFormat("[Graphics Backend] %1%\n%2%:%3%", {errorString, file, std::to_string(line)});
}