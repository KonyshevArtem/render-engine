#if RENDER_ENGINE_WINDOWS

#include "file_system_windows.h"
#include <windows.h>

FileSystemWindows::FileSystemWindows() : FileSystemBase()
{
    char executablePath[MAX_PATH];
    GetModuleFileNameA(NULL, executablePath, MAX_PATH);
    m_ResourcesPath = std::filesystem::path(executablePath).parent_path();
}

#endif