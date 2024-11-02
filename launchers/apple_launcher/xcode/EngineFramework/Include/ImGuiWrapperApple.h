#ifndef RENDER_ENGINE_IMGUI_WRAPPER_APPLE_H
#define RENDER_ENGINE_IMGUI_WRAPPER_APPLE_H

@class MTLRenderPassDescriptor;
@protocol MTLDevice, MTLCommandBuffer, MTLRenderCommandEncoder;

bool ImGui_Metal_Init(id<MTLDevice> device);
void ImGui_Metal_Shutdown();
void ImGui_Metal_NewFrame(MTLRenderPassDescriptor* renderPassDescriptor);
void ImGui_Metal_Render(id<MTLCommandBuffer> commandBuffer, id<MTLRenderCommandEncoder> commandEncoder);

@class NSEvent;
@class NSView;

bool ImGui_OSX_Init(NSView* _Nonnull view);
void ImGui_OSX_Shutdown();
void ImGui_OSX_NewFrame(NSView* _Nullable view);

#endif //RENDER_ENGINE_IMGUI_WRAPPER_APPLE_H
