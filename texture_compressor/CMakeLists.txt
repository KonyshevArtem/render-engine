cmake_minimum_required(VERSION 3.19)
project(TextureCompressor)

set(CMAKE_CXX_STANDARD 20)

set(
    TEXTURE_COMPRESSOR_SOURCES
    texture_compressor.cpp
    texture_compressor_backend.h
    texture_compressor_backend.cpp
    texture_compressor_formats.h
    texture_compressor_formats.cpp
    ../core/texture/texture_header.h
)

# windows
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    add_executable(TextureCompressor ${TEXTURE_COMPRESSOR_SOURCES})
    add_compile_definitions(TEXTURE_COMPRESSOR_WINDOWS)

    # glew for windows
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(TextureCompressor ${GLEW_LIBRARIES} glu32.lib opengl32.lib)

# macos
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    add_executable(TextureCompressor ${TEXTURE_COMPRESSOR_SOURCES})
    add_compile_definitions(TEXTURE_COMPRESSOR_MACOS GL_SILENCE_DEPRECATION)

    # opengl for macos
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
    target_link_libraries(TextureCompressor ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

endif()

target_link_libraries(TextureCompressor LodePNG tga-lib DebugUtil GameWindow)
