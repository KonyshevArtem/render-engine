#include "point.h"
#include "debug.h"

Point::Point() :
    DrawableGeometry()
{
    CHECK_GL(glEnableVertexAttribArray(0));
    CHECK_GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr));

    float zero[3] {0, 0, 0};
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(zero), &zero, GL_DYNAMIC_DRAW));

    CHECK_GL(glBindVertexArray(0));
}