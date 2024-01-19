#ifndef OPENGL_STUDY_TEXTURE_UNIT_H
#define OPENGL_STUDY_TEXTURE_UNIT_H

#include "graphics_backend.h"

enum class TextureUnit : GRAPHICS_BACKEND_TYPE_ENUM
{
    TEXTURE0    = GRAPHICS_BACKEND_TEXTURE0,
    TEXTURE1    = GRAPHICS_BACKEND_TEXTURE1,
    TEXTURE2    = GRAPHICS_BACKEND_TEXTURE2,
    TEXTURE3    = GRAPHICS_BACKEND_TEXTURE3,
    TEXTURE4    = GRAPHICS_BACKEND_TEXTURE4,
    TEXTURE5    = GRAPHICS_BACKEND_TEXTURE5,
    TEXTURE6    = GRAPHICS_BACKEND_TEXTURE6,
    TEXTURE7    = GRAPHICS_BACKEND_TEXTURE7,
    TEXTURE8    = GRAPHICS_BACKEND_TEXTURE8,
    TEXTURE9    = GRAPHICS_BACKEND_TEXTURE9,
    TEXTURE10   = GRAPHICS_BACKEND_TEXTURE10,
    TEXTURE11   = GRAPHICS_BACKEND_TEXTURE11,
    TEXTURE12   = GRAPHICS_BACKEND_TEXTURE12,
    TEXTURE13   = GRAPHICS_BACKEND_TEXTURE13,
    TEXTURE14   = GRAPHICS_BACKEND_TEXTURE14,
    TEXTURE15   = GRAPHICS_BACKEND_TEXTURE15,
    TEXTURE16   = GRAPHICS_BACKEND_TEXTURE16,
    TEXTURE17   = GRAPHICS_BACKEND_TEXTURE17,
    TEXTURE18   = GRAPHICS_BACKEND_TEXTURE18,
    TEXTURE19   = GRAPHICS_BACKEND_TEXTURE19,
    TEXTURE20   = GRAPHICS_BACKEND_TEXTURE20,
    TEXTURE21   = GRAPHICS_BACKEND_TEXTURE21,
    TEXTURE22   = GRAPHICS_BACKEND_TEXTURE22,
    TEXTURE23   = GRAPHICS_BACKEND_TEXTURE23,
    TEXTURE24   = GRAPHICS_BACKEND_TEXTURE24,
    TEXTURE25   = GRAPHICS_BACKEND_TEXTURE25,
    TEXTURE26   = GRAPHICS_BACKEND_TEXTURE26,
    TEXTURE27   = GRAPHICS_BACKEND_TEXTURE27,
    TEXTURE28   = GRAPHICS_BACKEND_TEXTURE28,
    TEXTURE29   = GRAPHICS_BACKEND_TEXTURE29,
    TEXTURE30   = GRAPHICS_BACKEND_TEXTURE30,
    TEXTURE31   = GRAPHICS_BACKEND_TEXTURE31,
};

namespace TextureUnitUtils
{
    inline GraphicsBackendTextureUnitIndex TextureUnitToIndex(TextureUnit unit)
    {
        return static_cast<GraphicsBackendTextureUnitIndex>(static_cast<int>(unit) - static_cast<int>(TextureUnit::TEXTURE0));
    }
}

#endif //OPENGL_STUDY_TEXTURE_UNIT_H
