#ifndef OPENGL_STUDY_BILLBOARD_RENDERER_H
#define OPENGL_STUDY_BILLBOARD_RENDERER_H

#include "renderer.h"
#include "bounds/bounds.h"
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

    void   Render(const RenderSettings &_settings) const override;
    Bounds GetAABB() const override;

    inline void SetRenderQueue(int _renderQueue)
    {
        m_RenderQueue = _renderQueue;
    }

    void SetSize(float _size);

private:
    BillboardRenderer(const BillboardRenderer &) = delete;
    BillboardRenderer(BillboardRenderer &&)      = delete;

    BillboardRenderer &operator=(const BillboardRenderer &) = delete;
    BillboardRenderer &operator=(BillboardRenderer &&)      = delete;

    std::shared_ptr<Texture2D> m_Texture;
    float                      m_Size;
    Bounds                     m_Bounds;
    int                        m_RenderQueue       = 2000;
    GLuint                     m_VertexArrayObject = 0;
    GLuint                     m_PointsBuffer      = 0;

    int GetRenderQueue() const override;
};

#endif //OPENGL_STUDY_BILLBOARD_RENDERER_H
