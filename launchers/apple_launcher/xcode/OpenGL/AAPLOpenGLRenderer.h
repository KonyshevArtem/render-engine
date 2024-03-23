#import <Foundation/Foundation.h>
#include "AAPLGLHeaders.h"

@interface AAPLOpenGLRenderer : NSObject

- (instancetype)initWithDefaultFBOName:(GLuint)defaultFBOName;

- (void)draw;

- (void)resize:(CGSize)size;

@end
