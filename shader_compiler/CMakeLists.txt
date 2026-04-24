if (${RENDER_ENGINE_BUILD_TOOLS})

    cmake_minimum_required(VERSION 3.19)
    project(ShaderCompiler)

    add_library(ShaderCompilerLib STATIC 
        include/shader_compiler.h
        src/shader_compiler.cpp
        src/reflection_spirv.h
        src/reflection_dxc.h
        src/reflection_common.h
        src/serialization.h
        src/graphics_backend.h
        src/defines.h
        src/include_handler.h)
    target_include_directories(ShaderCompilerLib PUBLIC include)
    target_link_libraries(ShaderCompilerLib DXC spirv-cross-msl spirv-cross-glsl nlohmann_json::nlohmann_json DebugUtil)

    set(CMAKE_CXX_STANDARD 20)

    add_executable(ShaderCompiler main.cpp)
    target_link_libraries(ShaderCompiler ShaderCompilerLib Arguments StringSplit)

endif ()