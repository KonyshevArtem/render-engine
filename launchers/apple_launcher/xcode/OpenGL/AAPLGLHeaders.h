#ifndef AAPLGLHeaders_h
#define AAPLGLHeaders_h


#if TARGET_MACOS

#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/gl3.h>

#else // if (TARGET_IOS || TARGET_TVOS)

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/ES3/gl.h>

#endif // !(TARGET_IOS || TARGET_TVOS)

#endif
