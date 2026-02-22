#include "debug.h"

#include <iostream>
#include <map>
#include <shared_mutex>

#if __has_include("android/log.h")
#include <android/log.h>
#endif

#if __has_include("Windows.h")
#include "Windows.h"
#endif

std::shared_mutex s_LogMutex;

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

#if RENDER_ENGINE_WINDOWS

bool HasConsoleWindow()
{
    return GetConsoleWindow();
}

#endif

void Debug::LogInfo(const std::string& string)
{
    std::unique_lock lock(s_LogMutex);

#if RENDER_ENGINE_ANDROID
    __android_log_write(ANDROID_LOG_INFO, "RenderEngine", string.c_str());
#else
#if RENDER_ENGINE_WINDOWS
    if (!HasConsoleWindow())
    {
        OutputDebugStringA("[INFO] ");
        OutputDebugStringA(string.c_str());
        OutputDebugStringA("\n");
    }
    else
#endif
    std::cout << "[INFO]" << string << std::endl;
#endif

    NotifyListeners(string);
}

void Debug::LogWarning(const std::string& string)
{
    std::unique_lock lock(s_LogMutex);

#if RENDER_ENGINE_ANDROID
    __android_log_write(ANDROID_LOG_WARN, "RenderEngine", string.c_str());
#else
#if RENDER_ENGINE_WINDOWS
    if (!HasConsoleWindow())
    {
        OutputDebugStringA("[WARNING] ");
        OutputDebugStringA(string.c_str());
        OutputDebugStringA("\n");
    }
    else
#endif
    std::cout << "[WARNING]" << string << std::endl;
#endif

    NotifyListeners(string);
}

void Debug::LogError(const std::string& string)
{
    std::unique_lock lock(s_LogMutex);

#if RENDER_ENGINE_ANDROID
    __android_log_write(ANDROID_LOG_ERROR, "RenderEngine", string.c_str());
#else
#if RENDER_ENGINE_WINDOWS
    if (!HasConsoleWindow())
    {
        OutputDebugStringA("[ERROR] ");
        OutputDebugStringA(string.c_str());
        OutputDebugStringA("\n");
    }
    else
#endif
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