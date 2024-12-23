#include "imgui_wrapper.h"
#include "imgui_wrapper_common.h"

#ifdef ENABLE_IMGUI

#include "imgui_impl_metal.h"
#include "imgui_impl_osx.h"

namespace ImGuiWrapper
{
    static void* s_View;

    struct InitData
    {
        MTL::Device* Device;
        void* View;
    };

    struct NewFrameData
    {
        MTL::RenderPassDescriptor* RenderPassDescriptor;
        MTL::CommandBuffer* CommandBuffer;
    };

    void Init(void* data)
    {
        InitData* initData = static_cast<InitData*>(data);

        ImGuiWrapperCommon::Init();
        ImGui_ImplMetal_Init(initData->Device);
        ImGui_ImplOSX_Init(initData->View);

        s_View = initData->View;
    }

    void Shutdown()
    {
        ImGui_ImplMetal_Shutdown();
        ImGui_ImplOSX_Shutdown();
        ImGuiWrapperCommon::Shutdown();
    }

    void NewFrame(void* data)
    {
        NewFrameData* newFrameData = static_cast<NewFrameData*>(data);
        ImGui_ImplMetal_NewFrame(newFrameData->RenderPassDescriptor);
        ImGui_ImplOSX_NewFrame(s_View);
        ImGuiWrapperCommon::NewFrame();
    }

    void Render(void* data)
    {
        NewFrameData* newFrameData = static_cast<NewFrameData*>(data);

        newFrameData->RenderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);
        MTL::RenderCommandEncoder* commandEncoder = newFrameData->CommandBuffer->renderCommandEncoder(newFrameData->RenderPassDescriptor);

        ImGuiWrapperCommon::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), newFrameData->CommandBuffer, commandEncoder);

        commandEncoder->endEncoding();
    }
}

#else

namespace ImGuiWrapper
{
    void Init(void* data) {}
    void Shutdown() {}
    void NewFrame(void* data) {}
    void Render(void* data) {}
}

#endif