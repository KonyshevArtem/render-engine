#include "windows_id_pool.h"

int WindowsIdPool::GetID(size_t typeHashCode)
{
    auto it = m_WindowsIdsPool.find(typeHashCode);
    if (it == m_WindowsIdsPool.end())
    {
        m_WindowsCount[typeHashCode] = 0;
        m_WindowsIdsPool[typeHashCode] = {};
        return 0;
    }

    int windowsCount = m_WindowsCount[typeHashCode] + 1;
    m_WindowsCount[typeHashCode] = windowsCount;

    auto &pool = it->second;
    if (pool.empty())
    {
        return windowsCount;
    }

    int id = pool.back();
    pool.pop_back();
    return id;
}

void WindowsIdPool::ReturnID(size_t typeHashCode, int id)
{
    auto it = m_WindowsIdsPool.find(typeHashCode);
    if (it == m_WindowsIdsPool.end())
    {
        m_WindowsIdsPool[typeHashCode] = {id};
        m_WindowsCount[typeHashCode] = 0;
    }
    else
    {
        auto &pool = it->second;
        pool.push_back(id);

        --m_WindowsCount[typeHashCode];
    }
}