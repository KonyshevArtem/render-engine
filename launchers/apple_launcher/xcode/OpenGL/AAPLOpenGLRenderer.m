#import "AAPLOpenGLRenderer.h"
#import <Foundation/Foundation.h>
#import "EngineFrameworkWrapper.h"

@implementation AAPLOpenGLRenderer
{
    GLuint _defaultFBOName;
    CGSize _viewSize;
}

- (instancetype)initWithDefaultFBOName:(GLuint)defaultFBOName
{
    self = [super init];
    if(self)
    {
        NSLog(@"%s %s", glGetString(GL_RENDERER), glGetString(GL_VERSION));

        _defaultFBOName = defaultFBOName;
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
    
    [EngineFrameworkWrapper TickMainLoop:_viewSize.width height:_viewSize.height];
}

@end
