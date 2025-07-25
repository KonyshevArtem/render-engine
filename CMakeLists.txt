cmake_minimum_required(VERSION 3.19)
project(RenderEngine)

set(CMAKE_CXX_STANDARD 20)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

	add_compile_definitions(RENDER_ENGINE_WINDOWS=1 RENDER_ENGINE_APPLE=0 RENDER_ENGINE_ANDROID=0 _USE_MATH_DEFINES)
	set(RENDER_ENGINE_WINDOWS_PLATFORM 1)
	set(ENABLE_IMGUI 1)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

	set(RENDER_ENGINE_MACOS_PLATFORM 1)
	set(ENABLE_IMGUI 1)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "iOS")

	set(RENDER_ENGINE_IOS_PLATFORM 1)
	set(ENABLE_IMGUI 0)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")

	add_compile_definitions(RENDER_ENGINE_WINDOWS=0 RENDER_ENGINE_APPLE=0 RENDER_ENGINE_ANDROID=1)
	set(RENDER_ENGINE_ANDROID_PLATFORM 1)
	set(ENABLE_IMGUI 0)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/launchers/android_launcher/AndroidStudio/EngineFramework/src/main/cpp/libs/${CMAKE_ANDROID_ARCH_ABI}/${CMAKE_BUILD_TYPE}")

endif ()

if ((${RENDER_ENGINE_MACOS_PLATFORM}) OR (${RENDER_ENGINE_IOS_PLATFORM}))

	add_compile_definitions(RENDER_ENGINE_WINDOWS=0 RENDER_ENGINE_APPLE=1 RENDER_ENGINE_ANDROID=0)
	set(RENDER_ENGINE_APPLE_PLATFORM 1)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/launchers/apple_launcher/xcode/EngineFramework/Libraries/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")

endif ()

if ((${RENDER_ENGINE_WINDOWS_PLATFORM}) OR (${RENDER_ENGINE_MACOS_PLATFORM}))

	set(RENDER_ENGINE_BUILD_TOOLS 1)

endif ()

add_library(
	RenderEngine
	STATIC
	engine_framework/engine_framework.h
	engine_framework/engine_framework.cpp
	scripts/camera_fly_controller/camera_fly_controller.cpp
	scripts/camera_fly_controller/camera_fly_controller.h
	scripts/game_components_register.h
	scripts/game_components_register.cpp
	scripts/pbr_demo/pbr_demo_updater.h
	scripts/pbr_demo/pbr_demo_updater.cpp
	scripts/test_scene/flashlight.h
	scripts/test_scene/flashlight.cpp
	scripts/test_scene/test_scene_updater.h
	scripts/test_scene/test_scene_updater.cpp
)

# TODO add non-editor compile target
add_compile_definitions(RENDER_ENGINE_EDITOR)

#launchers source code
add_subdirectory(launchers/windows_launcher)
add_subdirectory(launchers/apple_launcher)
add_subdirectory(launchers/android_launcher)

# engine source code
add_subdirectory(graphics_backend)
add_subdirectory(external)
add_subdirectory(core)
add_subdirectory(math)
add_subdirectory(utils)
add_subdirectory(game_window)
add_subdirectory(shader_compiler)
add_subdirectory(texture_compressor)
add_subdirectory(model_compiler)
target_link_libraries(RenderEngine Core GameWindow ImGuiWrapper Arguments)