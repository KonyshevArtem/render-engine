cmake_minimum_required(VERSION 3.19)
project(RenderEngine)

set(CMAKE_CXX_STANDARD 20)

add_executable(
	RenderEngine
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

# boost
# set(Boost_DEBUG ON)
# set(Boost_ARCHITECTURE -x64) # might be required on windows, based on how boost was compiled
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS json)
include_directories(${Boost_INCLUDE_DIRS})

# TODO add non-editor compile target
add_compile_definitions(RENDER_ENGINE_EDITOR)

# source code
add_subdirectory(graphics_backend)
add_subdirectory(external/lodepng)
add_subdirectory(external/glfw)
add_subdirectory(external/metal-cpp)
add_subdirectory(external/metal-cpp-extensions)
add_subdirectory(external/OpenFBX)
add_subdirectory(external/imgui)
add_subdirectory(external/tga)
add_subdirectory(core)
add_subdirectory(math)
add_subdirectory(utils)
add_subdirectory(debug_util)
add_subdirectory(game_window)
target_link_libraries(RenderEngine Core GameWindow)

# copy resources to build directory
add_custom_target(PreBuild ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:RenderEngine>/resources
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:RenderEngine>/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:RenderEngine>/resources
        COMMAND ${CMAKE_COMMAND} -E echo \"[Pre Build] Resources copied\")
add_dependencies(RenderEngine PreBuild)

# texture compressor utility
add_subdirectory(texture_compressor)