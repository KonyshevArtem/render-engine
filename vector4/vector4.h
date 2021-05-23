#ifndef OPENGL_STUDY_VECTOR4_H
#define OPENGL_STUDY_VECTOR4_H

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vector4;

vector4 vector4_zero();

vector4 vector4_build(float x, float y, float z, float w);

#endif //OPENGL_STUDY_VECTOR4_H
