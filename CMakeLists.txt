cmake_minimum_required(VERSION 3.19)
project(OpenGL_Study)

set(CMAKE_CXX_STANDARD 20)

add_library(
        libpng
        external/lodepng/lodepng.cpp
        external/lodepng/lodepng.h
)

add_library(
        OpenFBX
        external/OpenFBX/ofbx.cpp
        external/OpenFBX/ofbx.h
        external/OpenFBX/miniz.c
        external/OpenFBX/miniz.h
)

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
        core/light/light.h
        core/shader/shader.h
        core/shader/shader.cpp
        core/texture_2d/texture_2d.cpp
        core/texture_2d/texture_2d.h
        core/material/material.cpp
        core/material/material.h
        core/mesh/plane/plane_mesh.cpp
        core/mesh/plane/plane_mesh.h
        core/scene/scene.h
        scenes/test_scene.cpp
        scenes/test_scene.h
        core/scene/scene.cpp
        core/input/input.cpp
        core/input/input.h
        core/graphics/graphics.cpp
        core/graphics/graphics.h
        core/camera/camera.cpp
        core/camera/camera.h
        core/time/time.cpp
        core/time/time.h
        scripts/camera_fly_controller/camera_fly_controller.cpp
        scripts/camera_fly_controller/camera_fly_controller.h
        core/graphics/uniform_block.cpp
        core/graphics/uniform_block.h
        core/graphics/context.cpp
        core/graphics/context.h
        core/graphics/passes/shadow_caster_pass.cpp
        core/graphics/passes/shadow_caster_pass.h
        core/graphics/passes/render_pass.cpp
        core/graphics/passes/render_pass.h
        core/texture_2d_array/texture_2d_array.cpp
        core/texture_2d_array/texture_2d_array.h
        core/cubemap/cubemap.cpp
        core/cubemap/cubemap.h
        core/graphics/passes/skybox_pass.cpp
        core/graphics/passes/skybox_pass.h
)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

target_link_libraries(
        OpenGL_Study
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARY}
        libpng
        OpenFBX
)

add_custom_target(PreBuild ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E echo \"[Pre Build] Resources copied\")

add_dependencies(OpenGL_Study PreBuild)