#ifndef RENDER_ENGINE_RESOURCES_H
#define RENDER_ENGINE_RESOURCES_H

#include <memory>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include <shared_mutex>
#include <vector>

#include "worker/worker.h"
#include "arguments.h"

class Resource;
class Texture;
class TextureBinaryReader;

class Resources
{
public:
    template<typename T>
    static std::shared_ptr<T> Load(const std::filesystem::path& path, bool asyncSubresourceLoads = false);

    template<typename T>
    static std::shared_ptr<Worker::Task> LoadAsync(const std::filesystem::path& path, const std::function<void(std::shared_ptr<T>)>& callback)
    {
        static bool syncLoading = Arguments::Contains("-resources_sync_load");
        if (syncLoading)
        {
            std::shared_ptr<T> resource = Load<T>(path);
            callback(resource);
            return Worker::Noop();
        }

        std::shared_ptr<T> cachedResource;
        if (TryGetFromCache(path, cachedResource))
        {
            callback(cachedResource);
            return Worker::Noop();
        }

        std::shared_ptr<Worker::Task> task;
        {
            auto AddCallback = [&callback](AsyncLoadRequest& request)
            {
                request.Callbacks.push_back([callback](std::shared_ptr<Resource> resource){ callback(std::dynamic_pointer_cast<T>(resource)); });
            };

            std::unique_lock lock(s_AsyncLoadRequestsMutex);
            auto it = s_AsyncLoadRequests.find(path);
            if (it != s_AsyncLoadRequests.end())
            {
                AddCallback(it->second);
                task = it->second.Task;
            }
            else
            {
                task = Worker::CreateTask([path](){ LoadTask<T>(path); });
                task->Schedule();

                AsyncLoadRequest request {task};
                AddCallback(request);
                s_AsyncLoadRequests[path] = std::move(request);
            }
        }

        return task;
    }

    static void UnloadAllResources();

private:
    struct AsyncLoadRequest
    {
        std::shared_ptr<Worker::Task> Task;
        std::vector<std::function<void(std::shared_ptr<Resource>)>> Callbacks;
    };

    static void UploadPixels(Texture& texture, int facesCount, int mipCount, TextureBinaryReader& reader);

    static std::unordered_map<std::filesystem::path, std::shared_ptr<Resource>> s_LoadedResources;
    static std::unordered_map<std::filesystem::path, AsyncLoadRequest> s_AsyncLoadRequests;

    static std::shared_mutex s_LoadedResourcesMutex;
    static std::shared_mutex s_AsyncLoadRequestsMutex;

    static void AddToCache(const std::filesystem::path& path, std::shared_ptr<Resource> resource);

    template<typename T>
    static bool TryGetFromCache(const std::filesystem::path& path, std::shared_ptr<T>& outResource)
    {
        std::shared_lock lock(s_LoadedResourcesMutex);
        auto it = s_LoadedResources.find(path);
        if (it == s_LoadedResources.end())
            return false;

        outResource = std::dynamic_pointer_cast<T>(it->second);
        return true;
    }

    template<typename T>
    static void LoadTask(const std::filesystem::path& path)
    {
        std::shared_ptr<T> resource = Load<T>(path, true);

        std::unique_lock lock(s_AsyncLoadRequestsMutex);
        AsyncLoadRequest& request = s_AsyncLoadRequests[path];

        for (const auto& callback: request.Callbacks)
            callback(resource);

        s_AsyncLoadRequests.erase(path);
    }
};

#endif //RENDER_ENGINE_RESOURCES_H
