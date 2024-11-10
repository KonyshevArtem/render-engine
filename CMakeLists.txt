cmake_minimum_required(VERSION 3.19)
project(RenderEngine)

set(CMAKE_CXX_STANDARD 20)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

	add_compile_definitions(RENDER_ENGINE_WINDOWS=1 RENDER_ENGINE_APPLE=0 _USE_MATH_DEFINES)
	set(RENDER_ENGINE_WINDOWS_PLATFORM 1)
	set(ENABLE_IMGUI 1)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

	set(RENDER_ENGINE_MACOS_PLATFORM 1)
	set(ENABLE_IMGUI 1)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "iOS")

	set(RENDER_ENGINE_IOS_PLATFORM 1)
	set(ENABLE_IMGUI 0)

endif ()

if ((${RENDER_ENGINE_MACOS_PLATFORM}) OR (${RENDER_ENGINE_IOS_PLATFORM}))

	add_compile_definitions(RENDER_ENGINE_WINDOWS=0 RENDER_ENGINE_APPLE=1)
	set(RENDER_ENGINE_APPLE_PLATFORM 1)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/launchers/apple_launcher/xcode/EngineFramework/Libraries/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")

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
)

# TODO add non-editor compile target
add_compile_definitions(RENDER_ENGINE_EDITOR)

#launchers source code
add_subdirectory(launchers/windows_launcher)
add_subdirectory(launchers/apple_launcher)

# engine source code
add_subdirectory(graphics_backend)
add_subdirectory(external)
add_subdirectory(core)
add_subdirectory(math)
add_subdirectory(debug_util)
add_subdirectory(game_window)
add_subdirectory(shader_compiler)
add_subdirectory(texture_compressor)
target_link_libraries(RenderEngine Core GameWindow)