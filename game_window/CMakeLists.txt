add_library(GameWindow game_window.cpp game_window.h)

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
target_link_libraries(GameWindow ${GLUT_LIBRARIES})

add_compile_definitions(GL_SILENCE_DEPRECATION)
target_include_directories(GameWindow PUBLIC .)