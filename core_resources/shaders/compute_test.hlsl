struct TestStruct
{
    float A;
    float B;
};

Buffer<float> TypedBufferIn : register(t0, space1);
RWBuffer<float> TypedBufferOut : register(u0, space1);

StructuredBuffer<TestStruct> StructuredBufferIn : register(t1, space1);
RWStructuredBuffer<TestStruct> StructuredBufferOut : register(u1, space1);

ByteAddressBuffer ByteBufferIn : register(t2, space1);
RWByteAddressBuffer ByteBufferOut : register(u2, space1);

Texture2D<float> TextureIn : register(t0);
RWTexture2D<float> TextureOut : register(u0);

[numthreads(64, 1, 1)]
void computeMain(uint3 DTid : SV_DispatchThreadID)
{
    float f = TypedBufferIn[DTid.x];    
    TypedBufferOut[DTid.x] = f * 2;
    
    TestStruct s = StructuredBufferIn[DTid.x];
    s.A *= 2;
    s.B *= 2;
    StructuredBufferOut[DTid.x] = s;
    
    f = asfloat(ByteBufferIn.Load(DTid.x * 4));
    ByteBufferOut.Store(DTid.x * 4, asuint(f * 2));

    float t = TextureIn[uint2(DTid.x / 8, DTid.x % 8)];
    TextureOut[uint2(DTid.x / 8, DTid.x % 8)] = t * 2;
}