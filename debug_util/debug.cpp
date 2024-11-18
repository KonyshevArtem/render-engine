#include "debug.h"

#include <iostream>
#include <map>

std::map<int, std::function<void(std::string)>> *GetListeners()
{
    static auto *listeners = new std::map<int, std::function<void(std::string)>>();
    return listeners;
}

void NotifyListeners(const std::string& string)
{
    auto &listeners = *GetListeners();
    for (const auto &pair : listeners)
    {
        pair.second(string);
    }
}

void Debug::LogInfo(const std::string& string)
{
    std::cout << "[INFO]" << string << std::endl;
    NotifyListeners(string);
}

void Debug::LogWarning(const std::string& string)
{
    std::cout << "[WARNING]" << string << std::endl;
    NotifyListeners(string);
}

void Debug::LogError(const std::string& string)
{
    std::cerr << "[ERROR] " << string << std::endl;
    NotifyListeners(string);
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