#include "worker.h"

#include <chrono>

std::vector<std::shared_ptr<Worker>> Worker::s_Workers;
std::queue<std::function<void()>> Worker::s_Tasks;
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
    uint32_t cores = std::thread::hardware_concurrency();
    for (uint32_t i = 0; i < cores - 1; ++i)
        s_Workers.push_back(std::make_shared<Worker>());
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

void Worker::Run()
{
    while (m_Running)
    {
        std::function<void()> task;
        {
            std::lock_guard<std::mutex> lock(s_TasksMutex);
            if (!s_Tasks.empty())
            {
                task = s_Tasks.front();
                s_Tasks.pop();
            }
        }

        if (task)
            task();
        else
            std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}