#ifndef RAYTRACING_COMMON_H
#define RAYTRACING_COMMON_H

#include "../common/global_defines.h"

struct PerInstanceData
{
    uint VerticesBufferIndex;
    uint IndicesBufferIndex;
    uint VertexStride;
    float Padding0;
};

RaytracingAccelerationStructure RTScene : register(RT_SCENE);
StructuredBuffer<PerInstanceData> PerInstanceDataBuffer : register(RT_PER_INSTANCE_DATA);

ByteAddressBuffer VertexAndIndexBuffers[] : register(BINDLESS_RESOURCES);

float3 GetHitWorldNormal(uint instanceIndex, uint primitiveIndex, float3x4 worldToObjectMatrix)
{
    PerInstanceData instanceData = PerInstanceDataBuffer[instanceIndex];
    ByteAddressBuffer vertexBuffer = VertexAndIndexBuffers[instanceData.VerticesBufferIndex];
    ByteAddressBuffer indexBuffer = VertexAndIndexBuffers[instanceData.IndicesBufferIndex];

    uint3 indices = indexBuffer.Load3(primitiveIndex * 3 * 4);
    float3 p1 = asfloat(vertexBuffer.Load3(instanceData.VertexStride * indices.x));
    float3 p2 = asfloat(vertexBuffer.Load3(instanceData.VertexStride * indices.y));
    float3 p3 = asfloat(vertexBuffer.Load3(instanceData.VertexStride * indices.z));

    float3 objectNormal = normalize(cross(p2 - p1, p3 - p1));
    return normalize(mul(objectNormal, (float3x3)worldToObjectMatrix));
}

#endif