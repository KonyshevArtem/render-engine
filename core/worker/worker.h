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
    struct Task
    {
        std::function<void()> Func;
        std::atomic<bool> IsFinished;
    };

    Worker();
    ~Worker();

    static void Init();
    static void Shutdown();

    static std::shared_ptr<Task> CreateTask(const std::function<void()>& taskFunc);
    static std::shared_ptr<Task> Noop();
    static void WaitForTask(const std::shared_ptr<Task>& task);

    static int32_t GetWorkerId();

private:
    static std::unordered_map<std::thread::id, int32_t> s_WorkerIds;
    static std::vector<std::shared_ptr<Worker>> s_Workers;
    static std::queue<std::shared_ptr<Task>> s_Tasks;
    static std::mutex s_TasksMutex;

    std::thread m_Thread;
    std::atomic<bool> m_Running;

    void Run();
};

#endif //RENDER_ENGINE_WORKER_H
