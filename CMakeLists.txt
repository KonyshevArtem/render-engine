cmake_minimum_required(VERSION 3.19)
project(OpenGL_Study)

set(CMAKE_CXX_STANDARD 20)

set(
    OPENGL_STUDY_SOURCES
    main.cpp
    game_window.cpp
    game_window.h
    scenes/test_scene.cpp
    scenes/test_scene.h
    scripts/camera_fly_controller/camera_fly_controller.cpp
	scripts/camera_fly_controller/camera_fly_controller.h
)

# windows
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    add_executable(OpenGL_Study WIN32 ${OPENGL_STUDY_SOURCES})
    add_compile_definitions(OPENGL_STUDY_WINDOWS _USE_MATH_DEFINES WIN32_LEAN_AND_MEAN)

    # glew for windows
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(OpenGL_Study ${GLEW_LIBRARIES} glu32.lib opengl32.lib)

# macos
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    add_executable(OpenGL_Study ${OPENGL_STUDY_SOURCES})
    add_compile_definitions(OPENGL_STUDY_MACOS GL_SILENCE_DEPRECATION)

    # opengl for macos
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS})
    target_link_libraries(OpenGL_Study ${OPENGL_LIBRARIES})

endif()

# boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS json)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(OpenGL_Study Boost::json)

# qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL)
target_link_libraries(OpenGL_Study Qt6::Widgets Qt6::OpenGL)

# TODO add non-editor compile target
add_compile_definitions(OPENGL_STUDY_EDITOR)

# source code
add_subdirectory(external/lodepng)
add_subdirectory(external/OpenFBX)
add_subdirectory(core)
add_subdirectory(editor_gui)
add_subdirectory(math)
add_subdirectory(utils)
target_link_libraries(OpenGL_Study Core Editor)

# copy resources to build directory
add_custom_target(PreBuild ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E echo \"[Pre Build] Resources copied\")
add_dependencies(OpenGL_Study PreBuild)
