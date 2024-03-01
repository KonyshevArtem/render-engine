add_library(GameWindow game_window.cpp game_window.h)

add_compile_definitions(GL_SILENCE_DEPRECATION)
target_include_directories(GameWindow PUBLIC .)
target_link_libraries(GameWindow ImGui GraphicsBackend)