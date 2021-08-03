#include "material.h"

Material::Material(shared_ptr<Shader> shader)
{
    ShaderPtr  = shader;
    Albedo     = Texture::White();
    AlbedoST   = Vector4(0, 0, 1, 1);
    Smoothness = 0;
}
