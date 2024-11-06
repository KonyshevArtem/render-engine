#ifndef __OBJC__
#define __OBJC__
#endif

#include "imgui_wrapper_apple.h"

#ifdef ENABLE_IMGUI
#include "imgui_impl_metal.h"
#include "imgui_impl_osx.h"

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

bool ImGui_OSX_Init(MTKView* _Nonnull view)
{
    return ImGui_ImplOSX_Init(static_cast<NSView*>(view));
}

void ImGui_OSX_Shutdown()
{
    ImGui_ImplOSX_Shutdown();
}

void ImGui_OSX_NewFrame(MTKView* _Nullable view)
{
    ImGui_ImplOSX_NewFrame(static_cast<NSView*>(view));
}

#else

bool ImGui_Metal_Init(id<MTLDevice> device){}
void ImGui_Metal_Shutdown(){}
void ImGui_Metal_NewFrame(MTLRenderPassDescriptor* renderPassDescriptor){}
void ImGui_Metal_Render(id<MTLCommandBuffer> commandBuffer, id<MTLRenderCommandEncoder> commandEncoder){}
bool ImGui_OSX_Init(MTKView* _Nonnull view){return true;}
void ImGui_OSX_Shutdown(){}
void ImGui_OSX_NewFrame(MTKView* _Nullable view){}

#endif