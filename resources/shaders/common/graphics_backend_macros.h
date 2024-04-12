#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_MACROS_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_MACROS_H

#if defined(OPENGL_SHADER)

    #define float4x4        mat4
    #define float4          vec4
    #define packed_float3   vec3
    #define float3          vec3
    #define half4_type      mediump vec4
    #define half4           vec4
    #define half3_type      mediump vec3
    #define half3           vec3

    #define ATTRIBUTE(index)
    #define DECLARE_CLIP_POS(type, name)
    #ifdef VERTEX_PROGRAM
        #define OUTPUT_CLIP_POS(name, value)    gl_Position = value;
    #else
        #define OUTPUT_CLIP_POS(name, value)
    #endif

    #define DECLARE_UBO(name) layout(std140) uniform name { name##Struct name##Var; }

    #define SAMPLE_TEXTURE(textureName, samplerName, uv) texture(textureName, uv)
    #define SAMPLE_TEXTURE_LOD(textureName, samplerName, uv, lod) textureLod(textureName, uv, lod)
    #define CUBEMAP_HALF_PARAMETER(cubeName, samplerName) in samplerCube cubeName

#elif defined(METAL_SHADER)

    #define ATTRIBUTE(index)                [[attribute(index)]]
    #define DECLARE_CLIP_POS(type, name)    type name [[position]];
    #define OUTPUT_CLIP_POS(name, value)    name = value;

    #define half4_type half4
    #define half3_type half3

    #define SAMPLE_TEXTURE(textureName, samplerName, uv) textureName.sample(samplerName, uv)
    #define SAMPLE_TEXTURE_LOD(textureName, samplerName, uv, lod) textureName.sample(samplerName, uv, level(lod))
    #define CUBEMAP_HALF_PARAMETER(cubeName, samplerName) texturecube<half> cubeName, sampler samplerName

#endif

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_MACROS_H
