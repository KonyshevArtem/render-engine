#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_TLAS_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_TLAS_H

struct GraphicsBackendTLAS
{
public:
    bool IsValid() const
    {
        return TLAS != nullptr;
    }

private:
    void* TLAS = nullptr;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
    friend class GraphicsBackendBase;
};

#endif // RENDER_ENGINE_GRAPHICS_BACKEND_TLAS_H
