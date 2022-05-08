#include "point.h"

Point::Point() :
    DrawableGeometry()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    float zero[3] {0, 0, 0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(zero), &zero, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}