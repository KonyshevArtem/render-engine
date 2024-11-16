if (${RENDER_ENGINE_BUILD_TOOLS})

    cmake_minimum_required(VERSION 3.19)
    project(ShaderCompiler)

    set(CMAKE_CXX_STANDARD 20)

    add_executable(ShaderCompiler main.cpp reflection.h serialization.h graphics_backend.h defines.h)
    target_link_libraries(ShaderCompiler DXC spirv-cross-msl spirv-cross-glsl nlohmann_json::nlohmann_json)

endif ()