/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of the renderer class that performs Metal setup and per-frame rendering.
*/
@import simd;
@import ModelIO;
@import MetalKit;

#import "AAPLMetalRenderer.h"
#import "AAPLMeshData.h"
#import "AAPLMathUtilities.h"
#import "AAPLShaderTypes.h"

static const MTLPixelFormat AAPLDepthFormat = MTLPixelFormatDepth32Float;
static const MTLPixelFormat AAPLColorFormat = MTLPixelFormatBGRA8Unorm_sRGB;

// The maximum number of command buffers in flight.
static const NSUInteger AAPLMaxBuffersInFlight = 3;

/// Main class that performs the rendering.
@implementation AAPLMetalRenderer
{
    dispatch_semaphore_t _inFlightSemaphore;
    id<MTLDevice>        _device;
    id<MTLCommandQueue>  _commandQueue;

    id<MTLDepthStencilState>   _depthState;

    // Metal objects you use to render the temple mesh.
    id<MTLRenderPipelineState>     _templeRenderPipeline;
    id<MTLBuffer>                  _templeVertexPositions;
    id<MTLBuffer>                  _templeVertexGenerics;

    // Arrays of submesh index buffers and textures for the temple mesh.
    NSMutableArray<id<MTLBuffer>>  *_templeIndexBuffers;
    NSMutableArray<id<MTLTexture>> *_templeTextures;

#if RENDER_REFLECTION
    // Metal objects used to render the reflective quad.
    MTLRenderPassDescriptor   *_reflectionRenderPassDescriptor;
    id<MTLRenderPipelineState> _quadRenderPipeline;
    id<MTLTexture>             _reflectionColorTexture;
    id<MTLTexture>             _reflectionDepthTexture;
    id<MTLBuffer>              _reflectionQuadBuffer;
    matrix_float4x4            _reflectionQuadMVPMatrix;
#endif

    // Collection of Metal buffers you use to set the shader data that changes each frame.
    id<MTLBuffer> _dynamicDataBuffers[AAPLMaxBuffersInFlight];

    // Buffer index tracking which of the dynamic data buffers you update for the current frame.
    uint8_t _currentBufferIndex;

    matrix_float4x4 _projectionMatrix;
    matrix_float4x4 _templeCameraMVPMatrix;
    matrix_float4x4 _templeReflectionMVPMatrix;

    // Current rotation value of the temple, in radians.
    float _rotation;
}

/// Initialize the renderer with the MetalKit view that references the Metal device you render with.
/// You also use the MetalKit view to set the pixel format and other properties of the drawable.
- (nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)mtkView
{
    self = [super init];
    if(self)
    {
        _device = mtkView.device;
        _inFlightSemaphore = dispatch_semaphore_create(AAPLMaxBuffersInFlight);
        _commandQueue = [_device newCommandQueue];

        mtkView.colorPixelFormat        = AAPLColorFormat;
        mtkView.depthStencilPixelFormat = AAPLDepthFormat;
        {
            // Configure a combined depth and stencil descriptor that enables the creation
            // of an immutable depth and stencil state object.
            MTLDepthStencilDescriptor *depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
            depthStencilDesc.depthCompareFunction = MTLCompareFunctionLess;
            depthStencilDesc.depthWriteEnabled = YES;
            _depthState = [_device newDepthStencilStateWithDescriptor:depthStencilDesc];
        }

        // Create three buffers to contain the data you pass to the GPU, which changes each frame.
        // Unlike OpenGL, in Metal you explicitly manage app synchronization by cycling through buffers
        // so you can write to one buffer with the CPU while you read from another buffer with the GPU.
        // Although the Metal implementation may seem more complex than OpenGL, in which the driver manages
        // the synchronization on your behalf, an OpenGL implementation may lead to unpredictable performance
        // due to the use of inconsistent heuristics that determine how synchronization should be managed.
        for(NSUInteger i = 0; i < AAPLMaxBuffersInFlight; i++)
        {
            // Set shared storage so that both the CPU and the GPU can access the buffers.
            const MTLResourceOptions storageMode = MTLResourceStorageModeShared;

            _dynamicDataBuffers[i] = [_device newBufferWithLength:sizeof(AAPLFrameData)
                                                          options:storageMode];

            _dynamicDataBuffers[i].label = [NSString stringWithFormat:@"PerFrameDataBuffer%lu", i];
        }

        [self buildTempleObjects];

        [self buildReflectiveQuadObjects];
    }

    return self;
}

/// Create and load assets, including meshes and textures, into Metal objects.
- (BOOL) buildTempleObjects
{
    NSError *error;

    NSURL *modelFileURL = [[NSBundle mainBundle] URLForResource:@"Meshes/Temple.obj"
                                                  withExtension:nil];

    NSAssert(modelFileURL, @"Could not find model (%@) file in the bundle.", modelFileURL.absoluteString);

    // Load mesh data from a file into memory.
    // This method only loads mesh data and does not create Metal objects.

    AAPLMeshData *meshData = [[AAPLMeshData alloc] initWithURL:modelFileURL error:&error];

    NSAssert(meshData, @"Could not load mesh from model file (%@), error: %@.", modelFileURL.absoluteString, error);

    // Extract the vertex data, reconfigure the layout for the vertex shader, and place the data into
    // a Metal buffer.
    {
        // In Metal, there's no equivalent to a vertex array object. Instead, you define the layout of the
        // vertices with a render pipeline state object.
        // In this case, you create a vertex descriptor that defines the vertex layout which you set in
        // the render pipeline state object for the temple model. See `mtlVertexDescriptor` below for more
        // information.

        // Create Metal buffers to store the vertex data (i.e. positions, texture coordinates, and normals).
        NSUInteger positionElementSize = sizeof(vector_float3);
        NSUInteger positionDataSize    = positionElementSize * meshData.vertexCount;

        NSUInteger genericElementSize = sizeof(AAPLVertexGenericData);
        NSUInteger genericsDataSize   = genericElementSize * meshData.vertexCount;

        _templeVertexPositions = [_device newBufferWithLength:positionDataSize
                                                      options:MTLResourceStorageModeShared];

        _templeVertexGenerics = [_device newBufferWithLength:genericsDataSize
                                                     options:MTLResourceStorageModeShared];

        vector_float3 *positionsArray = (vector_float3 *)_templeVertexPositions.contents;
        AAPLVertexGenericData *genericsArray = (AAPLVertexGenericData *)_templeVertexGenerics.contents;

        // Load mesh vertex data into Metal buffers.
        struct AAPLVertexData *vertexData = meshData.vertexData;

        for(unsigned long vertex = 0; vertex < meshData.vertexCount; vertex++)
        {
            positionsArray[vertex] = vertexData[vertex].position;
            genericsArray[vertex].texcoord = vertexData[vertex].texcoord;
            genericsArray[vertex].normal.x = vertexData[vertex].normal.x;
            genericsArray[vertex].normal.y = vertexData[vertex].normal.y;
            genericsArray[vertex].normal.z = vertexData[vertex].normal.z;
        }
    }

    // Load submesh data into index buffers and textures.
    {
        // Create a texture loader to load textures from images.
        MTKTextureLoader *textureLoader = [[MTKTextureLoader alloc] initWithDevice:_device];

        // Indicate to MetalKit that the origin of the loaded image is the bottom-left corner. Metal textures
        // define their origin as the top-left corner, so the texture loader flips the image when it performs
        // the load operation.
        NSDictionary *loaderOptions =
        @{
          MTKTextureLoaderOptionGenerateMipmaps : @YES,
          MTKTextureLoaderOptionOrigin : MTKTextureLoaderOriginBottomLeft,
          };

        _templeIndexBuffers = [[NSMutableArray alloc] init];
        _templeTextures = [[NSMutableArray alloc] init];

        for(NSUInteger index = 0; index < meshData.submeshes.allValues.count; index++)
        {
            AAPLSubmeshData *submeshData = meshData.submeshes.allValues[index];

            NSUInteger indexBufferSize = sizeof(uint32_t) * submeshData.indexCount;

            _templeIndexBuffers[index] = [_device newBufferWithBytes:submeshData.indexData
                                                              length:indexBufferSize
                                                             options:MTLResourceStorageModeShared];

            _templeTextures[index] = [textureLoader newTextureWithContentsOfURL:submeshData.baseColorMapURL
                                                                        options:loaderOptions
                                                                          error:&error];

            NSAssert(_templeTextures[index], @"Could not load image (%@) into Metal texture: %@",
                     submeshData.baseColorMapURL.absoluteString, error);
        }
    }

    // Build the render pipeline state object to render the temple.
    {
        // Create a vertex descriptor for the Metal pipeline, which specifies the vertex layout that the
        // pipeline expects. The layout below defines attributes you use to calculate vertex shader
        // output positions (e.g. world positions, skinning, tweening weights) separate from other attributes
        // (e.g. texture coordinates, normals). This generally maximizes pipeline efficiency.

        // In OpenGL, you layout vertex data with a vertex array object. OpenGL links each vertex array
        // object with a buffer containing the vertex data. However, Metal links the vertex layout with
        // the pipeline state object, not the buffers.
        // This approach allows Metal to optimize the vertex shader when your app builds the render
        // pipeline. OpenGL must defer such optimizations until draw time because it can't know in advance
        // which programs you'll be use with which vertex array objects.

        // Create a Metal vertex descriptor that specifies how vertices are laid out for input into the render
        // pipeline and how Model I/O must condition the vertex data.
        MTLVertexDescriptor *mtlVertexDescriptor = [[MTLVertexDescriptor alloc] init];

        // Positions.
        mtlVertexDescriptor.attributes[AAPLVertexAttributePosition].format = MTLVertexFormatFloat3;
        mtlVertexDescriptor.attributes[AAPLVertexAttributePosition].offset = 0;
        mtlVertexDescriptor.attributes[AAPLVertexAttributePosition].bufferIndex = AAPLBufferIndexMeshPositions;

        // Texture coordinates.
        mtlVertexDescriptor.attributes[AAPLVertexAttributeTexcoord].format = MTLVertexFormatFloat2;
        mtlVertexDescriptor.attributes[AAPLVertexAttributeTexcoord].offset = 0;
        mtlVertexDescriptor.attributes[AAPLVertexAttributeTexcoord].bufferIndex = AAPLBufferIndexMeshGenerics;

        // Normals.
        mtlVertexDescriptor.attributes[AAPLVertexAttributeNormal].format = MTLVertexFormatFloat3;
        mtlVertexDescriptor.attributes[AAPLVertexAttributeNormal].offset = 8;
        mtlVertexDescriptor.attributes[AAPLVertexAttributeNormal].bufferIndex = AAPLBufferIndexMeshGenerics;

        // Position buffer layout.
        mtlVertexDescriptor.layouts[AAPLBufferIndexMeshPositions].stride = 16;
        mtlVertexDescriptor.layouts[AAPLBufferIndexMeshPositions].stepRate = 1;
        mtlVertexDescriptor.layouts[AAPLBufferIndexMeshPositions].stepFunction = MTLVertexStepFunctionPerVertex;

        // Generic attribute buffer layout.
        mtlVertexDescriptor.layouts[AAPLBufferIndexMeshGenerics].stride = 20;
        mtlVertexDescriptor.layouts[AAPLBufferIndexMeshGenerics].stepRate = 1;
        mtlVertexDescriptor.layouts[AAPLBufferIndexMeshGenerics].stepFunction = MTLVertexStepFunctionPerVertex;

        // Configure a pipeline descriptor that enables the creation of an immutable pipeline state object.
        MTLRenderPipelineDescriptor *pipelineStateDescriptor = [MTLRenderPipelineDescriptor new];
        pipelineStateDescriptor.label                        = @"Temple Pipeline";

        // Load the library of precompiled shaders.
        id<MTLLibrary> defaultLibrary = [_device newDefaultLibrary];

        // Load the precompiled vertex and fragment shaders from the library.
        id<MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:@"templeTransformAndLightingShader"];
        id<MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:@"templeSamplingFragmentShader"];

        // Set the vertex input descriptor, vertex shader, and fragment shader for this pipeline object.
        pipelineStateDescriptor.vertexDescriptor = mtlVertexDescriptor;
        pipelineStateDescriptor.vertexFunction = vertexFunction;
        pipelineStateDescriptor.fragmentFunction = fragmentFunction;

        // Set the render target formats that this pipeline renders to. Unlike OpenGL program objects,
        // where any program can render to any framebuffer object, Metal pipeline objects can render only
        // to the set of render target using the pixel formats that they're built for. By linking the
        // formats to the pipeline, Metal can optimize the fragment shader for those specific formats when
        // the app creates the pipeline object.
        pipelineStateDescriptor.colorAttachments[0].pixelFormat = AAPLColorFormat;
        pipelineStateDescriptor.depthAttachmentPixelFormat      = AAPLDepthFormat;

        // Use the settings in `pipelineStateDescriptor` to create the immutable pipeline state.
        _templeRenderPipeline = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                        error:&error];

        NSAssert(_templeRenderPipeline, @"Failed to create temple render pipeline state, error: %@.", error);
    }

    return YES;
}

- (void) buildReflectiveQuadObjects
{
#if RENDER_REFLECTION
    NSError *error;
    // Setup the buffer for the reflective quad.
    {
        // In Metal, there's no equivalent to a vertex array object. Instead, you define the layout of the
        // vertices with a render pipeline state object.
        // In this case, you use the `AAPLQuadVertex` structure as an argument for the vertex shader of
        // `_quadRenderPipeline`. This structure defines the layout of vertices in memory, just like any other
        // C structure.

        static const AAPLQuadVertex AAPLQuadVertices[] =
        {
            // You flip the y texture coordinate values because OpenGL defines a bottom-left texture origin,
            // whereas Metal defines a top-left texture origin.
            // For reflections of more complex models, you could instead invert the y texture coordinate within
            // the shader that samples from the reflection texture.
            { { -500, -500, 0.0, 1.0}, {1.0, 1.0} },
            { { -500,  500, 0.0, 1.0}, {1.0, 0.0} },
            { {  500,  500, 0.0, 1.0}, {0.0, 0.0} },

            { { -500, -500, 0.0, 1.0}, {1.0, 1.0} },
            { {  500,  500, 0.0, 1.0}, {0.0, 0.0} },
            { {  500, -500, 0.0, 1.0}, {0.0, 1.0} },
        };

        _reflectionQuadBuffer = [_device newBufferWithBytes:&AAPLQuadVertices
                                                     length:sizeof(AAPLQuadVertices)
                                                    options:MTLResourceStorageModeShared];
    }

    // Create texture objects and a render pass descriptor to render and display the reflection.
    {
        MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor new];
        textureDescriptor.width = AAPLReflectionSize.x;
        textureDescriptor.height = AAPLReflectionSize.y;
        textureDescriptor.mipmapLevelCount = 9;
        textureDescriptor.storageMode = MTLStorageModePrivate;
        textureDescriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
        textureDescriptor.pixelFormat = AAPLColorFormat;

        _reflectionColorTexture = [_device newTextureWithDescriptor:textureDescriptor];

        textureDescriptor.mipmapLevelCount = 1;
        textureDescriptor.pixelFormat = AAPLDepthFormat;

        _reflectionDepthTexture = [_device newTextureWithDescriptor:textureDescriptor];

        _reflectionRenderPassDescriptor = [MTLRenderPassDescriptor new];

        // Configure the render pass to clear the color texture when the pass begins (`MTLLoadActionClear`)
        // and store the results of rendering to the texture when the pass ends (`MTLStoreActionStore`).
        _reflectionRenderPassDescriptor.colorAttachments[0].texture = _reflectionColorTexture;
        _reflectionRenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        _reflectionRenderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

        // Configure the render pass to clear the depth texture when the pass begins (`MTLLoadActionClear`)
        // and discard the results of rendering to the texture when the pass ends (`MTLStoreActionDontCare`).
        _reflectionRenderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
        _reflectionRenderPassDescriptor.depthAttachment.storeAction = MTLStoreActionDontCare;
        _reflectionRenderPassDescriptor.depthAttachment.texture = _reflectionDepthTexture;
    }

    {
        // Configure a pipeline descriptor that enables the creation of an immutable pipeline state object.
        MTLRenderPipelineDescriptor *pipelineStateDescriptor = [MTLRenderPipelineDescriptor new];
        pipelineStateDescriptor.label = @"Quad Pipeline";

        // In this pipeline, you don't use a vertex descriptor to setup the vertex inputs. Instead, you
        // specify the vertex buffer layout with a structure. A pointer to this structure is the type you
        // use to define one of the vertex shader's arguments. The vertex shader indexes into the pointer
        // array to retrieve the vertex data, rather than having Metal feed each vertex into the shader, which
        // is what Metal does when a render pipeline is configured with a vertex descriptor.
        pipelineStateDescriptor.vertexDescriptor = nil;

        // Load the Metal shader library to access the vertex and fragment shaders.
        id<MTLLibrary> defaultLibrary = [_device newDefaultLibrary];

        // Set the vertex and fragment shaders.
        pipelineStateDescriptor.vertexFunction =
        [defaultLibrary newFunctionWithName:@"reflectionQuadTransformShader"];

        pipelineStateDescriptor.fragmentFunction =
        [defaultLibrary newFunctionWithName:@"reflectionQuadFragmentShader"];

        // Set the render target formats that this pipeline renders to. Unlike OpenGL program objects,
        // where any program can render to any framebuffer object, Metal pipeline objects can render only
        // to the set of render target using the pixel formats that they're built for. By linking the
        // formats to the pipeline, Metal can optimize the fragment shader for those specific formats when
        // the app creates the pipeline object.
        pipelineStateDescriptor.colorAttachments[0].pixelFormat = AAPLColorFormat;
        pipelineStateDescriptor.depthAttachmentPixelFormat      = AAPLDepthFormat;

        // Use the settings in `pipelineStateDescriptor` to create the immutable pipeline state.
        _quadRenderPipeline = [_device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                      error:&error];

        NSAssert(_quadRenderPipeline, @"Failed to create quad render pipeline state: %@", error);
    }
#endif
}


/// Update any game state, including dynamic updates to the changing Metal buffer.
- (void) updateFrameState
{
    const vector_float3 ambientLightColor = {0.02, 0.02, 0.02};
    const vector_float3 directionalLightDirection = vector_normalize ((vector_float3){0.0, 0.0, 1.0});
    const vector_float3 directionalLightInvDirection = -directionalLightDirection;
    const vector_float3 directionalLightColor = {.7, .7, .7};

    const vector_float3   cameraPosition = {0.0, 0.0, -1000.0};
    const matrix_float4x4 cameraViewMatrix  = matrix4x4_translation(-cameraPosition);

    const vector_float3   templeRotationAxis      = {0, 1, 0};
    const matrix_float4x4 templeRotationMatrix    = matrix4x4_rotation (_rotation, templeRotationAxis);
    const matrix_float4x4 templeTranslationMatrix = matrix4x4_translation(0.0, -400, 0);
    const matrix_float4x4 templeModelMatrix       = matrix_multiply(templeRotationMatrix, templeTranslationMatrix);
    const matrix_float4x4 templeModelViewMatrix   = matrix_multiply (cameraViewMatrix, templeModelMatrix);
    const matrix_float3x3 templeNormalMatrix      = matrix3x3_upper_left(templeModelMatrix);


    _currentBufferIndex = (_currentBufferIndex + 1) % AAPLMaxBuffersInFlight;

    // Get a pointer to the buffer that contains the current frame data buffer contents, and modify the data.
    AAPLFrameData *frameData = (AAPLFrameData*)_dynamicDataBuffers[_currentBufferIndex].contents;
    frameData->templeNormalMatrix            = templeNormalMatrix;
    frameData->directionalLightColor         = directionalLightColor;;
    frameData->directionalLightInvDirection  = directionalLightInvDirection;
    frameData->ambientLightColor             = ambientLightColor;

    _templeCameraMVPMatrix     = matrix_multiply(_projectionMatrix, templeModelViewMatrix);

#if RENDER_REFLECTION
    const vector_float3  quadRotationAxis  = {1, 0, 0};
    const float          quadRotationAngle = 270 * M_PI/180;
    const vector_float3  quadTranslation   = {0, 300, 0};

    const matrix_float4x4 quadRotationMatrix            = matrix4x4_rotation(quadRotationAngle, quadRotationAxis);
    const matrix_float4x4 quadTranslationMatrtix        = matrix4x4_translation(quadTranslation);
    const matrix_float4x4 quadModelMatrix               = matrix_multiply(quadTranslationMatrtix, quadRotationMatrix);
    const matrix_float4x4 quadModeViewMatrix            = matrix_multiply(cameraViewMatrix, quadModelMatrix);

    const vector_float4 target = matrix_multiply(quadModelMatrix, (vector_float4){0, 0, 0, 1});
    const vector_float4 eye    = matrix_multiply(quadModelMatrix, (vector_float4){0.0, 0.0, 250, 1});
    const vector_float4 up     = matrix_multiply(quadModelMatrix, (vector_float4){0, 1, 0, 1});

    const matrix_float4x4 reflectionViewMatrix       = matrix_look_at_left_hand(eye.xyz, target.xyz, up.xyz);
    const matrix_float4x4 reflectionModelViewMatrix  = matrix_multiply(reflectionViewMatrix, templeModelMatrix);
    const matrix_float4x4 reflectionProjectionMatrix = matrix_perspective_left_hand(M_PI/2.0, 1, 0.1, 3000.0);

    _templeReflectionMVPMatrix = matrix_multiply(reflectionProjectionMatrix, reflectionModelViewMatrix);

    _reflectionQuadMVPMatrix   = matrix_multiply(_projectionMatrix, quadModeViewMatrix);
#endif

    _rotation += .01;
}

/// Called whenever the view orientation, layout, or size changes.
- (void) mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    // Handle the resize of the draw rectangle. In particular, update the perspective projection matrix
    // with a new aspect ratio because the view orientation, layout, or size has changed.
    // This sample uses `matrix_perspective_left_hand()` in the Metal renderer and
    // `matrix_perspective_left_hand_gl()` in the OpenGL renderer. This is necessary because Metal's
    // clip region, which the projection matrix renders to, is different than OpenGL. In Metal, the clip
    // space depth uses the range [0, 1] while OpenGL uses [-1, 1].
    float aspect = size.width / (float)size.height;
    _projectionMatrix = matrix_perspective_left_hand(65.0f * (M_PI / 180.0f), aspect, 1.0f, 5000.0);
}

/// Called whenever the view needs to render.
- (void) drawInMTKView:(nonnull MTKView *)view
{
    // Wait to ensure only a maximum of `AAPLMaxBuffersInFlight` frames are being processed by any
    // stage in the Metal pipeline (e.g. app, Metal, drivers, GPU, etc.) at any time. This mechanism
    // prevents the CPU from overwriting dynamic buffer data before the GPU has read it.
    dispatch_semaphore_wait(_inFlightSemaphore, DISPATCH_TIME_FOREVER);

    [self updateFrameState];
    id<MTLCommandBuffer> commandBuffer;

#if RENDER_REFLECTION

    commandBuffer = [_commandQueue commandBuffer];
    commandBuffer.label = @"Reflections Command Buffer";

    id<MTLRenderCommandEncoder> renderEncoder =
        [commandBuffer renderCommandEncoderWithDescriptor:_reflectionRenderPassDescriptor];
    renderEncoder.label = @"Reflection Render Encoder";

    // Set the render pipeline state.
    //[renderEncoder setCullMode:MTLCullModeBack];
    [renderEncoder setRenderPipelineState:_templeRenderPipeline];
    [renderEncoder setDepthStencilState:_depthState];

    // Set any buffers fed into the render pipeline when a draw executes.
    [renderEncoder setVertexBuffer:_dynamicDataBuffers[_currentBufferIndex]
                            offset:0
                           atIndex:AAPLBufferIndexUniforms];

    [renderEncoder setVertexBuffer:_templeVertexPositions
                            offset:0
                           atIndex:AAPLBufferIndexMeshPositions];

    [renderEncoder setVertexBuffer:_templeVertexGenerics
                            offset:0
                           atIndex:AAPLBufferIndexMeshGenerics];

    [renderEncoder setVertexBytes:&_templeReflectionMVPMatrix
                           length:sizeof(_templeReflectionMVPMatrix)
                          atIndex:AAPLBufferIndexMVPMatrix];

    for(NSUInteger index = 0; index < _templeIndexBuffers.count; index++)
    {
        // Set any textures read or sampled from the render pipeline.
        [renderEncoder setFragmentTexture:_templeTextures[index]
                                  atIndex:AAPLTextureIndexBaseColor];

        NSUInteger indexCount = _templeIndexBuffers[index].length / sizeof(uint32_t);

        // Draw the submesh.
        [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:indexCount
                                   indexType:MTLIndexTypeUInt32
                                 indexBuffer:_templeIndexBuffers[index]
                           indexBufferOffset:0];
    }

    [renderEncoder endEncoding];

    [commandBuffer commit];

#endif

    // Create a new command buffer to render to the drawable.
    commandBuffer = [_commandQueue commandBuffer];
    commandBuffer.label = @"Drawable Command Buffer";

    // Add a completion hander that signals `_inFlightSemaphore` when Metal and the GPU have fully
    // finished processing the commands encoded this frame. This indicates that the dynamic bufers,
    // written to this frame, are no longer be needed by Metal or the GPU, meaning that you can
    // change the buffer contents without corrupting any rendering.
    __block dispatch_semaphore_t block_sema = _inFlightSemaphore;
    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
    {
        dispatch_semaphore_signal(block_sema);
    }];

    // Obtain a render pass descriptor generated from the view's drawable textures.
    MTLRenderPassDescriptor *drawableRenderPassDescriptor = view.currentRenderPassDescriptor;

    // If you obtained a valid render pass descriptor, render to the drawable. Otherwise, skip
    // any rendering for this frame because there is no drawable to draw to.
    if(drawableRenderPassDescriptor != nil)
    {
        id<MTLRenderCommandEncoder> renderEncoder =
            [commandBuffer renderCommandEncoderWithDescriptor:drawableRenderPassDescriptor];
        renderEncoder.label = @"Drawable Render Encoder";

        // Set the render pipeline state.
        //  [renderEncoder setCullMode:MTLCullModeBack];
        [renderEncoder setRenderPipelineState:_templeRenderPipeline];
        [renderEncoder setDepthStencilState:_depthState];

        // Set any buffers fed into the render pipeline when a draw executes.
        [renderEncoder setVertexBuffer:_dynamicDataBuffers[_currentBufferIndex]
                                offset:0
                               atIndex:AAPLBufferIndexUniforms];

        [renderEncoder setFragmentBuffer:_dynamicDataBuffers[_currentBufferIndex]
                                  offset:0
                                 atIndex:AAPLBufferIndexUniforms];

        [renderEncoder setVertexBuffer:_templeVertexPositions
                                offset:0
                               atIndex:AAPLBufferIndexMeshPositions];

        [renderEncoder setVertexBuffer:_templeVertexGenerics
                                offset:0
                               atIndex:AAPLBufferIndexMeshGenerics];

        [renderEncoder setVertexBytes:&_templeCameraMVPMatrix
                               length:sizeof(_templeCameraMVPMatrix)
                              atIndex:AAPLBufferIndexMVPMatrix];

        for(NSUInteger index = 0; index < _templeIndexBuffers.count; index++)
        {
            // Set any textures read or sampled from the render pipeline.
            [renderEncoder setFragmentTexture:_templeTextures[index]
                                      atIndex:AAPLTextureIndexBaseColor];

            NSUInteger indexCount = _templeIndexBuffers[index].length / sizeof(uint32_t);

            // Draw the submesh.
            [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                      indexCount:indexCount
                                       indexType:MTLIndexTypeUInt32
                                     indexBuffer:_templeIndexBuffers[index]
                               indexBufferOffset:0];
        }

#if RENDER_REFLECTION
        // Set the state for the reflective quad and draw it.
        [renderEncoder setRenderPipelineState:_quadRenderPipeline];

        [renderEncoder setVertexBytes:&_reflectionQuadMVPMatrix
                               length:sizeof(_reflectionQuadMVPMatrix)
                              atIndex:AAPLBufferIndexMVPMatrix];

        [renderEncoder setVertexBuffer:_reflectionQuadBuffer
                                offset:0
                               atIndex:AAPLBufferIndexMeshPositions];

        [renderEncoder setFragmentTexture:_reflectionColorTexture
                                  atIndex:AAPLTextureIndexBaseColor];

        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:0
                          vertexCount:6];
#endif

        // End encoding commands.
        [renderEncoder endEncoding];

        // Schedule a drawable presentation to occur after the framebuffer is complete.
        [commandBuffer presentDrawable:view.currentDrawable];
    }

    // Finalize rendering and submit the command buffer to the GPU.
    [commandBuffer commit];
}

@end
