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
    BillboardRenderer(const shared_ptr<GameObject> &_gameObject, shared_ptr<Texture2D> _texture);
    BillboardRenderer(const BillboardRenderer &) = delete;
    virtual ~BillboardRenderer();

    void Render() const override;
    void Render(const Shader &_shader) const override;

    inline void SetSize(float _size)
    {
        m_Size = _size;
    };

private:
    shared_ptr<Texture2D> m_Texture;
    float                 m_Size;
    GLuint                m_VertexArrayObject = 0;
    GLuint                m_PointsBuffer      = 0;
};

#endif //OPENGL_STUDY_BILLBOARD_RENDERER_H
