#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_BLAS_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_BLAS_H

struct GraphicsBackendBLAS
{
public:
    bool IsValid() const
    {
        return BLAS != nullptr;
    }

private:
	void* BLAS = nullptr;

    friend class GraphicsBackendOpenGL;
    friend class GraphicsBackendMetal;
    friend class GraphicsBackendDX12;
    friend class GraphicsBackendBase;
};

#endif // RENDER_ENGINE_GRAPHICS_BACKEND_BLAS_H
