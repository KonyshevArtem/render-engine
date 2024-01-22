#include "graphics_backend_debug.h"
#include "graphics_backend_api.h"
#include "debug.h"

static int debugGroupID = 0;

GraphicsBackendDebug::DebugGroup::DebugGroup(const std::string &_name)
{
#ifdef GRAPHICS_BACKEND_WINDOWS
    CHECK_GL(glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, debugGroupID++, -1, _name.c_str()));
#endif
}

GraphicsBackendDebug::DebugGroup::~DebugGroup()
{
#ifdef GRAPHICS_BACKEND_WINDOWS
    glPopDebugGroup();
    --debugGroupID;

    if (debugGroupID < 0)
    {
        LogError("Popping more debug groups than pushing");
        debugGroupID = 0;
    }
#endif
}

void GraphicsBackendDebug::CheckError()
{
    GraphicsBackendError error = GraphicsBackend::GetError();
    if (error != 0)
        GraphicsBackendDebug::LogError(error, "", 0);
}

void GraphicsBackendDebug::LogError(GraphicsBackendError errorCode, const std::string &file, int line)
{
    auto *errorString = GraphicsBackend::GetErrorString(errorCode);
    if (errorString == nullptr)
        Debug::LogErrorFormat("[Graphics Backend] Unknown error %1%\n%2%:%3%", {std::to_string(errorCode), file, std::to_string(line)});
    else
        Debug::LogErrorFormat("[Graphics Backend] %1%\n%2%:%3%", {errorString, file, std::to_string(line)});
}