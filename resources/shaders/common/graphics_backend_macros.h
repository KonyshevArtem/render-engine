#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_MACROS_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_MACROS_H

#if defined(OPENGL_SHADER)

    #define half3x3_type    mediump mat3
    #define half4_type      mediump vec4
    #define half3_type      mediump vec3
    #define half2_type      mediump vec2
    #define half_type       mediump float

    #define float4x4        mat4
    #define float3x3        mat3
    #define half3x3         mat3

    #define float4          vec4
    #define float3          vec3
    #define float2          vec2
    #define half4           vec4
    #define half3           vec3
    #define half2           vec2
    #define half            float

    #define packed_float3   vec3
    #define packed_float2   vec2

    #define ATTRIBUTE(index)
    #define DECLARE_CLIP_POS(type, name)
    #ifdef VERTEX_PROGRAM
        #define OUTPUT_CLIP_POS(name, value)    gl_Position = value;
    #else
        #define OUTPUT_CLIP_POS(name, value)
    #endif

    #define DECLARE_UBO(name) layout(std140) uniform name { name##Struct name##Var; }

    #define SAMPLE_TEXTURE(textureName, samplerName, uv) texture(textureName, uv)
    #define SAMPLE_TEXTURE_ARRAY(textureName, samplerName, uv, index) texture(textureName, vec3(uv, index))
    #define SAMPLE_TEXTURE_LOD(textureName, samplerName, uv, lod) textureLod(textureName, uv, lod)
    #define TEXTURE2D_HALF_PARAMETER(texName, samplerName) in sampler2D texName
    #define TEXTURE2D_FLOAT_PARAMETER(texName, samplerName) in sampler2D texName
    #define TEXTURE2D_ARRAY_FLOAT_PARAMETER(texName, samplerName) in sampler2DArray texName
    #define CUBEMAP_HALF_PARAMETER(cubeName, samplerName) in samplerCube cubeName
    #define PASS_TEXTURE_PARAMETER(texName, samplerName) texName

    #define SCREEN_UV_UPSIDE_DOWN 0

    #define constant const
    #define OUT(type) out type

#elif defined(METAL_SHADER)

    #define ATTRIBUTE(index)                [[attribute(index)]]
    #define DECLARE_CLIP_POS(type, name)    type name [[position]];
    #define OUTPUT_CLIP_POS(name, value)    name = value;

    #define half3x3_type    half3x3
    #define half4_type      half4
    #define half3_type      half3
    #define half2_type      half2
    #define half_type       half

    #define SAMPLE_TEXTURE(textureName, samplerName, uv) textureName.sample(samplerName, uv)
    #define SAMPLE_TEXTURE_ARRAY(textureName, samplerName, uv, index) textureName.sample(samplerName, uv, index)
    #define SAMPLE_TEXTURE_LOD(textureName, samplerName, uv, lod) textureName.sample(samplerName, uv, level(lod))
    #define TEXTURE2D_HALF_PARAMETER(texName, samplerName) texture2d<half> texName, sampler samplerName
    #define TEXTURE2D_FLOAT_PARAMETER(texName, samplerName) texture2d<float> texName, sampler samplerName
    #define TEXTURE2D_ARRAY_FLOAT_PARAMETER(texName, samplerName) texture2d_array<float> texName, sampler samplerName
    #define CUBEMAP_HALF_PARAMETER(cubeName, samplerName) texturecube<half> cubeName, sampler samplerName
    #define PASS_TEXTURE_PARAMETER(texName, samplerName) texName, samplerName

    #define SCREEN_UV_UPSIDE_DOWN 1

    #define OUT(type) thread type&

#endif

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_MACROS_H
