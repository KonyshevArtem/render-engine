#ifndef OPENGL_STUDY_MATERIAL_H
#define OPENGL_STUDY_MATERIAL_H

#include "../../math/vector4/vector4.h"
#include "../shader/shader.h"
#include "../texture_2d/texture_2d.h"
#include "unordered_map"

using namespace std;

class Material
{
public:
    explicit Material(shared_ptr<Shader> _shader);

    void SetTexture(const string &_name, shared_ptr<Texture2D> _value);
    void SetVector4(const string &_name, Vector4 _value);
    void SetFloat(const string &_name, float _value);

    shared_ptr<Texture2D> GetTexture(const string &_name);
    Vector4               GetVector4(const string &_name);
    float                 GetFloat(const string &_name);

private:
    shared_ptr<Shader>                           m_Shader;
    unordered_map<string, shared_ptr<Texture2D>> m_Textures2D;
    unordered_map<string, Vector4>               m_Vectors4;
    unordered_map<string, float>                 m_Floats;

    friend class RenderPass; // TODO: get rid of friendship
};

#endif //OPENGL_STUDY_MATERIAL_H
