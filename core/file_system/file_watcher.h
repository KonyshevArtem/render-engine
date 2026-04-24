#ifndef RENDER_ENGINE_FILE_WATCHER_H
#define RENDER_ENGINE_FILE_WATCHER_H

#include <filesystem>
#include <unordered_map>

class FileWatcher
{
public:
	FileWatcher() = default;

	void AddFile(const std::filesystem::path& filePath);
	bool FilesChanged();

private:
	std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> m_WatchedFiles;
};

#endif