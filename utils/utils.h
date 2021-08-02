#ifndef OPENGL_STUDY_UTILS_H
#define OPENGL_STUDY_UTILS_H

#define CHECK_GL_ERROR printf("%s\n", gluErrorString(glGetError()));

#include "string"
#include "filesystem"

using namespace std;

class Utils
{
public:
    static string ReadFile(const filesystem::path &path);
    static string ReadFileWithIncludes(const filesystem::path &path);
};

#endif //OPENGL_STUDY_UTILS_H
