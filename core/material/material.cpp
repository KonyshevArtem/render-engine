#include "material.h"

#include <utility>

Material::Material(shared_ptr<Shader> _shader)
{
    ShaderPtr  = std::move(_shader);
    Albedo     = Texture::White();
    AlbedoST   = Vector4(0, 0, 1, 1);
    Smoothness = 0;
}
