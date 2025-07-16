if (${RENDER_ENGINE_BUILD_TOOLS})

    cmake_minimum_required(VERSION 3.19)
    project(ModelCompiler)

    set(CMAKE_CXX_STANDARD 20)

    set(
            MODEL_COMPILER_SOURCES
            model_compiler.cpp
            ../core/mesh/mesh_header.h
    )

    add_executable(ModelCompiler ${MODEL_COMPILER_SOURCES})
    target_link_libraries(ModelCompiler Arguments OpenFBX Math)

endif ()