#import "AAPLOpenGLRenderer.h"
#import <Foundation/Foundation.h>
#import "EngineFrameworkWrapper.h"
#import "ImGuiWrapper.h"

@implementation AAPLOpenGLRenderer
{
    GLuint _defaultFBOName;
    CGSize _viewSize;
    PlatformViewBase* _view;
}

- (instancetype)initWithView:(PlatformViewBase*)view defaultFBOName:(GLuint)defaultFBOName
{
    self = [super init];
    if(self)
    {
        NSLog(@"%s %s", glGetString(GL_RENDERER), glGetString(GL_VERSION));

        _view = view;
        _defaultFBOName = defaultFBOName;
        
        [EngineFrameworkWrapper Initialize:(void*)nil graphicsBackend:@"OpenGL"];
        [ImGuiWrapper Init_OSX:_view];
        [ImGuiWrapper Init_OpenGL];
    }

    return self;
}

- (void)resize:(CGSize)size
{
    _viewSize = size;
}

- (void)draw
{
//    glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBOName);
    
    if ([EngineFrameworkWrapper ShouldCloseWindow])
    {
        [[_view window] performClose:self];
        return;
    }
    
    [ImGuiWrapper NewFrame_OpenGL];
    [ImGuiWrapper NewFrame_OSX:_view];
    
    [EngineFrameworkWrapper TickMainLoop:nil backbufferDescriptor:nil width:_viewSize.width height:_viewSize.height];
    
    [ImGuiWrapper Render_OpenGL];
}

- (void) dealloc
{
    [ImGuiWrapper Shutdown_OpenGL];
    [ImGuiWrapper Shutdown_OSX];
    [EngineFrameworkWrapper Shutdown];
}

@end
