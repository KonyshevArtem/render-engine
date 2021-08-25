cmake_minimum_required(VERSION 3.19)
project(OpenGL_Study)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        OpenGL_Study
        main.cpp
        utils/utils.cpp
        utils/utils.h
        math/matrix4x4/matrix4x4.cpp
        math/matrix4x4/matrix4x4.h
        math/vector4/vector4.cpp
        math/vector4/vector4.h
        math/vector3/vector3.cpp
        math/vector3/vector3.h
        math/quaternion/quaternion.cpp
        math/quaternion/quaternion.h
        core/gameObject/gameObject.h
        core/gameObject/gameObject.cpp
        math/math_utils.cpp
        math/math_utils.h
        core/mesh/mesh.cpp
        core/mesh/mesh.h
        core/mesh/cube/cube_mesh.cpp
        core/mesh/cube/cube_mesh.h
        core/mesh/cylinder/cylinder_mesh.cpp
        core/mesh/cylinder/cylinder_mesh.h
        core/light/light_data.h
        core/shader/shader.h
        core/shader/shader.cpp
        core/texture/texture.cpp
        core/texture/texture.h
        utils/lodepng.cpp
        utils/lodepng.h
        core/material/material.cpp
        core/material/material.h
        core/mesh/plane/plane_mesh.cpp
        core/mesh/plane/plane_mesh.h
        core/scene/scene.h
        scenes/test_scene.cpp
        scenes/test_scene.h
        core/input/input.cpp
        core/input/input.h)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

target_link_libraries(OpenGL_Study ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})