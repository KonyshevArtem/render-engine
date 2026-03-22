#ifndef RENDER_ENGINE_UNIFORM_INFO_H
#define RENDER_ENGINE_UNIFORM_INFO_H

#include <cstdint>

struct GraphicsBackendTextureInfo
{
    uint32_t Binding = 0;
    bool ReadWrite = false;
};

#endif //RENDER_ENGINE_UNIFORM_INFO_H
