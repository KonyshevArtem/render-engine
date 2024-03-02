#include "debug.h"

#include <boost/format.hpp>
#include <iostream>
#include <unordered_map>

std::unordered_map<int, std::function<void(std::string)>> s_Listeners;

void Debug::LogError(const std::string &_string)
{
    std::cerr << "[ERROR] " << _string << std::endl;

    for (const auto &pair : s_Listeners)
    {
        pair.second(_string);
    }
}

void Debug::LogErrorFormat(const std::string &_format, std::initializer_list<std::string> _values)
{
    boost::format format(_format);
    for (const auto &v: _values)
        format = format % v;
    LogError(format.str());
}

void Debug::AddListener(int listenerId, std::function<void(std::string)> listener)
{
    s_Listeners[listenerId] = std::move(listener);
}

void Debug::RemoveListener(int listenerId)
{
    s_Listeners.erase(listenerId);
}