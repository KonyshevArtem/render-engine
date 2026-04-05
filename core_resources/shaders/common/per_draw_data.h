#ifndef RENDER_ENGINE_PER_DRAW_DATA_H
#define RENDER_ENGINE_PER_DRAW_DATA_H

#include "global_defines.h"

struct PerDrawDataStruct
{
    float4x4 _ModelMatrix;
    float4x4 _ModelNormalMatrix;
};

StructuredBuffer<PerDrawDataStruct> TransformMatricesBuffer : register(TRANSFORM_MATRICES_DATA);

#ifdef _INSTANCING

    static uint _InstanceID;

    Buffer<uint> InstanceMatricesEntriesBuffer : register(INSTANCING_MATRICES_ENTRIES_DATA);
    
    #define _ModelMatrix            TransformMatricesBuffer[InstanceMatricesEntriesBuffer[_InstanceID]]._ModelMatrix
    #define _ModelNormalMatrix      TransformMatricesBuffer[InstanceMatricesEntriesBuffer[_InstanceID]]._ModelNormalMatrix

    #define DECLARE_INSTANCE_ID_ATTRIBUTE() uint InstanceID : SV_InstanceID;
    #define DECLARE_INSTANCE_ID_VARYING(varID) nointerpolation uint InstanceID : TEXCOORD##varID;

    #define SETUP_INSTANCE_ID(input) _InstanceID = input.InstanceID;
    #define TRANSFER_INSTANCE_ID_VARYING(output) output.InstanceID = _InstanceID;

#else

    #define _ModelMatrix            TransformMatricesBuffer[0]._ModelMatrix
    #define _ModelNormalMatrix      TransformMatricesBuffer[0]._ModelNormalMatrix

    #define DECLARE_INSTANCE_ID_ATTRIBUTE()
    #define DECLARE_INSTANCE_ID_VARYING(varID)

    #define SETUP_INSTANCE_ID(input)
    #define TRANSFER_INSTANCE_ID_VARYING(output)

#endif

#endif // RENDER_ENGINE_PER_DRAW_DATA_H