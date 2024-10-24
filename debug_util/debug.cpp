#include "debug.h"

#include <boost/format.hpp>
#include <iostream>
#include <map>

std::map<int, std::function<void(std::string)>> *GetListeners()
{
    static auto *listeners = new std::map<int, std::function<void(std::string)>>();
    return listeners;
}

void Debug::LogError(const std::string &_string)
{
    std::cerr << "[ERROR] " << _string << std::endl;

    auto &listeners = *GetListeners();
    for (const auto &pair : listeners)
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
    (*GetListeners())[listenerId] = std::move(listener);
}

void Debug::RemoveListener(int listenerId)
{
    auto listeners = GetListeners();
    if (listeners)
    {
        (*listeners).erase(listenerId);
    }
}