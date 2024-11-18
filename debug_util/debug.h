#ifndef RENDER_ENGINE_DEBUG_H
#define RENDER_ENGINE_DEBUG_H

#include <string>
#include <functional>
#include <format>

namespace Debug
{
    void LogInfo(const std::string& string);
    void LogWarning(const std::string& string);
    void LogError(const std::string& string);

    template<typename... Args>
    inline void LogInfoFormat(const std::format_string<Args...>& format, Args&&... values)
    {
        LogInfo(std::vformat(format.get(), std::make_format_args(values...)));
    }

    template<typename... Args>
    inline void LogWarningFormat(const std::format_string<Args...>& format, Args&&... values)
    {
        LogWarning(std::vformat(format.get(), std::make_format_args(values...)));
    }

    template<typename... Args>
    inline void LogErrorFormat(const std::format_string<Args...>& format, Args&&... values)
    {
        LogError(std::vformat(format.get(), std::make_format_args(values...)));
    }

    void AddListener(int listenerId, std::function<void(std::string)> listener);
    void RemoveListener(int listenerId);
} // namespace Debug

#endif