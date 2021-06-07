cmake_minimum_required(VERSION 3.19)
project(OpenGL_Study C)

set(CMAKE_C_STANDARD 99)

add_executable(
        OpenGL_Study
        main.c
        shaders/shader_loader.h
        shaders/shader_loader.c
        utils/utils.c
        utils/utils.h
        math/matrix4x4/matrix4x4.c
        math/matrix4x4/matrix4x4.h
        math/vector4/vector4.c
        math/vector4/vector4.h
        math/quaternion/quaternion.c
        math/quaternion/quaternion.h
        core/gameObject/gameObject.c
        core/gameObject/gameObject.h)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

target_link_libraries(OpenGL_Study ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})