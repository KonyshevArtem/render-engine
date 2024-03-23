list(
        APPEND
        GAME_WINDOW_SOURCES
        game_window.cpp
        game_window.h
        top_menu_bar.cpp
        top_menu_bar.h
        windows/base_window.cpp
        windows/base_window.h
        windows/debug_console_window.cpp
        windows/debug_console_window.h
        windows/windows_id_pool.cpp
        windows/windows_id_pool.h
        windows/scene_hierarchy_window.cpp
        windows/scene_hierarchy_window.h
        window_manager.cpp
        window_manager.h
        windows/graphics_settings_window.cpp
        windows/graphics_settings_window.h
)

if (${RENDER_ENGINE_WINDOWS_PLATFORM})
    list(
            APPEND
            GAME_WINDOW_SOURCES
            game_window_implementations/game_window_platform_windows.cpp
            game_window_implementations/game_window_platform_windows.h
    )
elseif (${RENDER_ENGINE_APPLE_PLATFORM})
    list(
            APPEND
            GAME_WINDOW_SOURCES
            game_window_implementations/game_window_platform_apple.cpp
            game_window_implementations/game_window_platform_apple.h
    )
endif ()

add_library(GameWindow STATIC ${GAME_WINDOW_SOURCES})

add_compile_definitions(GL_SILENCE_DEPRECATION)
target_include_directories(GameWindow PUBLIC .)
target_link_libraries(GameWindow ImGui Core)