cmake_minimum_required(VERSION 3.19)
project(OpenGL_Study)

set(CMAKE_CXX_STANDARD 20)

set(
    OPENGL_STUDY_SOURCES
    main.cpp
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
    target_link_libraries(OpenGL_Study ${GLEW_LIBRARIES})

# macos
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    add_executable(OpenGL_Study ${OPENGL_STUDY_SOURCES})
    add_compile_definitions(OPENGL_STUDY_MACOS GL_SILENCE_DEPRECATION)

    # opengl for macos
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS})
    target_link_libraries(OpenGL_Study ${OPENGL_LIBRARIES})

endif()

# glut
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
target_link_libraries(OpenGL_Study ${GLUT_LIBRARIES})

# boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(OpenGL_Study ${Boost_LIBRARIES})

# source code
add_subdirectory(external/lodepng)
add_subdirectory(external/OpenFBX)
add_subdirectory(core)
add_subdirectory(math)
add_subdirectory(utils)
target_link_libraries(OpenGL_Study Core Utils Math LodePNG OpenFBX)

# copy resources to build directory
add_custom_target(PreBuild ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:OpenGL_Study>/resources
        COMMAND ${CMAKE_COMMAND} -E echo \"[Pre Build] Resources copied\")
add_dependencies(OpenGL_Study PreBuild)
