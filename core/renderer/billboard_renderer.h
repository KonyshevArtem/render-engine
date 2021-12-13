#ifndef OPENGL_STUDY_BILLBOARD_RENDERER_H
#define OPENGL_STUDY_BILLBOARD_RENDERER_H

#include "renderer.h"
#include <OpenGL/gl3.h>
#include <memory>

class GameObject;
class Texture2D;
class Shader;

class BillboardRenderer: public Renderer
{
public:
    BillboardRenderer(shared_ptr<GameObject> _gameObject, shared_ptr<Texture2D> _texture);
    virtual ~BillboardRenderer();

    void Render() override;
    void Render(const shared_ptr<Shader> &_shader) override;

    inline void SetSize(float _size)
    {
        m_Size = _size;
    };

private:
    static shared_ptr<Shader> m_Shader;
    shared_ptr<Texture2D>     m_Texture;
    float                     m_Size;
    GLuint                    m_VertexArrayObject = 0;
    GLuint                    m_PointsBuffer      = 0;
};

#endif //OPENGL_STUDY_BILLBOARD_RENDERER_H
