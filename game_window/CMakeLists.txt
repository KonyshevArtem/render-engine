add_library(GameWindow game_window.cpp game_window.h)

target_include_directories(GameWindow PUBLIC .)
target_link_libraries(GameWindow)