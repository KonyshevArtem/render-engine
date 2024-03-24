#import "ImGuiWrapper.h"
#import "ImGuiWrapperApple.h"

@implementation ImGuiWrapper : NSObject

+ (bool) Init_OpenGL
{
    return ImGui_OpenGL3_Init();
}

+ (void) Shutdown_OpenGL
{
    ImGui_OpenGL3_Shutdown();
}

+ (void) NewFrame_OpenGL
{
    ImGui_OpenGL3_NewFrame();
}

+ (void) Render_OpenGL
{
    ImGui_OpenGL3_Render();
}

+ (bool) Init_Metal:(id<MTLDevice>)device
{
    return ImGui_Metal_Init(device);
}

+ (void) Shutdown_Metal
{
    ImGui_Metal_Shutdown();
}

+ (void) NewFrame_Metal:(MTLRenderPassDescriptor *)renderPassDescriptor
{
    ImGui_Metal_NewFrame(renderPassDescriptor);
}

+ (void) Render_Metal:(id<MTLCommandBuffer>)commandBuffer commandEncoder:(id<MTLRenderCommandEncoder>)commandEncoder
{
    ImGui_Metal_Render(commandBuffer, commandEncoder);
}

+ (bool) Init_OSX:(NSView *)view
{
    return ImGui_OSX_Init(view);
}

+ (void) Shutdown_OSX
{
    ImGui_OSX_Shutdown();
}

+ (void) NewFrame_OSX:(NSView *)view
{
    ImGui_OSX_NewFrame(view);
}

@end
