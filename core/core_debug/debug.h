#include <string>

namespace Debug
{
    void Init();
    void LogError(const std::string &_string);
    void LogErrorFormat(const std::string &_format, std::initializer_list<std::string> _values);
    void CheckOpenGLError();
}