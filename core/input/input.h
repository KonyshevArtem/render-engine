#ifndef OPENGL_STUDY_INPUT_H
#define OPENGL_STUDY_INPUT_H

#include "../../math/vector2/vector2.h"

namespace Input
{
    void           Init();
    void           Update();
    void           CleanUp();
    bool           GetKeyDown(unsigned char _key);
    bool           GetKeyUp(unsigned char _key);
    bool           GetKey(unsigned char _key);
    const Vector2 &GetMousePosition();
    const Vector2 &GetMouseDelta();
}; // namespace Input

#endif //OPENGL_STUDY_INPUT_H
