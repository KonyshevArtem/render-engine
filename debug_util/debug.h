#ifndef OPENGL_STUDY_DEBUG_H
#define OPENGL_STUDY_DEBUG_H

#include <string>

namespace Debug
{
    void Init();
    void LogError(const std::string &_string);
    void LogErrorFormat(const std::string &_format, std::initializer_list<std::string> _values);
} // namespace Debug

#endif