#include "imgui_wrapper.h"
#include "imgui_wrapper_common.h"

#ifdef ENABLE_IMGUI

#include "imgui_impl_metal.h"
#include "imgui_impl_osx.h"
#include "MetalKit/MetalKit.hpp"

namespace ImGuiWrapper
{
    static MTK::View* s_View;
    static MTL::CommandQueue* s_RenderQueue;

    void Init(const std::function<void(void*)>& fillImGuiData)
    {
        struct ImGuiData
        {
            MTK::View* View;
            MTL::CommandQueue* RenderQueue;
        };

        ImGuiData data;
        fillImGuiData(static_cast<void*>(&data));
        s_View = data.View;
        s_RenderQueue = data.RenderQueue;

        ImGuiWrapperCommon::Init();
        ImGui_ImplMetal_Init(s_View->device());
        ImGui_ImplOSX_Init(s_View);
    }

    void Shutdown()
    {
        ImGui_ImplMetal_Shutdown();
        ImGui_ImplOSX_Shutdown();
        ImGuiWrapperCommon::Shutdown();
    }

    void NewFrame()
    {
        ImGui_ImplMetal_NewFrame(s_View->currentRenderPassDescriptor());
        ImGui_ImplOSX_NewFrame(s_View);
        ImGuiWrapperCommon::NewFrame();
    }

    void Render()
    {
        MTL::RenderPassDescriptor* descriptor = s_View->currentRenderPassDescriptor();
        descriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);

        MTL::CommandBuffer* commandBuffer = s_RenderQueue->commandBuffer();
        MTL::RenderCommandEncoder* commandEncoder = commandBuffer->renderCommandEncoder(descriptor);

        ImGuiWrapperCommon::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, commandEncoder);

        commandEncoder->endEncoding();
        commandBuffer->commit();
    }

    void ProcessMessage(void* data) {}
}

#else

namespace ImGuiWrapper
{
    void Init(const std::function<void(void*)>& fillImGuiData) {}
    void Shutdown() {}
    void NewFrame() {}
    void Render() {}
    void ProcessMessage(void* data) {}
}

#endif