#ifndef OPENGL_STUDY_MATERIAL_H
#define OPENGL_STUDY_MATERIAL_H

#include "../shader/shader.h"
#include "../texture/texture.h"

using namespace std;

class Material
{
public:
    explicit Material(shared_ptr<Shader> shader);

    shared_ptr<Shader>  ShaderPtr;
    shared_ptr<Texture> Texture;
    float               Smoothness;
};

#endif //OPENGL_STUDY_MATERIAL_H
