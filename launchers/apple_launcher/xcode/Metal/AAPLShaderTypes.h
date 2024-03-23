/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header containing types and enumeration constants shared between Metal shaders and C/Objective-C source.
*/
#ifndef ShaderTypes_h
#define ShaderTypes_h

#include "AAPLCommonDefinitions.h"
#include <simd/simd.h>

// Structure shared between shader and C code to ensure the layout of uniform data accessed in
// Metal shaders matches the layout of the frame data set in C code.
typedef struct
{
    // Per-mesh uniforms.
    matrix_float3x3 templeNormalMatrix;

    // Per-light properties.
    vector_float3 ambientLightColor;
    vector_float3 directionalLightInvDirection;
    vector_float3 directionalLightColor;

} AAPLFrameData;

#endif
