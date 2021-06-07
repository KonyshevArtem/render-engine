#ifndef OPENGL_STUDY_VECTOR4_H
#define OPENGL_STUDY_VECTOR4_H

typedef struct vector4
{
    float x;
    float y;
    float z;
    float w;
} vector4;

vector4 vector4_zero();

vector4 vector4_build(float x, float y, float z, float w);

vector4 vector4_toVector3(const vector4 *vector);

float vector4_length(const vector4 *vector);

vector4 vector4_normalize(const vector4 *vector);

#endif//OPENGL_STUDY_VECTOR4_H
