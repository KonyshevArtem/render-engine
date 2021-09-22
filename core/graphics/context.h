#ifndef OPENGL_STUDY_CONTEXT_H
#define OPENGL_STUDY_CONTEXT_H

#include "../../math/matrix4x4/matrix4x4.h"
#include "../../math/vector4/vector4.h"
#include "../gameObject/gameObject.h"
#include "../light/light.h"
#include "../texture_2d/texture_2d.h"
#include "../texture_2d_array/texture_2d_array.h"
#include "memory"
#include "string"
#include "unordered_map"
#include "vector"

using namespace std;

class Context
{
public:
    Context();

    Vector4                                           AmbientLight;
    vector<shared_ptr<Light>>                         Lights;
    vector<shared_ptr<GameObject>>                    GameObjects;
    Matrix4x4                                         ViewMatrix       = Matrix4x4();
    Matrix4x4                                         ProjectionMatrix = Matrix4x4();
    unordered_map<string, shared_ptr<Texture2D>>      Textures2D;
    unordered_map<string, shared_ptr<Texture2DArray>> Texture2DArrays;
};

#endif //OPENGL_STUDY_CONTEXT_H
