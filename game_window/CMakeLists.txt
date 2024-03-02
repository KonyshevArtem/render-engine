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
        windows/windows_id_pool.h)

add_compile_definitions(GL_SILENCE_DEPRECATION)
target_include_directories(GameWindow PUBLIC .)
target_link_libraries(GameWindow ImGui Core)