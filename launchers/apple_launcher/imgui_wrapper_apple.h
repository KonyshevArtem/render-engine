#ifndef RENDER_ENGINE_IMGUI_WRAPPER_APPLE_H
#define RENDER_ENGINE_IMGUI_WRAPPER_APPLE_H

@class MTKView;
@class MTLRenderPassDescriptor;
@protocol MTLDevice, MTLCommandBuffer, MTLRenderCommandEncoder;

bool ImGui_Metal_Init(id<MTLDevice> device);
void ImGui_Metal_Shutdown();
void ImGui_Metal_NewFrame(MTLRenderPassDescriptor* renderPassDescriptor);
void ImGui_Metal_Render(id<MTLCommandBuffer> commandBuffer, id<MTLRenderCommandEncoder> commandEncoder);

bool ImGui_OSX_Init(MTKView* _Nonnull view);
void ImGui_OSX_Shutdown();
void ImGui_OSX_NewFrame(MTKView* _Nullable view);

#endif //RENDER_ENGINE_IMGUI_WRAPPER_APPLE_H
