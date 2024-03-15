add_library(
        GameWindow
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
        game_window_implementations/game_window_opengl.cpp
        game_window_implementations/game_window_opengl.h
        game_window_implementations/game_window_metal.cpp
        game_window_implementations/game_window_metal.h)

add_compile_definitions(GL_SILENCE_DEPRECATION)
target_include_directories(GameWindow PUBLIC .)
target_link_libraries(GameWindow ImGui Core metal-cpp metal-cpp-extensions)