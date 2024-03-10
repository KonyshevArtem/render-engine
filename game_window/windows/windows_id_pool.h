#ifndef RENDER_ENGINE_WINDOWS_ID_POOL_H
#define RENDER_ENGINE_WINDOWS_ID_POOL_H

#include <vector>
#include <unordered_map>

class WindowsIdPool
{
public:
    int GetID(size_t typeHashCode);
    void ReturnID(size_t typeHashCode, int id);

private:
    std::unordered_map<size_t, int> m_WindowsCount;
    std::unordered_map<size_t, std::vector<int>> m_WindowsIdsPool;
};


#endif //RENDER_ENGINE_WINDOWS_ID_POOL_H
