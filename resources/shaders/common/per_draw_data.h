#ifndef RENDER_ENGINE_PER_DRAW_DATA
#define RENDER_ENGINE_PER_DRAW_DATA

struct PerDrawDataStruct
{
    float4x4 _ModelMatrix;
    float4x4 _ModelNormalMatrix;
};

#ifdef _INSTANCING

    struct PerInstanceIndexStruct
    {
        uint Index;
    };

    static uint _InstanceID;

    StructuredBuffer<PerDrawDataStruct> InstanceMatricesBuffer;
    StructuredBuffer<PerInstanceIndexStruct> PerInstanceIndices;

    #define _ModelMatrix            InstanceMatricesBuffer[_InstanceID]._ModelMatrix
    #define _ModelNormalMatrix      InstanceMatricesBuffer[_InstanceID]._ModelNormalMatrix

    #define DECLARE_INSTANCE_ID_ATTRIBUTE() uint InstanceID : SV_InstanceID;
    #define DECLARE_INSTANCE_ID_VARYING(varID) nointerpolation uint InstanceID : TEXCOORD##varID;

    #define SETUP_INSTANCE_ID(input) _InstanceID = input.InstanceID;
    #define TRANSFER_INSTANCE_ID_VARYING(output) output.InstanceID = _InstanceID;

    #define GET_PER_INSTANCE_VALUE(var) PerInstanceData[PerInstanceIndices[_InstanceID].Index].var
    #define PerInstanceDataBuffer(structType) StructuredBuffer<structType> PerInstanceData

#else

    ConstantBuffer<PerDrawDataStruct> PerDrawData;

    #define _ModelMatrix            PerDrawData._ModelMatrix
    #define _ModelNormalMatrix      PerDrawData._ModelNormalMatrix

    #define DECLARE_INSTANCE_ID_ATTRIBUTE()
    #define DECLARE_INSTANCE_ID_VARYING(varID)

    #define SETUP_INSTANCE_ID(input)
    #define TRANSFER_INSTANCE_ID_VARYING(output)

    #define GET_PER_INSTANCE_VALUE(var) PerInstanceData.var
    #define PerInstanceDataBuffer(structType) ConstantBuffer<structType> PerInstanceData

#endif

#endif