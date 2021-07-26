#include "material.h"

Material::Material(shared_ptr<Shader> shader)
{
    ShaderPtr  = shader;
    Texture    = Texture::White();
    Smoothness = 0;
}
