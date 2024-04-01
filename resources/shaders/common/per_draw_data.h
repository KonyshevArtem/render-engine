#ifndef RENDER_ENGINE_PER_DRAW_DATA
#define RENDER_ENGINE_PER_DRAW_DATA

#include "graphics_backend_macros.h"

#define BEGIN_PER_INSTANCE_DATA struct PerInstanceDataStruct {

#ifdef _INSTANCING
    int _InstanceID;

    #if SUPPORT_SSBO
        #define END_PER_INSTANCE_DATA   }; layout(std140) readonly buffer PerInstanceData { PerInstanceDataStruct PerInstance[]; };
    #else
        #define END_PER_INSTANCE_DATA   }; layout(std140) uniform PerInstanceData { PerInstanceDataStruct PerInstance[MAX_INSTANCING_COUNT]; };
    #endif

    #define GET_PER_INSTANCE_VALUE(name)        PerInstance[_InstanceID].name
    #define DECLARE_INSTANCE_ID_VARYING         flat int instanceID;
    #define TRANSFER_INSTANCE_ID_VARYING(vars)  vars.instanceID = gl_InstanceID;

    #if defined(VERTEX_PROGRAM)
        #define SETUP_INSTANCE_ID                   _InstanceID = gl_InstanceID;
    #elif defined(FRAGMENT_PROGRAM)
        #define SETUP_INSTANCE_ID(vars)             _InstanceID = vars.instanceID;
    #endif
#else
    #define END_PER_INSTANCE_DATA               }; layout(std140) uniform PerInstanceData { PerInstanceDataStruct PerInstance[1]; };
    #define GET_PER_INSTANCE_VALUE(name)        PerInstance[0].name
    #define DECLARE_INSTANCE_ID_VARYING
    #define TRANSFER_INSTANCE_ID_VARYING(vars)

    #if defined(VERTEX_PROGRAM)
        #define SETUP_INSTANCE_ID
    #elif defined(FRAGMENT_PROGRAM)
        #define SETUP_INSTANCE_ID(vars)
    #endif
#endif

#ifdef _INSTANCING
    #if SUPPORT_SSBO
        layout(std140) readonly buffer InstanceMatricesBuffer
        {
            mat4 InstanceMatrices[];
        };

        #define _ModelMatrix        InstanceMatrices[_InstanceID * 2 + 0]
        #define _ModelNormalMatrix  InstanceMatrices[_InstanceID * 2 + 1]
    #else
        layout(location = 4) in mat4 _ModelMatrix;
        layout(location = 8) in mat4 _ModelNormalMatrix;
    #endif
#else
    struct PerDrawDataStruct
    {
        float4x4 _ModelMatrix;
        float4x4 _ModelNormalMatrix;
    };
#endif

#endif