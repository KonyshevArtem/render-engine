#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION

#include "game_window_metal.h"

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>
#include <utility>

class RenderEngineViewDelegate : public MTK::ViewDelegate
{
public:
    explicit RenderEngineViewDelegate(MTL::Device *device);
    ~RenderEngineViewDelegate() override;

    void drawInMTKView(MTK::View *view) override;

private:
    MTL::Device *m_Device;
    MTL::CommandQueue *m_CommandQueue;
};

class RenderEngineAppDelegate : public NS::ApplicationDelegate
{
public:
    RenderEngineAppDelegate(std::string title, float width, float height);
    ~RenderEngineAppDelegate() override;

    void applicationWillFinishLaunching(NS::Notification *notification) override;
    void applicationDidFinishLaunching(NS::Notification *notification) override;
    bool applicationShouldTerminateAfterLastWindowClosed(NS::Application *sender) override;

private:
    std::string m_WindowTitle;
    float m_Width;
    float m_Height;

    NS::Window* m_Window = nullptr;
    MTK::View* m_View = nullptr;
    MTL::Device* m_Device = nullptr;
    RenderEngineViewDelegate* m_ViewDelegate = nullptr;
};

GameWindowMetal::GameWindowMetal(int width, int height,
                                 RenderHandler renderHandler,
                                 KeyboardInputHandlerDelegate keyboardInputHandler,
                                 MouseMoveHandlerDelegate mouseMoveHandler)
        : GameWindow(std::move(renderHandler), std::move(keyboardInputHandler), std::move(mouseMoveHandler))
{
    m_AutoreleasePool = NS::AutoreleasePool::alloc()->init();
    m_AppDelegate = new RenderEngineAppDelegate(GetWindowTitle(), width, height);

    m_Application = NS::Application::sharedApplication();
    m_Application->setDelegate(dynamic_cast<NS::ApplicationDelegate*>(m_AppDelegate));
}

GameWindowMetal::~GameWindowMetal()
{
    m_AutoreleasePool->release();
    delete m_AppDelegate;
}

void GameWindowMetal::BeginMainLoop()
{
    m_Application->run();
}

void GameWindowMetal::SetCloseFlag()
{

}

/// Render Engine View Delegate

RenderEngineViewDelegate::RenderEngineViewDelegate(MTL::Device *device)
{
    m_Device = device->retain();
    m_CommandQueue = device->newCommandQueue();
}

RenderEngineViewDelegate::~RenderEngineViewDelegate()
{
    m_CommandQueue->release();
    m_Device->release();
}

void RenderEngineViewDelegate::drawInMTKView(MTK::View *view)
{
    auto* pool = NS::AutoreleasePool::alloc()->init();

    auto *cmd = m_CommandQueue->commandBuffer();
    auto *rpd = view->currentRenderPassDescriptor();
    auto *enc = cmd->renderCommandEncoder(rpd);
    enc->endEncoding();
    cmd->presentDrawable(view->currentDrawable());
    cmd->commit();

    pool->release();
}

/// Render Engine App Delegate

RenderEngineAppDelegate::RenderEngineAppDelegate(std::string title, float width, float height)
        : m_WindowTitle(std::move(title)), m_Width(width), m_Height(height)
{
}

RenderEngineAppDelegate::~RenderEngineAppDelegate()
{
    m_View->release();
    m_Window->release();
    m_Device->release();
    delete m_ViewDelegate;
}

void RenderEngineAppDelegate::applicationWillFinishLaunching(NS::Notification *notification)
{
    auto *app = reinterpret_cast<NS::Application*>(notification->object());
    app->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
}

void RenderEngineAppDelegate::applicationDidFinishLaunching(NS::Notification *notification)
{
    CGRect frame = {{0, 0},{m_Width, m_Height}};

    m_Window = NS::Window::alloc()->init(
            frame,
            NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
            NS::BackingStoreBuffered,
            false);

    m_Device = MTL::CreateSystemDefaultDevice();

    m_View = MTK::View::alloc()->init(frame, m_Device);
    m_View->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    m_View->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));

    m_ViewDelegate = new RenderEngineViewDelegate(m_Device);
    m_View->setDelegate(m_ViewDelegate);

    m_Window->setContentView(m_View);
    m_Window->setTitle(NS::String::string(m_WindowTitle.c_str(), NS::StringEncoding::UTF8StringEncoding));

    m_Window->makeKeyAndOrderFront(nullptr);

    auto *app = reinterpret_cast<NS::Application*>( notification->object());
    app->activateIgnoringOtherApps(true);
}

bool RenderEngineAppDelegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application *sender)
{
    return true;
}
