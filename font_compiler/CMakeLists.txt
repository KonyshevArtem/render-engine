if (${RENDER_ENGINE_BUILD_TOOLS})

    cmake_minimum_required(VERSION 3.19)
    project(FontCompiler)

    set(CMAKE_CXX_STANDARD 20)

    add_executable(
        FontCompiler
        font_compiler.cpp
        ../core/texture/texture_header.h
    )
    target_link_libraries(FontCompiler GraphicsBackend Arguments Cuttlefish::lib)

endif ()