#include "../common/global_defines.h"
#include "../common/camera_data.h"
#include "../common/lighting.h"

struct PerInstanceData
{
    uint VerticesBufferIndex;
    uint IndicesBufferIndex;
    uint VertexStride;
    float Padding0;
};

RaytracingAccelerationStructure RTScene : register(t0, space1);
StructuredBuffer<PerInstanceData> PerInstanceDataBuffer : register(t1, space1);

ByteAddressBuffer VertexAndIndexBuffers[] : register(t0, space2);

cbuffer Data : register(b0)
{
    float4x4 InvVPMatrix;

    uint2 TargetSize;
    float2 Padding0;
};

float3 GetPixelWorldDirection(float2 pixelCoord)
{
    float2 ndc = pixelCoord / TargetSize * 2 - 1;
#if SCREEN_UV_UPSIDE_DOWN
    ndc.y *= -1;
#endif

    float4 near = mul(InvVPMatrix, float4(ndc, 0, 1));
    float4 far = mul(InvVPMatrix, float4(ndc, 1, 1));

    near = near / near.w;
    far = far / far.w;

    return normalize(far.xyz - near.xyz);
}

float3 GetWorldNormal(uint instanceIndex, uint primitiveIndex, float3x4 worldToObjectMatrix)
{
    PerInstanceData instanceData = PerInstanceDataBuffer[instanceIndex];
    ByteAddressBuffer vertexBuffer = VertexAndIndexBuffers[instanceData.VerticesBufferIndex];
    ByteAddressBuffer indexBuffer = VertexAndIndexBuffers[instanceData.IndicesBufferIndex];
        
    uint3 indices = indexBuffer.Load3(primitiveIndex * 3 * 4);
    float3 p1 = asfloat(vertexBuffer.Load3(instanceData.VertexStride * indices.x));
    float3 p2 = asfloat(vertexBuffer.Load3(instanceData.VertexStride * indices.y));
    float3 p3 = asfloat(vertexBuffer.Load3(instanceData.VertexStride * indices.z));
        
    float3 objectNormal = normalize(cross(p2 - p1, p3 - p1));
    return normalize(mul(objectNormal, (float3x3) worldToObjectMatrix));
}

struct Attributes
{
    float3 positionOS : POSITION;
};

float4 vertexMain(Attributes attributes) : SV_Position
{
    return float4(attributes.positionOS.xyz, 1);
}

float4 fragmentMain(float4 pixelCoord : SV_Position) : SV_Target
{
    RayDesc ray;
    ray.Origin = _CameraPosWS;
    ray.Direction = GetPixelWorldDirection(pixelCoord.xy);
    ray.TMin = _NearClipPlane;
    ray.TMax = _FarClipPlane;

    RayQuery<RAY_FLAG_FORCE_OPAQUE> query;

    query.TraceRayInline(RTScene, RAY_FLAG_NONE, 0xFF, ray);
    while (query.Proceed()){}

    float3 colors[6] =
    {
        float3(1, 0, 0),
        float3(0, 1, 0),
        float3(0, 0, 1),
        float3(1, 1, 0),
        float3(1, 0, 1),
        float3(0, 0, 1)
    };

    if (query.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    {
        float3 worldPos = ray.Origin + ray.Direction * query.CommittedRayT();
        float3 worldNormal = GetWorldNormal(query.CommittedInstanceIndex(), query.CommittedPrimitiveIndex(), query.CommittedWorldToObject3x4());
        
        float3 light = getLightPBR(worldPos, worldNormal, colors[query.CommittedInstanceID() % 6].xyz, 1, 0, float3(0, 0, 0), _CameraPosWS);
        return float4(light, 1);
    }
    
    return float4(0, 0, 0, 0);
}