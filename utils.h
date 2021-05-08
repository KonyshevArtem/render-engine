#ifndef OPENGL_STUDY_UTILS_H
#define OPENGL_STUDY_UTILS_H

#define CHECK_GL_ERROR printf("%s\n", gluErrorString(glGetError()));

char *readFile(char *path);

#endif //OPENGL_STUDY_UTILS_H
