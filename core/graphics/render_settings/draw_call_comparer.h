#ifndef OPENGL_STUDY_DRAW_CALL_COMPARER_H
#define OPENGL_STUDY_DRAW_CALL_COMPARER_H

#include <vector3/vector3.h>

class Renderer;
struct DrawCallInfo;

enum class DrawCallSortMode
{
    FRONT_TO_BACK,
    BACK_TO_FRONT,
    NO_SORTING
};

struct DrawCallComparer
{
    DrawCallSortMode SortMode = DrawCallSortMode::FRONT_TO_BACK;
    Vector3 CameraPosition = Vector3();

    bool operator()(const DrawCallInfo &drawCallA, const DrawCallInfo &drawCallB);
};

#endif //OPENGL_STUDY_DRAW_CALL_COMPARER_H
