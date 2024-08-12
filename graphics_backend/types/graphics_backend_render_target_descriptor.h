#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_RENDER_TARGET_DESCRIPTOR_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_RENDER_TARGET_DESCRIPTOR_H

#include "enums/framebuffer_attachment.h"
#include "enums/load_action.h"
#include "enums/store_action.h"
#include "graphics_backend_texture.h"

struct GraphicsBackendRenderTargetDescriptor
{
    FramebufferAttachment Attachment;
    GraphicsBackendTexture Texture;
    LoadAction LoadAction;
    StoreAction StoreAction;
    int Level;
    int Layer;
    bool IsBackbuffer;

    static inline GraphicsBackendRenderTargetDescriptor ColorBackbuffer(enum LoadAction loadAction = LoadAction::LOAD, enum StoreAction storeAction = StoreAction::STORE)
    {
        return { .Attachment = FramebufferAttachment::COLOR_ATTACHMENT0, .LoadAction = loadAction, .StoreAction = storeAction, .IsBackbuffer = true };
    }

    static inline GraphicsBackendRenderTargetDescriptor DepthBackbuffer(enum LoadAction loadAction = LoadAction::LOAD, enum StoreAction storeAction = StoreAction::STORE)
    {
        return { .Attachment = FramebufferAttachment::DEPTH_ATTACHMENT, .LoadAction = loadAction, .StoreAction = storeAction, .IsBackbuffer = true };
    }
};

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_RENDER_TARGET_DESCRIPTOR_H
