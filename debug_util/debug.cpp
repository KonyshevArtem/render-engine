#include "debug.h"

#include <iostream>
#include <map>

#if __has_include("android/log.h")
#include <android/log.h>
#endif

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
#if RENDER_ENGINE_ANDROID
    __android_log_write(ANDROID_LOG_INFO, "RenderEngine", string.c_str());
#else
    std::cout << "[INFO]" << string << std::endl;
#endif

    NotifyListeners(string);
}

void Debug::LogWarning(const std::string& string)
{
#if RENDER_ENGINE_ANDROID
    __android_log_write(ANDROID_LOG_WARN, "RenderEngine", string.c_str());
#else
    std::cout << "[WARNING]" << string << std::endl;
#endif

    NotifyListeners(string);
}

void Debug::LogError(const std::string& string)
{
#if RENDER_ENGINE_ANDROID
    __android_log_write(ANDROID_LOG_ERROR, "RenderEngine", string.c_str());
#else
    std::cerr << "[ERROR] " << string << std::endl;
#endif

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