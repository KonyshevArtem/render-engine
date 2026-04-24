#include "file_watcher.h"
#include "file_system.h"

void FileWatcher::AddFile(const std::filesystem::path& filePath)
{
#ifdef RENDER_ENGINE_EDITOR
	const std::filesystem::path editorResourcesPath = FileSystem::GetEditorResourcesPath() / filePath;
	const std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(editorResourcesPath);
	m_WatchedFiles[editorResourcesPath] = lastWriteTime;
#endif
}

bool FileWatcher::FilesChanged()
{
	bool changed = false;

#ifdef RENDER_ENGINE_EDITOR
	for (auto& pair : m_WatchedFiles)
	{
		const std::filesystem::file_time_type lastWriteTime = std::filesystem::last_write_time(pair.first);
		if (lastWriteTime != pair.second)
		{
			pair.second = lastWriteTime;
			changed = true;
		}
	}
#endif

	return changed;
}
