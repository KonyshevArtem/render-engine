#ifndef RENDER_ENGINE_GRAPHICS_BACKEND_MACROS_H
#define RENDER_ENGINE_GRAPHICS_BACKEND_MACROS_H

#if defined(OPENGL_SHADER)

    #define float4x4        mat4
    #define float4          vec4
    #define packed_float3   vec3
    #define float3          vec3
    #define half4_type      mediump vec4
    #define half4           vec4

    #define ATTRIBUTE(index)
    #define SV_POSITION

    #define DECLARE_UBO(name) layout(std140) uniform name { name##Struct Data; } name

#elif defined(METAL_SHADER)

    #define ATTRIBUTE(index)    [[attribute(index)]]
    #define SV_POSITION         [[position]]

    #define half4_type half4

#endif

#endif //RENDER_ENGINE_GRAPHICS_BACKEND_MACROS_H
