#ifndef RENDER_ENGINE_DEBUG_H
#define RENDER_ENGINE_DEBUG_H

#include <string>
#include <functional>

namespace Debug
{
    void LogError(const std::string &_string);
    void LogErrorFormat(const std::string &_format, std::initializer_list<std::string> _values);

    void AddListener(int listenerId, std::function<void(std::string)> listener);
    void RemoveListener(int listenerId);
} // namespace Debug

#endif