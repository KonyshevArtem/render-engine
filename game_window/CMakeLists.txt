add_library(GameWindow game_window.cpp game_window.h)

# windows
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    # glew for windows
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(GameWindow ${GLEW_LIBRARIES} glu32.lib opengl32.lib)

# macos
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    # glut for macos
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIRS})
    target_link_libraries(GameWindow ${GLUT_LIBRARIES})

endif ()

add_compile_definitions(GL_SILENCE_DEPRECATION)
target_include_directories(GameWindow PUBLIC .)