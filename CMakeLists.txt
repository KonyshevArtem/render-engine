cmake_minimum_required(VERSION 3.19)
project(OpenGL_Study)

set(CMAKE_CXX_STANDARD 11)

add_executable(
        OpenGL_Study
        main.cpp
        shaders/shader_loader.h
        shaders/shader_loader.cpp
        utils/utils.cpp
        utils/utils.h
        math/matrix4x4/matrix4x4.cpp
        math/matrix4x4/matrix4x4.h
        math/vector4/vector4.cpp
        math/vector4/vector4.h
        math/quaternion/quaternion.cpp
        math/quaternion/quaternion.h
        core/gameObject/gameObject.h
        core/gameObject/gameObject.cpp
        math/math_utils.cpp
        math/math_utils.h)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

target_link_libraries(OpenGL_Study ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})