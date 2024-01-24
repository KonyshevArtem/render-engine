#include "debug.h"

#include <boost/format.hpp>
#include <iostream>

void Debug::Init()
{
#if defined(OPENGL_STUDY_WINDOWS) && defined(_DEBUG)
    AllocConsole();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    auto *cout = freopen("CONOUT$", "w", stdout);
    auto *cerr = freopen("CONOUT$", "w", stderr);
#endif
}

void Debug::LogError(const std::string &_string)
{
    std::cerr << "[ERROR] " << _string << std::endl;
}

void Debug::LogErrorFormat(const std::string &_format, std::initializer_list<std::string> _values)
{
    boost::format format(_format);
    for (const auto &v: _values)
        format = format % v;
    LogError(format.str());
}