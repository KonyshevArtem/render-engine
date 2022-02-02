#ifndef OPENGL_STUDY_BILLBOARD_RENDERER_H
#define OPENGL_STUDY_BILLBOARD_RENDERER_H

#include "renderer.h"
#ifdef OPENGL_STUDY_WINDOWS
#include <GL/glew.h>
#elif OPENGL_STUDY_MACOS
#include <OpenGL/gl3.h>
#endif
#include <memory>

class GameObject;
class Texture2D;
class Shader;

class BillboardRenderer: public Renderer
{
public:
    BillboardRenderer(const std::shared_ptr<GameObject> &_gameObject, std::shared_ptr<Texture2D> _texture);
    virtual ~BillboardRenderer();

    void Render() const override;

    inline void SetRenderQueue(int _renderQueue)
    {
        m_RenderQueue = _renderQueue;
    }

    inline void SetSize(float _size)
    {
        m_Size = _size;
    };

private:
    BillboardRenderer(const BillboardRenderer &) = delete;
    BillboardRenderer(BillboardRenderer &&)      = delete;

    BillboardRenderer &operator=(const BillboardRenderer &) = delete;
    BillboardRenderer &operator=(BillboardRenderer &&)      = delete;

    std::shared_ptr<Texture2D> m_Texture;
    float                      m_Size;
    int                        m_RenderQueue       = 2000;
    GLuint                     m_VertexArrayObject = 0;
    GLuint                     m_PointsBuffer      = 0;

    int GetRenderQueue() const override;
};

#endif //OPENGL_STUDY_BILLBOARD_RENDERER_H