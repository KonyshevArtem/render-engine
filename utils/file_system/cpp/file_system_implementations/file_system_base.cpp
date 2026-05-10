#include "file_system_base.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

bool FileSystemBase::FileExists(const std::filesystem::path& path)
{
    return std::filesystem::exists(path);
}

std::string FileSystemBase::ReadFile(const std::filesystem::path& path)
{
    FILE* file = fopen(path.string().c_str(), "r");
    if (file == nullptr)
	    throw std::runtime_error("[Utils] Can't open file: " + path.string());

    if (fseek(file, 0, SEEK_END) != 0)
	    throw std::runtime_error("[Utils] Error reading file: " + path.string());

    auto fileSize = ftell(file);
    if (fileSize == -1L)
	    throw std::runtime_error("[Utils] Error reading file: " + path.string());

    if (fseek(file, 0, SEEK_SET) != 0)
	    throw std::runtime_error("[Utils] Error reading file: " + path.string());

    std::string content(fileSize, ' ');
    int c;
    long i = -1;

    while ((c = fgetc(file)) != EOF)
        content[++i] = static_cast<char>(c);

    content[fileSize] = 0;

    fclose(file);

    return content;
}

bool FileSystemBase::ReadFileBytes(const std::filesystem::path& path, std::vector<uint8_t>& bytes)
{
    std::ifstream input(path.string(), std::ios::in | std::ios::binary);
    if (!input.is_open() || input.bad())
        return false;

    input.unsetf(std::ios::skipws);

    input.seekg(0, std::ios_base::end);
    size_t fileSize = input.tellg();
    input.seekg(0, std::ios_base::beg);

    bytes.reserve(fileSize);
    bytes.insert(bytes.begin(),
                 (std::istreambuf_iterator<char>(input)),
                 (std::istreambuf_iterator<char>()));

    input.close();

    return bytes.size() == fileSize;
}

void FileSystemBase::WriteFile(const std::filesystem::path& path, const std::string& content)
{
    std::filesystem::create_directories(path.parent_path());

    FILE* fp = fopen(path.string().c_str(), "w");
    fwrite(content.c_str(), content.size(), 1, fp);
    fclose(fp);
}

void FileSystemBase::WriteFileBytes(const std::filesystem::path& path, const std::span<const uint8_t>& bytes)
{
    std::filesystem::create_directories(path.parent_path());

    FILE* fp = fopen(path.string().c_str(), "wb");
    fwrite(bytes.data(), bytes.size(), 1, fp);
    fclose(fp);
}

const std::filesystem::path& FileSystemBase::GetBuildResourcesPath()
{
    return m_BuildResourcesPath;
}

const std::filesystem::path& FileSystemBase::GetEditorResourcesPath()
{
	return m_EditorResourcesPath;
}

const std::filesystem::path& FileSystemBase::GetSpecialFolderPath(SpecialFolder folder)
{
    const auto it = m_SpecialFolderPaths.find(folder);
    if (it != m_SpecialFolderPaths.end())
	    return it->second;

    m_SpecialFolderPaths[folder] = GetSpecialFolderPath_Internal(folder);
    return m_SpecialFolderPaths[folder];
}

std::filesystem::path FileSystemBase::GetSpecialFolderPath_Internal(SpecialFolder folder)
{
    return "";
}
