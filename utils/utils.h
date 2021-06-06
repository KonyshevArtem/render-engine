#ifndef OPENGL_STUDY_UTILS_H
#define OPENGL_STUDY_UTILS_H

#define CHECK_GL_ERROR printf("%s\n", gluErrorString(glGetError()));

char *readFile(char *path);

float float_lerp(float a, float b, float t);

#endif //OPENGL_STUDY_UTILS_H
