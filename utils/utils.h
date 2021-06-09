#ifndef OPENGL_STUDY_UTILS_H
#define OPENGL_STUDY_UTILS_H

#define CHECK_GL_ERROR printf("%s\n", gluErrorString(glGetError()));

#include "string"

class Utils
{
public:
    static char *ReadFile(const std::string &path);
};

#endif//OPENGL_STUDY_UTILS_H
