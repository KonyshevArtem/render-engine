cmake_minimum_required(VERSION 3.19)
project(RenderEngine)

set(CMAKE_CXX_STANDARD 20)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

	add_compile_definitions(RENDER_ENGINE_WINDOWS=1 RENDER_ENGINE_APPLE=0)
	set(RENDER_ENGINE_WINDOWS_PLATFORM 1)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

	add_compile_definitions(RENDER_ENGINE_WINDOWS=0 RENDER_ENGINE_APPLE=1)
	set(RENDER_ENGINE_APPLE_PLATFORM 1)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/launchers/apple_launcher/xcode/EngineFramework/Libraries")

endif ()

add_library(
	RenderEngine
	STATIC
	engine_framework/engine_framework.h
	engine_framework/engine_framework.cpp
    scenes/test_scene.cpp
    scenes/test_scene.h
    scenes/pbr_demo.cpp
    scenes/pbr_demo.h
    scenes/shadows_demo.cpp
    scenes/shadows_demo.h
    scripts/camera_fly_controller/camera_fly_controller.cpp
	scripts/camera_fly_controller/camera_fly_controller.h
	scenes/empty_scene.cpp
	scenes/empty_scene.h
)

# boost
# set(Boost_DEBUG ON)
# set(Boost_ARCHITECTURE -x64) # might be required on windows, based on how boost was compiled
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS json)
include_directories(${Boost_INCLUDE_DIRS})

# TODO add non-editor compile target
add_compile_definitions(RENDER_ENGINE_EDITOR)

#launchers source code
add_subdirectory(launchers/windows_launcher)
add_subdirectory(launchers/apple_launcher)

# engine source code
add_subdirectory(graphics_backend)
add_subdirectory(external/lodepng)
add_subdirectory(external/glfw)
add_subdirectory(external/metal-cpp)
add_subdirectory(external/OpenFBX)
add_subdirectory(external/imgui)
add_subdirectory(external/tga)
add_subdirectory(external/dxc)
add_subdirectory(external/SPIRV-Cross)
add_subdirectory(external/Cuttlefish)
add_subdirectory(core)
add_subdirectory(math)
add_subdirectory(utils)
add_subdirectory(debug_util)
add_subdirectory(game_window)
add_subdirectory(shader_compiler)
add_subdirectory(texture_compressor)
target_link_libraries(RenderEngine Core GameWindow)