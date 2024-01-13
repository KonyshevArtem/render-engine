#include "graphics_backend_debug.h"
#include "graphics_backend_api.h"
#include "debug.h"

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