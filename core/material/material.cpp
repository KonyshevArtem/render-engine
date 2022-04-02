#include "material.h"
#include "shader/shader.h"
#include "texture/texture.h"
#include <utility>

Material::Material(std::shared_ptr<Shader> _shader) :
    m_Shader(std::move(_shader))
{
}
