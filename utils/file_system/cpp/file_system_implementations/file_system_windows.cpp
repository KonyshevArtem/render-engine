#if RENDER_ENGINE_WINDOWS

#include "file_system_windows.h"
#include <windows.h>
#include <ShlObj.h>

FileSystemWindows::FileSystemWindows() : FileSystemBase()
{
    char executablePath[MAX_PATH];
    GetModuleFileNameA(NULL, executablePath, MAX_PATH);
    m_BuildResourcesPath = std::filesystem::path(executablePath).parent_path();
	m_EditorResourcesPath = m_BuildResourcesPath / "editor_resources";
}

std::filesystem::path FileSystemWindows::GetSpecialFolderPath_Internal(SpecialFolder folder)
{
	PWSTR path = nullptr;
	if (folder == SpecialFolder::DOCUMENTS)
		SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, nullptr, &path);
	std::filesystem::path result(path);
	CoTaskMemFree(path);
	return result;
}

#endif
