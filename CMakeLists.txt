cmake_minimum_required(VERSION 3.19)
project(OpenGL_Study)

set(CMAKE_CXX_STANDARD 20)

set(
    OPENGL_STUDY_SOURCES
    main.cpp
    scenes/test_scene.cpp
    scenes/test_scene.h
    scenes/pbr_demo.cpp
    scenes/pbr_demo.h
    scenes/shadows_demo.cpp
    scenes/shadows_demo.h
    scripts/camera_fly_controller/camera_fly_controller.cpp
	scripts/camera_fly_controller/camera_fly_controller.h
)

# windows
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    add_executable(OpenGL_Study WIN32 ${OPENGL_STUDY_SOURCES})
    add_compile_definitions(OPENGL_STUDY_WINDOWS _USE_MATH_DEFINES WIN32_LEAN_AND_MEAN)

# macos
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    add_executable(OpenGL_Study ${OPENGL_STUDY_SOURCES})
    add_compile_definitions(OPENGL_STUDY_MACOS)

endif()

# boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS json)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(OpenGL_Study Boost::json)

# TODO add non-editor compile target
add_compile_definitions(OPENGL_STUDY_EDITOR)

# source code
add_subdirectory(graphics_backend)
add_subdirectory(external/lodepng)
add_subdirectory(external/OpenFBX)
add_subdirectory(external/tga)
add_subdirectory(core)
add_subdirectory(editor_gui)
add_subdirectory(math)
add_subdirectory(utils)
add_subdirectory(debug_util)
add_subdirectory(game_window)
target_link_libraries(OpenGL_Study Core GameWindow)

# copy resources to build directory
add_custom_target(PreBuild ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E echo \"[Pre Build] Resources copied\")
add_dependencies(OpenGL_Study PreBuild)

# texture compressor utility
add_subdirectory(texture_compressor)