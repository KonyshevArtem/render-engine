#ifndef __OBJC__
#define __OBJC__
#endif

#include "imgui_wrapper_apple.h"
#include "imgui_impl_metal.h"
#include "imgui_impl_osx.h"
#include "graphics_backend_api.h"

bool ImGui_Metal_Init(id<MTLDevice> device)
{
    ImGui_ImplMetal_Init(device);
}

void ImGui_Metal_Shutdown()
{
    ImGui_ImplMetal_Shutdown();
}

void ImGui_Metal_NewFrame(MTLRenderPassDescriptor* renderPassDescriptor)
{
    ImGui_ImplMetal_NewFrame(renderPassDescriptor);
}

void ImGui_Metal_Render(id<MTLCommandBuffer> commandBuffer, id<MTLRenderCommandEncoder> commandEncoder)
{
    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, commandEncoder);
}

bool ImGui_OSX_Init(NSView* _Nonnull view)
{
    return ImGui_ImplOSX_Init(view);
}

void ImGui_OSX_Shutdown()
{
    ImGui_ImplOSX_Shutdown();
}

void ImGui_OSX_NewFrame(NSView* _Nullable view)
{
    ImGui_ImplOSX_NewFrame(view);
}
