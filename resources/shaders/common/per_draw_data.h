#ifndef RENDER_ENGINE_PER_DRAW_DATA
#define RENDER_ENGINE_PER_DRAW_DATA

struct PerDrawDataStruct
{
    float4x4 _ModelMatrix;
    float4x4 _ModelNormalMatrix;
};

#ifdef _INSTANCING

    StructuredBuffer<PerDrawDataStruct> InstanceMatricesBuffer;

    #define _ModelMatrix            InstanceMatricesBuffer[_InstanceID]._ModelMatrix
    #define _ModelNormalMatrix      InstanceMatricesBuffer[_InstanceID]._ModelNormalMatrix

    #define SETUP_INSTANCE_ID(instanceID) uint _InstanceID = instanceID;

#else

    ConstantBuffer<PerDrawDataStruct> PerDrawData;

    #define _ModelMatrix            PerDrawData._ModelMatrix
    #define _ModelNormalMatrix      PerDrawData._ModelNormalMatrix

    #define SETUP_INSTANCE_ID(instanceID)

#endif

#endif