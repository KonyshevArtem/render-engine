#ifndef RENDER_ENGINE_WORKER_H
#define RENDER_ENGINE_WORKER_H

#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <functional>
#include <atomic>
#include <unordered_map>

class Worker
{
public:
    Worker();
    ~Worker();

    static void Init();
    static void Shutdown();
    static void CreateTask(const std::function<void()>& task);
    static uint32_t GetWorkerId();

private:
    static std::unordered_map<std::thread::id, uint32_t> s_WorkerIds;
    static std::vector<std::shared_ptr<Worker>> s_Workers;
    static std::queue<std::function<void()>> s_Tasks;
    static std::mutex s_TasksMutex;

    std::thread m_Thread;
    std::atomic<bool> m_Running;

    void Run();
};

#endif //RENDER_ENGINE_WORKER_H
