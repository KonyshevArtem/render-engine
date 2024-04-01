#ifndef __OBJC__
#define __OBJC__
#endif

#include "imgui_wrapper_apple.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_metal.h"
#include "imgui_impl_osx.h"
#include "graphics_backend_api.h"
#include "graphics_backend_implementations/graphics_backend_api_opengl.h"

bool ImGui_OpenGL3_Init()
{
    auto backend = reinterpret_cast<GraphicsBackendOpenGL*>(GraphicsBackend::Current());
    return ImGui_ImplOpenGL3_Init(backend->GetGLSLVersionString().c_str());
}

void ImGui_OpenGL3_Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
}

void ImGui_OpenGL3_NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
}

void ImGui_OpenGL3_Render()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

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
