cmake_minimum_required(VERSION 3.19)
project(TextureCompressor)

set(CMAKE_CXX_STANDARD 20)

set(CUTTLEFISH_BUILD_TESTS OFF)
set(CUTTLEFISH_BUILD_TOOL OFF)

set(
    TEXTURE_COMPRESSOR_SOURCES
    texture_compressor.cpp
    texture_compressor_backend.h
    texture_compressor_backend.cpp
    texture_compressor_formats.h
    texture_compressor_formats.cpp
    ../core/texture/texture_header.h
)

add_executable(TextureCompressor ${TEXTURE_COMPRESSOR_SOURCES})
target_link_libraries(TextureCompressor GraphicsBackend Cuttlefish::lib)
target_include_directories(TextureCompressor PUBLIC ${Cuttlefish_INCLUDE_DIRS})
