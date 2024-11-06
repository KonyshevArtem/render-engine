add_library(
        GameWindow
        STATIC
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

target_include_directories(GameWindow PUBLIC .)
target_link_libraries(GameWindow Core)

if (${ENABLE_IMGUI})
    target_link_libraries(GameWindow ImGui)
    target_compile_definitions(GameWindow ENABLE_IMGUI)
endif()