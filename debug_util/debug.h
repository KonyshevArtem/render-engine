#ifndef RENDER_ENGINE_DEBUG_H
#define RENDER_ENGINE_DEBUG_H

#include <string>
#include <functional>
#include <format>

namespace Debug
{
    void LogError(const std::string &_string);

    template<typename... Args>
    inline void LogErrorFormat(const std::format_string<Args...>& format, Args&&... values)
    {
        LogError(std::vformat(format.get(), std::make_format_args(values...)));
    }

    void AddListener(int listenerId, std::function<void(std::string)> listener);
    void RemoveListener(int listenerId);
} // namespace Debug

#endif