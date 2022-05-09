#ifndef OPENGL_STUDY_DEBUG_H
#define OPENGL_STUDY_DEBUG_H

#include <string>

namespace Debug
{
    struct DebugGroup
    {
        DebugGroup(const std::string &_name);
        ~DebugGroup();
    };

    void Init();
    void LogError(const std::string &_string);
    void LogErrorFormat(const std::string &_format, std::initializer_list<std::string> _values);
    void CheckOpenGLError(const std::string &_file, int _line);

#ifdef OPENGL_STUDY_EDITOR
#define CHECK_GL(glFunction) glFunction; Debug::CheckOpenGLError(__FILE__, __LINE__)
#else
#define CHECK_GL(glFunction) glFunction
#endif
} // namespace Debug

#endif