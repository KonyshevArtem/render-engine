#include "worker.h"

#include <chrono>

std::unordered_map<std::thread::id, int32_t> Worker::s_WorkerIds;
std::vector<std::shared_ptr<Worker>> Worker::s_Workers;
std::queue<std::shared_ptr<Worker::Task>> Worker::s_Tasks;
std::mutex Worker::s_TasksMutex;

Worker::Worker() :
    m_Thread(&Worker::Run, this),
    m_Running(true)
{
}

Worker::~Worker()
{
    m_Running = false;
    if (m_Thread.joinable())
        m_Thread.join();
}

void Worker::Init()
{
    uint32_t cores = std::min(std::thread::hardware_concurrency() - 1, 6U);
    for (int32_t i = 0; i < cores - 1; ++i)
    {
        std::shared_ptr<Worker> worker = std::make_shared<Worker>();
        s_Workers.push_back(worker);
        s_WorkerIds[worker->m_Thread.get_id()] = i;
    }
}

void Worker::Shutdown()
{
    {
        std::lock_guard<std::mutex> lock(s_TasksMutex);
        while (!s_Tasks.empty())
            s_Tasks.pop();
    }

    s_Workers.clear();
}

std::shared_ptr<Worker::Task> Worker::CreateTask(const std::function<void()>& taskFunc)
{
    std::shared_ptr<Worker::Task> task = std::make_shared<Worker::Task>();
    task->Func = taskFunc;
    task->IsFinished = false;

    std::lock_guard<std::mutex> lock(s_TasksMutex);
    s_Tasks.push(task);
    return task;
}

std::shared_ptr<Worker::Task> Worker::Noop()
{
    std::shared_ptr<Worker::Task> task = std::make_shared<Worker::Task>();
    task->IsFinished = true;
    return task;
}

void Worker::WaitForTask(const std::shared_ptr<Task>& task)
{
    while (!task->IsFinished)
        continue;
}

int32_t Worker::GetWorkerId()
{
    std::thread::id threadId = std::this_thread::get_id();
    auto it = s_WorkerIds.find(threadId);
    return it != s_WorkerIds.end() ? it->second : -1;
}

void Worker::Run()
{
    while (m_Running)
    {
        std::shared_ptr<Worker::Task> task;
        {
            std::lock_guard<std::mutex> lock(s_TasksMutex);
            if (!s_Tasks.empty())
            {
                task = s_Tasks.front();
                s_Tasks.pop();
            }
        }

        if (task)
        {
            task->Func();
            task->IsFinished = true;
        }
        else
            std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}