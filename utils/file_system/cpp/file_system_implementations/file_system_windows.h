#ifndef RENDER_ENGINE_FILE_SYSTEM_WINDOWS_H
#define RENDER_ENGINE_FILE_SYSTEM_WINDOWS_H

#if RENDER_ENGINE_WINDOWS

#include "file_system_base.h"

class FileSystemWindows : public FileSystemBase
{
public:
    FileSystemWindows();

protected:
    std::filesystem::path GetSpecialFolderPath_Internal(SpecialFolder folder) override;
};

#endif

#endif //RENDER_ENGINE_FILE_SYSTEM_WINDOWS_H
