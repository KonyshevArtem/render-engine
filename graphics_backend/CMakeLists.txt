add_library(GraphicsBackend
        graphics_backend.h
        graphics_backend_debug.cpp
        graphics_backend_debug.h
        graphics_backend_api.cpp
        graphics_backend_api.h
        enums/texture_type.h
        enums/texture_parameter.h
        enums/texture_target.h
        enums/texture_level_parameter.h
        enums/texture_internal_format.h
        enums/texture_pixel_format.h
        enums/texture_data_type.h
        enums/texture_unit.h
        enums/framebuffer_target.h
        enums/framebuffer_attachment.h
        enums/sampler_parameter.h
        enums/texture_wrap_mode.h
        enums/texture_filtering_mode.h
        enums/primitive_type.h
        enums/buffer_bind_target.h
        enums/vertex_attribute_data_type.h
        enums/buffer_usage_hint.h
        enums/graphics_backend_capability.h
        enums/blend_factor.h
        enums/cull_face.h
        enums/depth_function.h
        enums/shader_type.h
        enums/shader_parameter.h
        enums/program_parameter.h
        enums/uniform_data_type.h)

# windows
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")

    set(MINOR_VERSION 6)
    set(PLATFORM_DEFINE "GRAPHICS_BACKEND_WINDOWS")

    # glew for windows
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(GraphicsBackend ${GLEW_LIBRARIES} glu32.lib opengl32.lib)

# macos
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    set(MINOR_VERSION 1)
    set(PLATFORM_DEFINE "GRAPHICS_BACKEND_MACOS")

    # opengl for macos
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIRS})
    target_link_libraries(GraphicsBackend ${OPENGL_LIBRARIES})

endif()

target_compile_definitions(GraphicsBackend PUBLIC GL_SILENCE_DEPRECATION ${PLATFORM_DEFINE})
set(GRAPHICS_BACKEND_DEFINITIONS OPENGL_MAJOR_VERSION=4 OPENGL_MINOR_VERSION=${MINOR_VERSION} PARENT_SCOPE)

target_include_directories(GraphicsBackend PUBLIC .)
target_link_libraries(GraphicsBackend DebugUtil)