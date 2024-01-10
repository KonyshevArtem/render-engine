add_library(GraphicsBackend graphics_backend.cpp graphics_backend.cpp)

# windows
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    set(MINOR_VERSION 6)

    # glew for windows
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(GraphicsBackend ${GLEW_LIBRARIES} glu32.lib opengl32.lib)

# macos
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    set(MINOR_VERSION 1)

    # opengl for macos
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS})
    target_link_libraries(GraphicsBackend ${OPENGL_LIBRARIES})

endif()

set(GRAPHICS_BACKEND_DEFINITIONS GL_SILENCE_DEPRECATION OPENGL_MAJOR_VERSION=4 OPENGL_MINOR_VERSION=${MINOR_VERSION} PARENT_SCOPE)

target_include_directories(GraphicsBackend PUBLIC .)