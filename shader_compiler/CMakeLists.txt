cmake_minimum_required(VERSION 3.19)
project(ShaderCompiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(ShaderCompiler main.cpp reflection.h serialization.h)
target_link_libraries(ShaderCompiler DXC Boost::json)
